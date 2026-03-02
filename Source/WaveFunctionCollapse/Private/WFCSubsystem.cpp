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
	TSubclassOf<ARoomBase> StartTile = Grid[StartPoint][StartPoint].AvailableRoomKeys[0];
	Grid[StartPoint][StartPoint].AvailableRoomKeys.Empty();
	Grid[StartPoint][StartPoint].AvailableRoomKeys.Add(StartTile);
	Grid[StartPoint][StartPoint].IsFullyCollapsed = true;
	
	int Iterator = 0;
	int LoopLimit = 1000;
	while (!IsGridFull() || Iterator<LoopLimit)
	{
		for (int x = 0; x < GridSize; x++)
		{
			for (int y = 0; y < GridSize; y++)
			{
				CollapseCell(x, y);
			}
		}
		if (Iterator > LoopLimit)
			break;
		Iterator++;
	}
}

void UWFCSubsystem::CollapseCell(int32 x, int32 y)
{
	//if (Grid[x][y].IsFullyCollapsed)
	//	return;

	bool HasBeenCollapsed = false;

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

		if (Grid[NeighbourX][NeighbourY].AvailableRoomKeys.IsEmpty())
		{
			UE_LOG(LogTemp, Warning, TEXT("WFC Algorithm failed at position:  %d , %d "), NeighbourX, NeighbourY);
			continue;
		}

		TArray<TSubclassOf<ARoomBase>> AvailableRooms;
		for (TSubclassOf<ARoomBase> NeighbourRoomKey : Grid[NeighbourX][NeighbourY].AvailableRoomKeys)
		{
			for (TSubclassOf<ARoomBase> Key : AdjacencyRules[NeighbourRoomKey].Neighbours[i].Row)
			{
				if (AvailableRooms.Contains(Key))
					continue;

				AvailableRooms.Add(Key);
			}
		}

		TArray<TSubclassOf<ARoomBase>> KeysToBeRemoved;
		for (TSubclassOf<ARoomBase> RoomKey : Grid[x][y].AvailableRoomKeys)
		{
			if (AvailableRooms.Contains(RoomKey))
				continue;

			KeysToBeRemoved.Add(RoomKey);
			HasBeenCollapsed = true;
		}

		for (TSubclassOf<ARoomBase> RoomKey : KeysToBeRemoved)
		{
			Grid[x][y].AvailableRoomKeys.Remove(RoomKey);
		}
		
		if (Grid[x][y].AvailableRoomKeys.Num() == 1)
			Grid[x][y].IsFullyCollapsed = true;
		else if (Grid[x][y].AvailableRoomKeys.IsEmpty())
			UE_LOG(LogTemp, Warning, TEXT("WFC Algorithm failed at position:  %d , %d "), (NeighbourX), (NeighbourY));


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
			for (TPair<TSubclassOf<ARoomBase>, FRoomData>& Pair : AdjacencyRules)
			{
				Grid[x][y].AvailableRoomKeys.Add(Pair.Key);
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

		FJSonRoomData RoomData;
		if (FJsonObjectConverter::JsonObjectToUStruct(RoomArray[0]->AsObject().ToSharedRef(), &RoomData, 0, 0))
		{
			FRoomData FormatedRoomData;
			FormatedRoomData.RoomClass = RoomData.RoomClass;
			FormatedRoomData.Neighbours.SetNum(4);
			FormatedRoomData.Neighbours[0].Row.Append(RoomData.Forward);
			FormatedRoomData.Neighbours[1].Row.Append(RoomData.Back);
			FormatedRoomData.Neighbours[2].Row.Append(RoomData.Left);
			FormatedRoomData.Neighbours[3].Row.Append(RoomData.Right);

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