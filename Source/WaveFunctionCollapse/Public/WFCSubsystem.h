// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "Logging/StructuredLog.h"
#include "WFCSubsystem.generated.h"

class ARoomBase;

DECLARE_LOG_CATEGORY_EXTERN(WFCWorldSubSystem, Log, All);

UCLASS(Blueprintable)
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

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tiles")
    ARoomBase* BlankTile;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tiles")
    ARoomBase* PlusTile;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tiles")
    ARoomBase* LineTile;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tiles")
    ARoomBase* TTile;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tiles")
    ARoomBase* CornerTile;

    virtual void Initialize(FSubsystemCollectionBase& Collection) override;
    virtual void Deinitialize() override;
};

USTRUCT(BlueprintType, Atomic)
struct FTileData
{
    GENERATED_USTRUCT_BODY()

    TArray<UWFCTiles> AvailableOptions;

    int Entropy;

    bool IsCollapsed = false;
};

UENUM(BlueprintType)
enum class UWFCTiles : uint8
{
    Blank           UMETA(DisplayName = "Blank"),
    Plus            UMETA(DisplayName = "Plus"),
    LineHorizontal  UMETA(DisplayName = "LineHorizontal"),
    LineVertical    UMETA(DisplayName = "LineVertical"),
    T0              UMETA(DisplayName = "T0"),
    T90             UMETA(DisplayName = "T90"),
    T180            UMETA(DisplayName = "T180"),
    T270            UMETA(DisplayName = "T270"),
    Corner0         UMETA(DisplayName = "Corner0"),
    Corner90        UMETA(DisplayName = "Corner90"),
    Corner180       UMETA(DisplayName = "Corner180"),
    Corner270       UMETA(DisplayName = "Corner270"),
    Length          UMETA(DisplayName = "Length"),
};

UENUM(BlueprintType)
enum class UWFCState : uint8
{
    None            UMETA(DisplayName = "None"),
    Initializing    UMETA(DisplayName = "Initializing"),
    Collapsing      UMETA(DisplayName = "Collapsing"),
    Finished        UMETA(DisplayName = "Finished"),
    Failed          UMETA(DisplayName = "Failed")
};