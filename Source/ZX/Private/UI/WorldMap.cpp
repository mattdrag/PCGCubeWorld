// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/WorldMap.h"

#include "Components/Image.h"
#include "Core/ZXUtils.h"

namespace WorldMapConsts
{
	constexpr float DissolveHidden = 1.f;
	constexpr float DissolveVisible = 0.f;
}

void UWorldMap::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	
	if (auto UIDelegates = UZXUtils::GetUIDelegates(this))
	{
		UIDelegates->OnMapTriggered.AddUObject(this, &ThisClass::HandleMapTriggered);
	}
	
	// Initialize to dissolved and collapsed:
	if (UMaterialInstanceDynamic* MapMaterial = MapImage->GetDynamicMaterial())
	{
		CurrentDissolve = WorldMapConsts::DissolveHidden;
		MapMaterial->SetScalarParameterValue("Dissolve", CurrentDissolve);
	}
	SetVisibility(ESlateVisibility::Collapsed);
}

void UWorldMap::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
	
	// NOTE: Tick will not happen while map is collapsed:
	
	if (UMaterialInstanceDynamic* MapMaterial = MapImage->GetDynamicMaterial())
	{
		// lerp dissolve either towards 1 or zero depending on opening or closing:
		CurrentDissolve = FMath::FInterpConstantTo(CurrentDissolve, 
									bIsMapOpening ? WorldMapConsts::DissolveVisible : WorldMapConsts::DissolveHidden, 
									InDeltaTime,
									DissolveSpeed);
		
		// if map is closing and the dissolve is nearly zero, collapse it (stopping the tick)
		if (!bIsMapOpening && FMath::IsNearlyEqual(CurrentDissolve, WorldMapConsts::DissolveHidden))
		{
			CurrentDissolve = WorldMapConsts::DissolveHidden;
			SetVisibility(ESlateVisibility::Collapsed);
		}
		
		MapMaterial->SetScalarParameterValue("Dissolve", CurrentDissolve);
	}
}

void UWorldMap::HandleMapTriggered()
{
	// Flip flop between visible and collapsed:
	bIsMapOpening = GetVisibility() == ESlateVisibility::Collapsed;
	
	// Here we handle map becoming visible, but tick will handle map collapsing:
	if (bIsMapOpening)
	{
		SetVisibility(ESlateVisibility::Visible);
	}
}
