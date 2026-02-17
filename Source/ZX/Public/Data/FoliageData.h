// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ZXAssetManager.h"
#include "ZXPrimaryDataAsset.h"
#include "FoliageData.generated.h"

class UPaperSprite;
/**
 * 
 */
UCLASS()
class ZX_API UFoliageData : public UZXPrimaryDataAsset
{
	GENERATED_BODY()

public:
	/**
	 * Data
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = General)
	FText DisplayName;

	// TODO: in bloom + harvested sprite differentiation
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = TileSet)
	UPaperSprite* Sprite;
	
	// TODO: interactions
	
	// TODO: hover data
	
	// TODO: loot tables
	
	/**
	 * Meta  - Include with every new Data Asset
	 */
	UFoliageData() { AssetType = UZXAssetManager::FoliageDataType; }
};
