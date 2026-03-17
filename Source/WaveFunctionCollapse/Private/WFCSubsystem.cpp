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

	IndexOffset.SetNum(4);
	IndexOffset[0] = (FIntPoint(-1, 0));
	IndexOffset[1] = (FIntPoint(1, 0));
	IndexOffset[2] = (FIntPoint(0, -1));
	IndexOffset[3] = (FIntPoint(0, 1));

	CellOffset = 300;
}

void UWFCSubsystem::AlgorithmSolver()
{
	//temporary start point for easier testing
	int32 StartPoint = GridSize * 0.5f;
	AssignRandomWeightedRoom(StartPoint, StartPoint);

	int Iterator = 0;
	int LoopLimit = 10000;

	while (!IsGridFull())
	{
		FIntPoint LowestEntropyIndex = FindLowestEntropy();

		AssignRandomWeightedRoom(LowestEntropyIndex.X, LowestEntropyIndex.Y);

		//Infinite loop prevention
		if (Iterator > LoopLimit)
		{
			UE_LOG(LogTemp, Warning, TEXT("WFC Algorithm Infinite Loop Hit %d "), Iterator);
			break;
		}
		Iterator++;
	}
}

void UWFCSubsystem::CollapseCell(int32 x, int32 y)
{
	//if (Grid[x][y].IsFullyCollapsed)
		//return;

	bool HasBeenCollapsed = false;
	
	for (int i = 0; i < IndexOffset.Num(); i++)
	{
		int32 NeighbourX = IndexOffset[i].X + x;
		int32 NeighbourY = IndexOffset[i].Y + y;

		if (!Grid.IsValidIndex(NeighbourX) || !Grid[NeighbourX].IsValidIndex(NeighbourY))
			continue;

		if (Grid[NeighbourX][NeighbourY].AvailableCellKeys.IsEmpty())
			continue;

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
		{
			Grid[x][y].IsFullyCollapsed = true;
			UE_LOG(LogTemp, Warning, TEXT("WFC Algorithm failed at position:  %d , %d "), x, y);
		}
	}
	UpdateEntropy(x, y);

	//Collapse neighbours if the available room array shrunk and update its entropy value
	if (HasBeenCollapsed)
	{
		for (int i = 0; i < IndexOffset.Num(); i++)
		{
			int32 NeighbourX = IndexOffset[i].X + x;
			int32 NeighbourY = IndexOffset[i].Y + y;
			if (!Grid.IsValidIndex(NeighbourX) || !Grid[NeighbourX].IsValidIndex(NeighbourY))
				continue;

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

FIntPoint UWFCSubsystem::FindLowestEntropy()
{
	FIntPoint LowestEntropyIndex = FIntPoint(0, 0);
	int32 LowestEntropy = 999999; //To Do: Remove temp value

	for (int x = 0; x < GridSize; x++)
	{
		for (int y = 0; y < GridSize; y++)
		{
			if (Grid[x][y].IsFullyCollapsed || Grid[x][y].AvailableCellKeys.IsEmpty())
				continue;

			if (Grid[x][y].Entropy < LowestEntropy)
			{
				LowestEntropyIndex = FIntPoint(x, y);
				LowestEntropy = Grid[x][y].Entropy;
			}
		}
	}
	return LowestEntropyIndex;
}

void UWFCSubsystem::UpdateEntropy(int32 x, int32 y)
{
	int32 TotalWeight = 0;
	for (TSubclassOf<ARoomBase> CellKey : Grid[x][y].AvailableCellKeys)
	{
		TotalWeight += AdjacencyRules[CellKey].Weight;
	}
	Grid[x][y].Entropy = TotalWeight;
}

void UWFCSubsystem::AssignRandomWeightedRoom(int32 x, int32 y)
{
	int32 TotalWeight = 0;

	for (TSubclassOf<ARoomBase> Room : Grid[x][y].AvailableCellKeys)
	{
		TotalWeight += AdjacencyRules[Room].Weight;
	}

	int32 Rand = FMath::RandRange(0, TotalWeight);

	for (TSubclassOf<ARoomBase> Room : Grid[x][y].AvailableCellKeys)
	{
		if (Rand > AdjacencyRules[Room].Weight)
		{
			Rand -= AdjacencyRules[Room].Weight;
			continue;
		}

		Grid[x][y].AvailableCellKeys.Empty();
		Grid[x][y].AvailableCellKeys.Add(Room);
		Grid[x][y].IsFullyCollapsed = true;
		UpdateEntropy(x, y);

		for (int i = 0; i < IndexOffset.Num(); i++)
		{
			int32 NeighbourX = IndexOffset[i].X + x;
			int32 NeighbourY = IndexOffset[i].Y + y;

			if (Grid.IsValidIndex(NeighbourX) && Grid[NeighbourX].IsValidIndex(NeighbourY))
			{
				CollapseCell(NeighbourX, NeighbourY);
			}
		}

		break;
	}
}

void UWFCSubsystem::SpawnGrid()
{
	for (int x = 0; x < GridSize; x++)
	{
		for (int y = 0; y < GridSize; y++)
		{
			if (Grid[x][y].AvailableCellKeys.IsEmpty())
				continue;

			FVector Position = FVector(x * CellOffset, y * CellOffset, 0);
			FRotator Rotation(0.0f, 0.0f, 0.0f);
			ARoomBase* Room = GetWorld()->SpawnActor<ARoomBase>(Grid[x][y].AvailableCellKeys[0], Position, Rotation);
			FString Name = FString::Printf(TEXT("Room: %d,%d"), x, y);
			Cast<AActor>(Room)->SetActorLabel(Name);
		}
	}
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
			UpdateEntropy(x, y);
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
			//loaded in reverse order to match the way the grid is generated
			FormatedRoomData.NeighbourCells[3].Row.Append(RoomData.Forward);
			FormatedRoomData.NeighbourCells[2].Row.Append(RoomData.Back);
			FormatedRoomData.NeighbourCells[1].Row.Append(RoomData.Left);
			FormatedRoomData.NeighbourCells[0].Row.Append(RoomData.Right);
			FormatedRoomData.Weight = RoomData.Weight;
			AdjacencyRules.Add(RoomName, FormatedRoomData);
		}
	}
	UE_LOG(LogTemp, Log, TEXT("Loaded %d room types"), AdjacencyRules.Num());


}

void UWFCSubsystem::OnWorldBeginPlay(UWorld& InWorld)
{
	LoadAdjacencyRules();
	PopulateGrid();
	AlgorithmSolver();
	SpawnGrid();
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