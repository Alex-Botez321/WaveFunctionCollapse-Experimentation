#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "WFCAlgorithm.generated.h"

class Vector2D;

UCLASS(Blueprintable, BlueprintType)
class WAVEFUNCTIONCOLLAPSE_API UWFCAlgorithm : public UObject
{
	GENERATED_BODY()
	

private:

    

public:

    UWFCAlgorithm();

    void AlgorithmSolver();

    void CheckNeighbours();

    TArray<TArray<FTileData>> Grid;
    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Grid Config")
    int GridSize;

    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Tiles")
    AActor* BlankTile;
    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Tiles")
    AActor* PlusTile;
    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Tiles")
    AActor* LineTile;
    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Tiles")
    AActor* TTile;
    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Tiles")
    AActor* CornerTile;

    //TArray<Vector2D> NeighbourIndices;
};

USTRUCT(BlueprintType, Atomic)
struct FTileData
{
    GENERATED_USTRUCT_BODY()

    TArray<UWFCTiles> AvailableOptions;

    int Entropy;
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