// Fill out your copyright notice in the Description page of Project Settings.


#include "DataCollector.h"
#include "RoomBase.h"
#include "Engine.h"
#include "EngineUtils.h"

// Sets default values
ADataCollector::ADataCollector()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ADataCollector::BeginPlay()
{
	Super::BeginPlay();
	int i = 0;
	UWorld* World = GetWorld();

	for (TActorIterator<ARoomBase> ItActor(World); ItActor; ++ItActor)
	{
		UE_LOG(LogTemp, Log, TEXT("[%hs]: found simple actor: %s"), __FUNCTION__, *ItActor->GetName());
	}
}