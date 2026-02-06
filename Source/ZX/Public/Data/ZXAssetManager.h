// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Core/ZX.h"
#include "Engine/AssetManager.h"
#include "ZXAssetManager.generated.h"

/**
 * 
 */
UCLASS()
class ZX_API UZXAssetManager : public UAssetManager
{
	GENERATED_BODY()

public:
	// Constructor and overrides
	UZXAssetManager() {}
	virtual void StartInitialLoading() override;

	/** Static types */
	static const FPrimaryAssetType BiomeDataType;
	static const FPrimaryAssetType TileSetDataType;
	static const FPrimaryAssetType SkillSetType;
};
