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
	
	// player hits the zoom threshold:
	void HandleMapOpened();
	
	// player zooms past camera max:
	void HandleMapZoom(float ZoomInput);
	
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
	float DissolveSpeed = 0.5f;
	
	UPROPERTY(EditDefaultsOnly, Category=Zoom)
	float InitialZoom = 10.f;
	
	UPROPERTY(EditDefaultsOnly, Category=Zoom)
	float MinUVScale = 0.1f;
	
	UPROPERTY(EditDefaultsOnly, Category=Zoom)
	float ZoomScaleModifier = 0.002f;
	
	UPROPERTY(EditDefaultsOnly, Category=Zoom)
	float UVZoomMin = 0.05f;
	
	UPROPERTY(EditDefaultsOnly, Category=Zoom)
	float UVZoomMax = 100.f;
	
private:
	TWeakObjectPtr<UMaterialInstanceDynamic> DynamicMapMaterial;
	
	float CurrentDissolve = 1.f;
	
	bool bIsMapOpening = false;
	
	int32 MapResolution_X = 0;
	int32 MapResolution_Y = 0;

	float AdditionalZoom = 0.f;
	float MapImageScale = 1.f;
	float UVScale = 1.f;
	
	float BaseZoom = 10.f;
};
