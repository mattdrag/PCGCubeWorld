// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Core/ZX.h"
#include "ZXPrimaryDataAsset.h"
#include "World/GridTypes.h"
#include "BiomeData.generated.h"

class UPaperSprite;
class UTileSetData;
/**
 * 
 */
UCLASS()
class ZX_API UBiomeData : public UZXPrimaryDataAsset
{
	GENERATED_BODY()

public:
	/**
	 * Data
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = General)
	EBiome BiomeId;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = General)
	FText DisplayName;

	
	// Tile Set:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = TileSet)
	TObjectPtr<UTexture> Dirt;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = TileSet)
	TObjectPtr<UTexture> Sand;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = TileSet)
	TArray<UTexture*> Grass;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = TileSet)
	TArray<UPaperSprite*> Foliage;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = TileSet)
	TArray<UTexture*> Water;

	
	// Styling:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Styling)
	FBiomeTileTypeConfig TileTypeConfig;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Styling)
	TMap<ETileType, FColorRange> TileColorRanges;
	
	
	/**
	 * Meta  - Include with every new Data Asset
	 */
	UBiomeData() { AssetType = UZXAssetManager::BiomeDataType; }
};
