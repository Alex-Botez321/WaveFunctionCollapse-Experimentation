#include "WFCAlgorithm.h"

UWFCAlgorithm::UWFCAlgorithm()
{
	GridSize = 10;

	Grid.SetNum(GridSize);

	for (int i = 0; i < GridSize; i++)
	{
		Grid[i].SetNum(GridSize);
		for (int j = 0; j < GridSize; j++)
		{
			for (int x = 0; x < UWFCTiles::Length - 1; x++)
			{
				Grid[i][j].AvailableOptions.Add(UWFCTiles); //Loop through enum somehow to add all options
			}
			
		}
	}

	Grid[GridSize * 0.5][GridSize * 0.5].AvailableOptions.Empty(); //TO DO: Replace with random position based on entropy
	Grid[GridSize * 0.5][GridSize * 0.5].AvailableOptions.Add(UWFCTiles::Plus);
}

void UWFCAlgorithm::AlgorithmSolver()
{
	for (int i = 0; i < 100; i++) //TO DO: Replace with entropy calculation or breaking down grid to prevent failure
	{
		for (int x = 0; x < GridSize; x++)
		{
			for (int y = 0; y < GridSize; y++)
			{
				//if (Grid[x][y] == UWFCTiles::Blank)
					//continue;


			}
		}
	}
}

void UWFCAlgorithm::CheckNeighbours()
{

}