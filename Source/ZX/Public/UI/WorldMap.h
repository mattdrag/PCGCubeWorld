// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Core/ZX.h"
#include "Blueprint/UserWidget.h"
#include "WorldMap.generated.h"

class UImage;
/**
 * 
 */
UCLASS()
class ZX_API UWorldMap : public UUserWidget
{
	GENERATED_BODY()
	
protected:
	virtual void NativeOnInitialized() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
	
	void HandleMapTriggered();
	
	UPROPERTY(meta = (BindWidget))
	const TObjectPtr<UImage> MapImage;
	
	UPROPERTY(EditDefaultsOnly)
	float DissolveSpeed = 0.5f;
	
private:
	TWeakObjectPtr<UMaterialInstanceDynamic> DynamicMapMaterial;
	
	float CurrentDissolve = 1.f;
	
	bool bIsMapOpening = false;
};
