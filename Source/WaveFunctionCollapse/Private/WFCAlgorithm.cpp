#include "WFCAlgorithm.h"

UWFCAlgorithm::UWFCAlgorithm()
{
	//GridSize = 10;

	//Grid.SetNum(GridSize);

	//for (int x = 0; x < GridSize; x++)
	//{
	//	Grid[x].SetNum(GridSize);
	//	for (int y = 0; y < GridSize; y++)
	//	{
	//		for (int i = 1; i < (int)UWFCTiles::Length - 1; i++) //x starts at 1 to avoid blank space
	//		{
	//			Grid[x][y].AvailableOptions.Add((UWFCTiles)i); 
	//		}
	//	}
	//}

	//Grid[GridSize * 0.5][GridSize * 0.5].AvailableOptions.Empty(); //TO DO: Replace with random position based on entropy
	//Grid[GridSize * 0.5][GridSize * 0.5].AvailableOptions.Add(UWFCTiles::Plus);
	//Grid[GridSize * 0.5][GridSize * 0.5].IsCollapsed = true;
}


//void UWFCAlgorithm::AlgorithmSolver()
//{
//	for (int i = 0; i < 100; i++) //TO DO: Replace with entropy calculation or breaking down grid to prevent failure
//	{
//		for (int x = 0; x < GridSize; x++)
//		{
//			for (int y = 0; y < GridSize; y++)
//			{
//				if (Grid[x][y].AvailableOptions.Num() == 1)
//				{
//					Grid[x][y].IsCollapsed = true;
//					continue;
//				}
//				else if (Grid[x][y].AvailableOptions.Num() == 0)
//				{
//					Grid[x][y].AvailableOptions.Add(UWFCTiles::Blank);
//					continue;
//				}
//			}
//		}
//	}
//}

//void UWFCAlgorithm::CollapseNeighboursOfCell(int x, int y)
//{
//	//recursion to check subsequent neighbours if there was any collapse
//}