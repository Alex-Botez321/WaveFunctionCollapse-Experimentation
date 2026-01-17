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
