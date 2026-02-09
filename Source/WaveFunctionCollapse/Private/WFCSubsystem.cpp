#include "WFCSubsystem.h"
#include "RoomBase.h"
#include "Engine/World.h"
#include "Engine/Engine.h"
#include "ObjectTrace.h"

//DEFINE_LOG_CATEGORY(WFCWorldSubSystem);

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

		//Lambda function essentially same as the above
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