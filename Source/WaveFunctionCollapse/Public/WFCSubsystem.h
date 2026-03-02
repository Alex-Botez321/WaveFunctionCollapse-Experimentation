// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "Logging/StructuredLog.h"
#include "RoomBase.h"
#include "DataCollector.h"
#include "WFCSubsystem.generated.h"

class UWFCSubsystem;
class ADataCollector;

USTRUCT(BlueprintType)
struct FNeighboursRow
{
    GENERATED_BODY()

    UPROPERTY()
    TArray<TSubclassOf<ARoomBase>> Row;
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
    TArray<FNeighboursRow> Neighbours;

    //needed for Tarray.find to work
    bool operator==(const FRoomData& Other) const
    {
        return RoomClass == Other.RoomClass;
    }
};

USTRUCT(BlueprintType, Atomic)
struct FTileData
{
    GENERATED_BODY()

    TArray<TSubclassOf<ARoomBase>> AvailableRoomKeys;

    int Entropy;

    bool IsFullyCollapsed = false;
};

UCLASS(Abstract, Blueprintable)
class WAVEFUNCTIONCOLLAPSE_API UWFCSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()
	
public:

    UWFCSubsystem();

    UFUNCTION(BlueprintCallable)
    void AlgorithmSolver();

    UFUNCTION(BlueprintCallable)
    void CollapseCell(int32 x, int32 y);

    UFUNCTION()
    void LoadAdjacencyRules();

    UFUNCTION()
    void PopulateGrid();

    UFUNCTION()
    void SpawnGrid();

    UFUNCTION()
    bool IsGridFull();

    TMap<TSubclassOf<ARoomBase>, FRoomData> AdjacencyRules;

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

