// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Core/ZX.h"
#include "ZXPrimaryDataAsset.h"
#include "TileSetData.generated.h"

/**
 * A tile set is everything that is used as a part of the material that makes up the cube.
 * - Layer0 - base layers that are available, like dirt, sand, water, etc
 * - Layer1 - the tile set, should be a set of 47
 * - Layer2 - static detailing on top to be sampled for procgen, like foliage
 */
UCLASS()
class ZX_API UTileSetData : public UZXPrimaryDataAsset
{
	GENERATED_BODY()

public:
	/**
	 * Data
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = General)
	FGameplayTag GameplayTag;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Data)
	TObjectPtr<UTexture> Layer0;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Data)
	TArray<UTexture*> Layer1;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Data)
	TArray<UTexture*> Layer2;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Data)
	TArray<UTexture*> Water;
	
	/**
	 * Meta - Include with every new Data Asset
	 */
	UTileSetData() { AssetType = UZXAssetManager::TileSetDataType; }
};
