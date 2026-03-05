#include "WFCSubsystem.h"
#include "RoomBase.h"
#include "Engine/World.h"
#include "Engine/Engine.h"
#include "ObjectTrace.h"

#include "DataCollector.h"

//JSON includes
#include "Dom/JsonObject.h"
#include "Serialization/JsonWriter.h"
#include "Serialization/JsonSerializer.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"
#include "JsonObjectConverter.h"

//World generation includes
#include "Math/UnrealMathUtility.h"

UWFCSubsystem::UWFCSubsystem()
{
	GridSize = 4;
}

void UWFCSubsystem::AlgorithmSolver()
{
	//temporary start point for easier testing
	int32 StartPoint = GridSize * 0.5f;
	TSubclassOf<ARoomBase> StartTile = Grid[StartPoint][StartPoint].AvailableCellKeys[0];
	Grid[StartPoint][StartPoint].AvailableCellKeys.Empty();
	Grid[StartPoint][StartPoint].AvailableCellKeys.Add(StartTile);
	Grid[StartPoint][StartPoint].IsFullyCollapsed = true;
	
	int Iterator = 0;
	int LoopLimit = 1000;
	while (!IsGridFull() || Iterator < LoopLimit)
	{
		for (int x = 0; x < GridSize; x++)
		{
			for (int y = 0; y < GridSize; y++)
			{
				CollapseCell(x, y);
			}
		}

		//Infinite loop prevention
		if (Iterator > LoopLimit)
			break;
		Iterator++;
	}
}


void UWFCSubsystem::CollapseCell(int32 x, int32 y)
{
	//Prevents collapse of rooms already collapsed
	if (Grid[x][y].IsFullyCollapsed)
		return;

	bool HasBeenCollapsed = false;

	//used to adjust index for neighbours
	TArray<FVector2D> Direction;
	Direction.SetNum(4);
	Direction[0] = (FVector2D(-1, 0));
	Direction[1] = (FVector2D(1, 0));
	Direction[2] = (FVector2D(0, 1));
	Direction[3] = (FVector2D(0, -1));
	
	for (int i = 0; i < Direction.Num(); i++)
	{
		int32 NeighbourX = (int32)Direction[i].X + x;
		int32 NeighbourY = (int32)Direction[i].Y + y;

		if (!Grid.IsValidIndex(NeighbourX) || !Grid[NeighbourX].IsValidIndex(NeighbourY))
			continue;

		if (Grid[NeighbourX][NeighbourY].AvailableCellKeys.IsEmpty())
		{
			UE_LOG(LogTemp, Warning, TEXT("WFC Algorithm failed at position:  %d , %d "), NeighbourX, NeighbourY);
			continue;
		}

		//Collecting the requirements of neighbouring rooms
		TArray<TSubclassOf<ARoomBase>> AvailableCells;
		for (TSubclassOf<ARoomBase> NeighbourRoomKey : Grid[NeighbourX][NeighbourY].AvailableCellKeys)
		{
			for (TSubclassOf<ARoomBase> Key : AdjacencyRules[NeighbourRoomKey].NeighbourCells[i].Row)
			{
				if (AvailableCells.Contains(Key))
					continue;

				AvailableCells.Add(Key);
			}
		}

		//collecting rooms that do not accomodate neighbour requirements
		TArray<TSubclassOf<ARoomBase>> KeysToBeRemoved;
		for (TSubclassOf<ARoomBase> CellKey : Grid[x][y].AvailableCellKeys)
		{
			if (AvailableCells.Contains(CellKey))
				continue;

			KeysToBeRemoved.Add(CellKey);
			HasBeenCollapsed = true;
		}

		//removing rooms that do not accomodate neighbour requirements
		for (TSubclassOf<ARoomBase> CellKey : KeysToBeRemoved)
		{
			Grid[x][y].AvailableCellKeys.Remove(CellKey);
		}
		
		if (Grid[x][y].AvailableCellKeys.Num() == 1)
			Grid[x][y].IsFullyCollapsed = true;
		else if (Grid[x][y].AvailableCellKeys.IsEmpty())
			UE_LOG(LogTemp, Warning, TEXT("WFC Algorithm failed at position:  %d , %d "), (NeighbourX), (NeighbourY));

		//Collapse neighbours if the available room array shrunk
		if (HasBeenCollapsed)
		{
			CollapseCell(NeighbourX, NeighbourY);
		}
	}
}

bool UWFCSubsystem::IsGridFull()
{
	for (int x = 0; x < GridSize; x++)
	{
		for (int y = 0; y < GridSize; y++)
		{
			if (!Grid[x][y].IsFullyCollapsed)
				return false;
		}
	}
	return true;
}

void UWFCSubsystem::PopulateGrid()
{
	Grid.SetNum(GridSize);

	for (int x = 0; x < GridSize; x++)
	{
		Grid[x].SetNum(GridSize);
		for (int y = 0; y < GridSize; y++)
		{
			for (TPair<TSubclassOf<ARoomBase>, FCellData>& Pair : AdjacencyRules)
			{
				Grid[x][y].AvailableCellKeys.Add(Pair.Key);
			}
		}
	}
}

void UWFCSubsystem::LoadAdjacencyRules()
{
	FString FilePath = FPaths::ProjectSavedDir() / TEXT("MyData.json");

	FString JsonString;
	if (!FFileHelper::LoadFileToString(JsonString, *FilePath))
	{
		UE_LOG(LogTemp, Warning, TEXT("Json could not be accessed"));
		return;
	}

	TSharedPtr<FJsonObject> JsonObject;
	if (!FJsonSerializer::Deserialize(TJsonReaderFactory<>::Create(JsonString), JsonObject))
	{
		UE_LOG(LogTemp, Warning, TEXT("Json could not be parsed into Json object"));
		return;
	}

	for (auto& JsonRoomType : JsonObject->Values)
	{
		TSubclassOf<ARoomBase> RoomName = LoadClass<ARoomBase>(nullptr, *JsonRoomType.Key);;

		TArray<TSharedPtr<FJsonValue>> RoomArray = JsonRoomType.Value->AsArray();

		FJSonCellData RoomData;
		if (FJsonObjectConverter::JsonObjectToUStruct(RoomArray[0]->AsObject().ToSharedRef(), &RoomData, 0, 0))
		{
			FCellData FormatedRoomData;
			FormatedRoomData.CellClass = RoomData.CellClass;
			FormatedRoomData.NeighbourCells.SetNum(4);
			FormatedRoomData.NeighbourCells[0].Row.Append(RoomData.Forward);
			FormatedRoomData.NeighbourCells[1].Row.Append(RoomData.Back);
			FormatedRoomData.NeighbourCells[2].Row.Append(RoomData.Left);
			FormatedRoomData.NeighbourCells[3].Row.Append(RoomData.Right);

			AdjacencyRules.Add(RoomName, FormatedRoomData);
		}
	}
	UE_LOG(LogTemp, Log, TEXT("Loaded %d room types"), AdjacencyRules.Num());


}

void UWFCSubsystem::SpawnGrid()
{
}

void UWFCSubsystem::OnWorldBeginPlay(UWorld& InWorld)
{
	LoadAdjacencyRules();
	PopulateGrid();
	AlgorithmSolver();
}

bool UWFCSubsystem::ShouldCreateSubsystem(UObject* Outer) const
{
	UWorld* World = Cast<UWorld>(Outer);
	if (World)
	{
		/*for (const TSoftObjectPtr<UWorld>& WorldAsset : AllowedWorlds)
		{
			if (WorldAsset.GetAssetName() == World->GetName())
			{
				return true;
			}
		}*/

		//Lambda function same as the above
		//simply using Contains() does not work as UWorld is not a TSoftObjectPtr
		return AllowedWorlds.ContainsByPredicate([World](const TSoftObjectPtr<UWorld>& WorldAsset)
			{
				return WorldAsset.GetAssetName() == World->GetName();
			});
	}
	return false;
}

void UWFCSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	UE_LOG(LogTemp, Warning, TEXT("WFCWorldSubSystem::Initialize"));
}

void UWFCSubsystem::Deinitialize()
{
	UE_LOG(LogTemp, Warning, TEXT("WFCWorldSubSystem::Deinitialize"));

	Super::Deinitialize();

}