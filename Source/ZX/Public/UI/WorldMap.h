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
	virtual void NativeDestruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
	
	// player presses 'M' or hits the zoom threshold:
	void HandleMapTriggered();
	
	// called when grid manager has finished map gen:
	void HandleMapGenerationComplete();
	void GenerateMapTexture();
	
	// recalc dimensions whenever screen resolution changes:
	void UpdateMapDimensions();
	void OnViewportResized(FViewport* Viewport, uint32 UnusedInt);
	
	// We hold ptr for texture, uproperty will handle gc:
	UPROPERTY()
	TObjectPtr<UTexture2D> MapTexture;
	
	UPROPERTY(meta = (BindWidget))
	const TObjectPtr<UImage> MapImage;
	
	UPROPERTY(EditDefaultsOnly)
	float InitialZoom = 10.f;
	
	UPROPERTY(EditDefaultsOnly)
	float DissolveSpeed = 0.5f;
	
private:
	TWeakObjectPtr<UMaterialInstanceDynamic> DynamicMapMaterial;
	
	float CurrentDissolve = 1.f;
	
	bool bIsMapOpening = false;
	
	int32 MapResolution_X = 0;
	int32 MapResolution_Y = 0;
	
	float ZoomAmount = 10.f;
	float MapImageScale = 1.f;
	float UVScale = 1.f;
};
