// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "DataCollector.h"
#include "Logging/StructuredLog.h"
#include "WFCSubsystem.generated.h"

class ARoomBase;
class UWFCSubsystem;
class ADataCollector;

UCLASS(Abstract, Blueprintable)
class WAVEFUNCTIONCOLLAPSE_API UWFCSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()
	
public:

    UWFCSubsystem();

    UFUNCTION(BlueprintCallable)
    void AlgorithmSolver();

    UFUNCTION(BlueprintCallable)
    void CollapseNeighboursOfCell(int x, int y);

    UFUNCTION()
    void LoadAdjancencyRules();

    TMap<FString, FRoomData> AdjacencyRules;

    TArray<TArray<FTileData>> Grid;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grid Config")
    int GridSize;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TArray<TSoftObjectPtr<UWorld>> AllowedWorlds; //TSoftObjectPointer is similar to TSubClassOf in this case

    virtual bool ShouldCreateSubsystem(UObject* Outer) const override;

    virtual void OnWorldBeginPlay(UWorld& InWorld) override;

    virtual void Initialize(FSubsystemCollectionBase& Collection) override;
    virtual void Deinitialize() override;
};

USTRUCT(BlueprintType, Atomic)
struct FTileData
{
    GENERATED_USTRUCT_BODY()

    TArray<ARoomBase>* AvailableRooms;

    int Entropy;

    bool IsCollapsed = false;
};