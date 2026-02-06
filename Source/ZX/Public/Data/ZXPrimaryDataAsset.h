// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "ZXPrimaryDataAsset.generated.h"

/**
 * Base class for our data assets, so we dont have to define asset type in every class
 */
UCLASS()
class ZX_API UZXPrimaryDataAsset : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = AssetManager)
	FPrimaryAssetType AssetType;

	FORCEINLINE FString GetIdentifierString() const { return GetPrimaryAssetId().ToString(); }
	
	FORCEINLINE virtual FPrimaryAssetId GetPrimaryAssetId() const override { return FPrimaryAssetId(AssetType, GetFName()); }
};
