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