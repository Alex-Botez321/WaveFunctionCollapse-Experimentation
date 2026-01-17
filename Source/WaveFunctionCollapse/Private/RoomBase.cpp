#include "RoomBase.h"

// Sets default values
ARoomBase::ARoomBase()
{
	AvaiableNeighbours.SetNum(NeighbourCount);
	AvaiableNeighbours[2].SetNum(AvaiableNeighboursLeft.Num());
	AvaiableNeighbours[3].SetNum(AvaiableNeighboursRight.Num());
	AvaiableNeighbours[0].SetNum(AvaiableNeighboursForwards.Num());
	AvaiableNeighbours[1].SetNum(AvaiableNeighboursBackwards.Num());

	AvaiableNeighbours[2].Append(AvaiableNeighboursLeft);
	AvaiableNeighbours[3].Append(AvaiableNeighboursRight);
	AvaiableNeighbours[0].Append(AvaiableNeighboursForwards);
	AvaiableNeighbours[1].Append(AvaiableNeighboursBackwards);
}

// Called when the game starts or when spawned
void ARoomBase::BeginPlay()
{
	Super::BeginPlay();
	
}