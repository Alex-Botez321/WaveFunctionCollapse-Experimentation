// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "LevelDataAssetBase.generated.h"

/**
 * 
 */
UCLASS()
class WAVEFUNCTIONCOLLAPSE_API ULevelDataAssetBase : public UPrimaryDataAsset
{
	GENERATED_BODY()
	//each room base needs to be split into its rotated variants
	//then take each room split it into each direction it can attach to
	//then take each direction and have a list of possible combinations

	//main question to think about. 
	// Is it better to:
	// Have each room rotation be separate in engine and store adjacency rules in ARoomBase
	// or
	// Have store adjecency rules here then rotate the rooms accordingly in spawn
};

//    Blank           UMETA(DisplayName = "Blank"),
//    Plus            UMETA(DisplayName = "Plus"),
//    LineHorizontal  UMETA(DisplayName = "LineHorizontal"),
//    LineVertical    UMETA(DisplayName = "LineVertical"),
//    T0              UMETA(DisplayName = "T0"),
//    T90             UMETA(DisplayName = "T90"),
//    T180            UMETA(DisplayName = "T180"),
//    T270            UMETA(DisplayName = "T270"),
//    Corner0         UMETA(DisplayName = "Corner0"),
//    Corner90        UMETA(DisplayName = "Corner90"),
//    Corner180       UMETA(DisplayName = "Corner180"),
//    Corner270       UMETA(DisplayName = "Corner270"),