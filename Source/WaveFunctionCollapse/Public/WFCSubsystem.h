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
struct FCellData
{
    GENERATED_BODY()

    //ensure UPROPERTY is added
    //Unreal ignores fields which lack it
    //work around lack of 2D array

    UPROPERTY()
    TSubclassOf<ARoomBase> CellClass;

    UPROPERTY()
    int32 Weight;

    UPROPERTY()
    TArray<FNeighboursRow> NeighbourCells;

    //needed for Tarray.find to work
    bool operator==(const FCellData& Other) const
    {
        return CellClass == Other.CellClass;
    }
};

USTRUCT(BlueprintType, Atomic)
struct FGridCellData
{
    GENERATED_BODY()

    TArray<TSubclassOf<ARoomBase>> AvailableCellKeys;

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

    /// <summary>
    /// Checks which cells can generate based on its neighbouring cells
    /// </summary>
    /// <param name="x">Grid index</param>
    /// <param name="y">Grid index</param>
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

    UFUNCTION()
    FIntPoint FindLowestEntropy();

    UFUNCTION()
    void UpdateEntropy(int32 x, int32 y);

    UPROPERTY()
    TMap<TSubclassOf<ARoomBase>, FCellData> AdjacencyRules;

    TArray<TArray<FGridCellData>> Grid;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grid Config")
    int32 GridSize;

    UPROPERTY()
    TArray<FIntPoint> IndexOffset;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TArray<TSoftObjectPtr<UWorld>> AllowedWorlds; //TSoftObjectPointer is similar to TSubClassOf in this case

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Generation")
    int32 CellOffset;

    virtual bool ShouldCreateSubsystem(UObject* Outer) const override;

    virtual void OnWorldBeginPlay(UWorld& InWorld) override;

    virtual void Initialize(FSubsystemCollectionBase& Collection) override;
    virtual void Deinitialize() override;
};

