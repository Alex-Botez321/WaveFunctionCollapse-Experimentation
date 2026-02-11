// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DataCollector.generated.h"

class ARoomBase;

UCLASS()
class WAVEFUNCTIONCOLLAPSE_API ADataCollector : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ADataCollector();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	float Distance;

	const int32 DirectionCount = 4;

	TArray<FRoomData> RoomsData;
public:	
	
};

USTRUCT(BlueprintType, Category = "RoomData")
struct FRoomData
{
	GENERATED_BODY()

	//ensure UPROPERTY is added
	//Unreal ignores fields which lack it
	//work around lack of 2D array
	
	UPROPERTY()
	TSubclassOf<ARoomBase> RoomClass;

	UPROPERTY()
	TArray<TSubclassOf<ARoomBase>> Forward;
	UPROPERTY()
	TArray<TSubclassOf<ARoomBase>> Back;
	UPROPERTY()
	TArray<TSubclassOf<ARoomBase>> Left;
	UPROPERTY()
	TArray<TSubclassOf<ARoomBase>> Right;

	//needed for Tarray.find to work
	bool operator==(const FRoomData& Other) const
	{
		return RoomClass == Other.RoomClass;
	}
};
