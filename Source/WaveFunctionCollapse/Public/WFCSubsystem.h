// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "Logging/StructuredLog.h"
#include "WFCSubsystem.generated.h"

class ARoomBase;

//DECLARE_LOG_CATEGORY_EXTERN(WFCWorldSubSystem, Log, All);

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

    TArray<TArray<FTileData>> Grid;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grid Config")
    int GridSize;

    virtual void Initialize(FSubsystemCollectionBase& Collection) override;
    virtual void Deinitialize() override;
};

USTRUCT(BlueprintType, Atomic)
struct FTileData
{
    GENERATED_USTRUCT_BODY()

    TArray<ARoomBase>* AvailableOptions;

    int Entropy;

    bool IsCollapsed = false;
};

/*data asset which
* Holds all tiles
* holds a rotation quat
* holds tiles that can be adjacent
*/
