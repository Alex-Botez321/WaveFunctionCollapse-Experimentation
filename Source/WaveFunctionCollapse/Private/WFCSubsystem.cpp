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

UWFCSubsystem::UWFCSubsystem()
{
	GridSize = 10;

	Grid.SetNum(GridSize);

	for (int x = 0; x < GridSize; x++)
	{
		Grid[x].SetNum(GridSize);
		for (int y = 0; y < GridSize; y++)
		{
			//for (int i = 1; i < (int)UWFCTiles::Length - 1; i++) //x starts at 1 to avoid blank space
			//{
			//	Grid[x][y].AvailableOptions.Add((UWFCTiles)i);
			//}
		}
	}

	//Grid[GridSize * 0.5][GridSize * 0.5].AvailableOptions.Empty(); //TO DO: Replace with random position based on entropy
	//Grid[GridSize * 0.5][GridSize * 0.5].AvailableOptions.Add(UWFCTiles::Plus);
	//Grid[GridSize * 0.5][GridSize * 0.5].IsCollapsed = true;
}

void UWFCSubsystem::AlgorithmSolver()
{
	
}

void UWFCSubsystem::CollapseNeighboursOfCell(int x, int y)
{
	//recursion to check subsequent neighbours if there was any collapse
}

void UWFCSubsystem::LoadAdjancencyRules()
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

void UWFCSubsystem::OnWorldBeginPlay(UWorld& InWorld)
{
	LoadAdjancencyRules();
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