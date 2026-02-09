// Fill out your copyright notice in the Description page of Project Settings.


#include "Data/ZXAssetManager.h"

const FPrimaryAssetType	UZXAssetManager::BiomeDataType = TEXT("Biome");
const FPrimaryAssetType	UZXAssetManager::SkillSetType = TEXT("SkillSet");

void UZXAssetManager::StartInitialLoading()
{
	Super::StartInitialLoading();

	// this happens on engine startup (including editor)
	LOGZXW("ZXAssetManager StartInitialLoading...");
}
