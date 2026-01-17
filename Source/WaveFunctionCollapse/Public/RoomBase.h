#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "RoomBase.generated.h"

UCLASS()
class WAVEFUNCTIONCOLLAPSE_API ARoomBase : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ARoomBase();

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Neighbours")
    TArray<TSubclassOf<ARoomBase>> AvaiableNeighboursLeft;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Neighbours")
    TArray<TSubclassOf<ARoomBase>> AvaiableNeighboursRight;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Neighbours")
    TArray<TSubclassOf<ARoomBase>> AvaiableNeighboursForwards;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Neighbours")
    TArray<TSubclassOf<ARoomBase>> AvaiableNeighboursBackwards;

	TArray<TArray<TSubclassOf<ARoomBase>>> AvaiableNeighbours;

    const int32 NeighbourCount = 4;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	

};

enum class GridDirections : uint8
{
	Forwards	UMETA(DisplayName = "Forwards"),
	Backwards	UMETA(DisplayName = "Backwards"),
	Left		UMETA(DisplayName = "Left"),
	Right		UMETA(DisplayName = "Right")
};

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