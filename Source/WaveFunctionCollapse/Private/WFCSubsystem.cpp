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

	IndexOffset.SetNum(6);
	IndexOffset[0] = (FIntVector(-1, 0, 0));
	IndexOffset[1] = (FIntVector(1, 0, 0));
	IndexOffset[2] = (FIntVector(0, -1, 0));
	IndexOffset[3] = (FIntVector(0, 1, 0));
	IndexOffset[4] = (FIntVector(0, 0, -1));
	IndexOffset[5] = (FIntVector(0, 0, 1));

	CellOffset = 300;
}

void UWFCSubsystem::AlgorithmSolver()
{
	//temporary start point for easier testing
	int32 StartPoint = GridSize * 0.5f;
	AssignRandomWeightedRoom(StartPoint, StartPoint, StartPoint);

	int Iterator = 0;
	int LoopLimit = 10000;

	while (!IsGridFull())
	{
		FIntVector LowestEntropyIndex = FindLowestEntropy();

		AssignRandomWeightedRoom(LowestEntropyIndex.X, LowestEntropyIndex.Y, LowestEntropyIndex.Z);

		//Infinite loop prevention
		if (Iterator > LoopLimit)
		{
			UE_LOG(LogTemp, Warning, TEXT("WFC Algorithm Infinite Loop Hit %d "), Iterator);
			break;
		}
		Iterator++;
	}
}

void UWFCSubsystem::CollapseCell(int32 x, int32 y, int32 z)
{
	//if (Grid[x][y][z].IsFullyCollapsed)
		//return;

	bool HasBeenCollapsed = false;
	
	for (int i = 0; i < IndexOffset.Num(); i++)
	{
		int32 NeighbourX = IndexOffset[i].X + x;
		int32 NeighbourY = IndexOffset[i].Y + y;
		int32 NeighbourZ = IndexOffset[i].Z + z;

		if (!Grid.IsValidIndex(NeighbourX) || !Grid[NeighbourX].IsValidIndex(NeighbourY) || !Grid[NeighbourX][NeighbourY].IsValidIndex(NeighbourZ))
			continue;

		if (Grid[NeighbourX][NeighbourY][NeighbourZ].AvailableCellKeys.IsEmpty())
			continue;

		//Collecting the requirements of neighbouring rooms
		TArray<TSubclassOf<ARoomBase>> AvailableCells;
		for (TSubclassOf<ARoomBase> NeighbourRoomKey : Grid[NeighbourX][NeighbourY][NeighbourZ].AvailableCellKeys)
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
		for (TSubclassOf<ARoomBase> CellKey : Grid[x][y][z].AvailableCellKeys)
		{
			if (AvailableCells.Contains(CellKey))
				continue;

			HasBeenCollapsed = true;
			KeysToBeRemoved.Add(CellKey);
		}

		//removing rooms that do not accomodate neighbour requirements
		for (TSubclassOf<ARoomBase> CellKey : KeysToBeRemoved)
		{
			Grid[x][y][z].AvailableCellKeys.Remove(CellKey);
		}
		
		if (Grid[x][y][z].AvailableCellKeys.Num() == 1)
			Grid[x][y][z].IsFullyCollapsed = true;
		else if (Grid[x][y][z].AvailableCellKeys.IsEmpty())
		{
			Grid[x][y][z].IsFullyCollapsed = true;
			UE_LOG(LogTemp, Warning, TEXT("WFC Algorithm failed at position:  %d , %d, %d "), x, y, z);
		}
	}
	UpdateEntropy(x, y, z);

	//Collapse neighbours if the available room array shrunk and update its entropy value
	if (HasBeenCollapsed)
	{
		for (int i = 0; i < IndexOffset.Num(); i++)
		{
			int32 NeighbourX = IndexOffset[i].X + x;
			int32 NeighbourY = IndexOffset[i].Y + y;
			int32 NeighbourZ = IndexOffset[i].Z + z;
			if (!Grid.IsValidIndex(NeighbourX) || !Grid[NeighbourX].IsValidIndex(NeighbourY) || !Grid[NeighbourX][NeighbourY].IsValidIndex(NeighbourZ))
				continue;

			CollapseCell(NeighbourX, NeighbourY, NeighbourZ);
		}
	}
}

bool UWFCSubsystem::IsGridFull()
{
	for (int x = 0; x < GridSize; x++)
	{
		for (int y = 0; y < GridSize; y++)
		{
			for (int z = 0; z < GridSize; z++)
			{
				if (!Grid[x][y][z].IsFullyCollapsed)
					return false;
			}
		}
	}
	return true;
}

FIntVector UWFCSubsystem::FindLowestEntropy()
{
	FIntVector LowestEntropyIndex = FIntVector(0, 0, 0);
	int32 LowestEntropy = 999999; //To Do: Remove temp value

	for (int x = 0; x < GridSize; x++)
	{
		for (int y = 0; y < GridSize; y++)
		{
			for (int z = 0; z < GridSize; z++)
			{
				if (Grid[x][y][z].IsFullyCollapsed || Grid[x][y][z].AvailableCellKeys.IsEmpty())
					continue;

				if (Grid[x][y][z].Entropy < LowestEntropy)
				{
					LowestEntropyIndex = FIntVector(x, y, z);
					LowestEntropy = Grid[x][y][z].Entropy;
				}
			}
		}
	}
	return LowestEntropyIndex;
}

void UWFCSubsystem::UpdateEntropy(int32 x, int32 y, int32 z)
{
	int32 TotalWeight = 0;
	for (TSubclassOf<ARoomBase> CellKey : Grid[x][y][z].AvailableCellKeys)
	{
		TotalWeight += AdjacencyRules[CellKey].Weight;
	}
	Grid[x][y][z].Entropy = TotalWeight;
}

void UWFCSubsystem::AssignRandomWeightedRoom(int32 x, int32 y, int32 z)
{
	int32 TotalWeight = 0;
	//To Do: Remove this and replace with entropy
	for (TSubclassOf<ARoomBase> Room : Grid[x][y][z].AvailableCellKeys)
	{
		TotalWeight += AdjacencyRules[Room].Weight;
	}

	int32 Rand = FMath::RandRange(0, TotalWeight);

	for (TSubclassOf<ARoomBase> Room : Grid[x][y][z].AvailableCellKeys)
	{
		if (Rand > AdjacencyRules[Room].Weight)
		{
			Rand -= AdjacencyRules[Room].Weight;
			continue;
		}

		Grid[x][y][z].AvailableCellKeys.Empty();
		Grid[x][y][z].AvailableCellKeys.Add(Room);
		Grid[x][y][z].IsFullyCollapsed = true;
		UpdateEntropy(x, y, z);

		for (int i = 0; i < IndexOffset.Num(); i++)
		{
			int32 NeighbourX = IndexOffset[i].X + x;
			int32 NeighbourY = IndexOffset[i].Y + y;
			int32 NeighbourZ = IndexOffset[i].Z + z;

			if (Grid.IsValidIndex(NeighbourX) && Grid[NeighbourX].IsValidIndex(NeighbourY) && Grid[NeighbourX][NeighbourY].IsValidIndex(NeighbourZ))
			{
				CollapseCell(NeighbourX, NeighbourY, NeighbourZ);
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
			for (int z = 0; z < GridSize; z++)
			{
				if (Grid[x][y][z].AvailableCellKeys.IsEmpty())
					continue;

				FVector Position = FVector(x * CellOffset, y * CellOffset, z * CellOffset);
				FRotator Rotation(0.0f, 0.0f, 0.0f);
				ARoomBase* Room = GetWorld()->SpawnActor<ARoomBase>(Grid[x][y][z].AvailableCellKeys[0], Position, Rotation);
				FString Name = FString::Printf(TEXT("Room: %d, %d, %d"), x, y, z);
				Cast<AActor>(Room)->SetActorLabel(Name);
			}
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
			Grid[x][y].SetNum(GridSize);
			for (int z = 0; z < GridSize; z++)
			{
				for (TPair<TSubclassOf<ARoomBase>, FCellData>& Pair : AdjacencyRules)
				{
					Grid[x][y][z].AvailableCellKeys.Add(Pair.Key);
				}
				UpdateEntropy(x, y, z);
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
			FormatedRoomData.NeighbourCells.SetNum(6);
			FormatedRoomData.NeighbourCells[0].Row.Append(RoomData.Forward);
			FormatedRoomData.NeighbourCells[1].Row.Append(RoomData.Back);
			FormatedRoomData.NeighbourCells[2].Row.Append(RoomData.Right);
			FormatedRoomData.NeighbourCells[3].Row.Append(RoomData.Left);
			FormatedRoomData.NeighbourCells[4].Row.Append(RoomData.Up);
			FormatedRoomData.NeighbourCells[5].Row.Append(RoomData.Down);
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