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
	TSubclassOf<ARoomBase> StartTile = Grid[StartPoint][StartPoint].AvailableCellKeys[0];
	Grid[StartPoint][StartPoint].AvailableCellKeys.Empty();
	Grid[StartPoint][StartPoint].AvailableCellKeys.Add(StartTile);
	Grid[StartPoint][StartPoint].IsFullyCollapsed = true;
	UpdateEntropy(StartPoint, StartPoint);

	for (int i = 0; i < IndexOffset.Num(); i++)
	{
		int32 NeighbourX = IndexOffset[i].X + StartPoint;
		int32 NeighbourY = IndexOffset[i].Y + StartPoint;

		if (Grid.IsValidIndex(NeighbourX) && Grid[NeighbourX].IsValidIndex(NeighbourY))
		{
			CollapseCell(NeighbourX, NeighbourY);
		}
	}

	/*
	///////////////---------------/////////////
	 To Do:
	FIGURE OUT HOW TO BEST COLLAPSE A CELL AFTER IT IS CHOSEN RANDOMLY
	///////////////---------------/////////////
	*/



	int Iterator = 0;
	int LoopLimit = 10000;

	while (!IsGridFull())
	{
		FIntPoint LowestEntropyIndex = FindLowestEntropy();
		//potentially remove extra collapse
		//if (Grid[LowestEntropyIndex.X][LowestEntropyIndex.Y].AvailableCellKeys.IsEmpty())
		//{
			UE_LOG(LogTemp, Error, TEXT("WFC Algorithm failed at position:  %d , %d.  "), LowestEntropyIndex.X, LowestEntropyIndex.Y);
			UE_LOG(LogTemp, Error, TEXT("WFC Algorithm failed at position:  %d "), Grid[LowestEntropyIndex.X][LowestEntropyIndex.Y].AvailableCellKeys.Num())
		//}
		CollapseCell(LowestEntropyIndex.X, LowestEntropyIndex.Y);
		if (Grid[LowestEntropyIndex.X][LowestEntropyIndex.Y].AvailableCellKeys.IsEmpty())
		{
			UE_LOG(LogTemp, Error, TEXT("WFC Algorithm failed at position:  %d , %d.  "), LowestEntropyIndex.X, LowestEntropyIndex.Y);
			UE_LOG(LogTemp, Error, TEXT("WFC Algorithm failed at position:  %d "), Grid[LowestEntropyIndex.X][LowestEntropyIndex.Y].AvailableCellKeys.Num());
			UE_LOG(LogTemp, Error, TEXT("WFC Algorithm failed at position:  %d "), Grid[LowestEntropyIndex.X + IndexOffset[0].X][LowestEntropyIndex.Y + IndexOffset[0].Y].AvailableCellKeys.Num());
			UE_LOG(LogTemp, Error, TEXT("WFC Algorithm failed at position:  %d "), Grid[LowestEntropyIndex.X + IndexOffset[1].X][LowestEntropyIndex.Y + IndexOffset[1].Y].AvailableCellKeys.Num());
			UE_LOG(LogTemp, Error, TEXT("WFC Algorithm failed at position:  %d "), Grid[LowestEntropyIndex.X + IndexOffset[2].X][LowestEntropyIndex.Y + IndexOffset[2].Y].AvailableCellKeys.Num());
			UE_LOG(LogTemp, Error, TEXT("WFC Algorithm failed at position:  %d "), Grid[LowestEntropyIndex.X + IndexOffset[3].X][LowestEntropyIndex.Y + IndexOffset[3].Y].AvailableCellKeys.Num());
			FVector Location = FVector(LowestEntropyIndex.X * CellOffset, LowestEntropyIndex.Y * CellOffset, 500);
			FRotator Rotation(0.0f, 0.0f, 0.0f);
			ARoomBase* Room = GetWorld()->SpawnActor<ARoomBase>(Grid[StartPoint][StartPoint].AvailableCellKeys[0], Location, Rotation);
			break;
		}

		FGridCellData NewCellData = Grid[LowestEntropyIndex.X][LowestEntropyIndex.Y];

		int32 RandomCell = FMath::RandRange(0, NewCellData.AvailableCellKeys.Num() - 1);
		TSubclassOf<ARoomBase> ChosenKey = NewCellData.AvailableCellKeys[RandomCell];
		NewCellData.AvailableCellKeys.Empty();
		NewCellData.AvailableCellKeys.Add(ChosenKey);
		NewCellData.IsFullyCollapsed = true;
		Grid[LowestEntropyIndex.X][LowestEntropyIndex.Y] = NewCellData;
		UpdateEntropy(LowestEntropyIndex.X, LowestEntropyIndex.Y);

		for (int i = 0; i < IndexOffset.Num(); i++)
		{
			int32 NeighbourX = IndexOffset[i].X + LowestEntropyIndex.X;
			int32 NeighbourY = IndexOffset[i].Y + LowestEntropyIndex.Y;

			if (Grid.IsValidIndex(NeighbourX) && Grid[NeighbourX].IsValidIndex(NeighbourY))
			{
				CollapseCell(NeighbourX, NeighbourY);
				UpdateEntropy(NeighbourX, NeighbourY);
			}
		}

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

		//Collapse neighbours if the available room array shrunk and update its entropy value
		if (HasBeenCollapsed)
		{
			CollapseCell(NeighbourX, NeighbourY);
		}
	}
	UpdateEntropy(x, y);
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
	/*if (Grid[x][y].IsFullyCollapsed)
	{
		Grid[x][y].Entropy = 0;
		return;
	}*/

	int32 TotalWeight = 0;
	for (TSubclassOf<ARoomBase> CellKey : Grid[x][y].AvailableCellKeys)
	{
		TotalWeight += AdjacencyRules[CellKey].Weight;
	}
	Grid[x][y].Entropy = TotalWeight;
}

void UWFCSubsystem::SpawnGrid()
{
	for (int x = 0; x < GridSize; x++)
	{
		for (int y = 0; y < GridSize; y++)
		{
			if (Grid[x][y].AvailableCellKeys.IsEmpty())
				continue;

			FVector Location = FVector(x * CellOffset, y * CellOffset, 0);
			FRotator Rotation(0.0f, 0.0f, 0.0f);
			ARoomBase* Room = GetWorld()->SpawnActor<ARoomBase>(Grid[x][y].AvailableCellKeys[0], Location, Rotation);
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