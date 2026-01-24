// Fill out your copyright notice in the Description page of Project Settings.


#include "DataCollector.h"
#include "RoomBase.h"
#include "Engine.h"
#include "EngineUtils.h"
#include "DrawDebugHelpers.h"

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
	UWorld* World = GetWorld();

	for (TActorIterator<ARoomBase> ItActor(World); ItActor; ++ItActor)
	{
		UE_LOG(LogTemp, Log, TEXT("[%hs]: found simple actor: %s"), __FUNCTION__, *ItActor->GetName());

		FVector Start = ItActor->GetActorLocation();
		float Distance = 300.0f;

		TArray<FVector> Directions;
		Directions.SetNum(4);

		Directions[0] = ItActor->GetActorForwardVector();
		Directions[1] = -ItActor->GetActorForwardVector();
		Directions[2] = ItActor->GetActorRightVector();
		Directions[3] = -ItActor->GetActorRightVector();

		for (int i = 0; i < 4; i++)
		{
			Directions[i].Normalize();

			FVector End = Start + (Directions[i] * Distance);

			FHitResult Hit;

			FCollisionQueryParams Params;
			Params.AddIgnoredActor(*ItActor);

			bool bHit = GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECC_Visibility, Params);

			if (bHit)
			{
				UE_LOG(LogTemp, Log, TEXT("Hit %s at ^s"), *Hit.GetActor()->GetName(), *Hit.ImpactPoint.ToString());
				DrawDebugLine(GetWorld(), Start, End, FColor::Green, true, 100.0f, 0, 1.5f);
			}
			else
				DrawDebugLine(GetWorld(), Start, End, FColor::Red, true, 100.0f, 0, 1.5f);
			
		}
		
	}
}