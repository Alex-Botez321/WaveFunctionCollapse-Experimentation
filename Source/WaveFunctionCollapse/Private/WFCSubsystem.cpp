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
	GridSize = 10;
}

void UWFCSubsystem::AlgorithmSolver()
{
	//temporary start point for easier testing
	int32 StartPoint = GridSize * 0.5f;
	FString StartTile = Grid[StartPoint][StartPoint].AvailableRoomsKeys[0];
	Grid[StartPoint][StartPoint].AvailableRoomsKeys.Empty();
	Grid[StartPoint][StartPoint].AvailableRoomsKeys.Add(StartTile);
	Grid[StartPoint][StartPoint].IsCollapsed = true;
	
	for (int x = 0; x < GridSize; x++)
	{
		for (int y = 0; y < GridSize; y++)
		{

		}
	}

}

void UWFCSubsystem::CollapseNeighboursOfCell(int32 x, int32 y)
{
	TArray<TArray<TSubclassOf<ARoomBase>>> NeighborsInDirection;
	FRoomData CurrentRoom;

	TArray<FVector2D> Direction;
	Direction.SetNum(NeighborsInDirection.Num());
	Direction.Add(FVector2D(1, 0));
	Direction.Add(FVector2D(-1, 0));
	Direction.Add(FVector2D(0, -1));
	Direction.Add(FVector2D(0, 1));
	
	if (AdjacencyRules.Contains(Grid[x][y].AvailableRoomsKeys[0]))
	{
		CurrentRoom = AdjacencyRules[Grid[x][y].AvailableRoomsKeys[0]];
		NeighborsInDirection[0].Append(CurrentRoom.Forward);
		NeighborsInDirection[1].Append(CurrentRoom.Back);
		NeighborsInDirection[2].Append(CurrentRoom.Left);
		NeighborsInDirection[3].Append(CurrentRoom.Right);
	}

	for (int i = 0; i < Direction.Num(); i++)
	{
		Grid[Direction[i].X][Direction[i].Y]
	}
}

bool UWFCSubsystem::IsOutOfBounds(int32 x, int32 y)
{
	return true;
}

bool UWFCSubsystem::IsGridFull()
{
	for (int x = 0; x < GridSize; x++)
	{
		for (int y = 0; y < GridSize; y++)
		{
			if (Grid[x][y].AvailableRoomsKeys.Num() < 1)
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
			for (TPair<FString, FRoomData>& Pair : AdjacencyRules)
			{
				Grid[x][y].AvailableRoomsKeys.Add(Pair.Key);
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

	for (auto& RoomType : JsonObject->Values)
	{
		FString RoomName = RoomType.Key;

		TArray<TSharedPtr<FJsonValue>> RoomArray = RoomType.Value->AsArray();

		FRoomData RoomData;
		if (FJsonObjectConverter::JsonObjectToUStruct(RoomArray[0]->AsObject().ToSharedRef(), &RoomData, 0, 0))
		{
			AdjacencyRules.Add(RoomName, RoomData);
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