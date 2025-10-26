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

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	

};
