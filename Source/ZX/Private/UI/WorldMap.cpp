// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/WorldMap.h"

#include "Components/Image.h"
#include "Core/ZXUtils.h"
#include "World/GridManagerComponent.h"

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
		UIDelegates->OnMapGenerationComplete.AddUObject(this, &ThisClass::HandleMapGenerationComplete);
	}
	
	// Initialize to dissolved and collapsed:
	if (UMaterialInstanceDynamic* MapMaterial = MapImage->GetDynamicMaterial())
	{
		CurrentDissolve = WorldMapConsts::DissolveHidden;
		MapMaterial->SetScalarParameterValue("Dissolve", CurrentDissolve);
	}
	SetVisibility(ESlateVisibility::Collapsed);
	
	// although we've bound to map gen, theres a chance the map is already generated:
	if (UGridManagerComponent* GridManager = UZXUtils::GetGridManager(this))
	{
		if (GridManager->IsGridGenerated())
		{
			HandleMapGenerationComplete();
		}
	}
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

void UWorldMap::HandleMapGenerationComplete()
{
	// run generation:
	GenerateMapTexture();
	
	// update material:
	if (UMaterialInstanceDynamic* MapMaterial = MapImage->GetDynamicMaterial())
	{
		MapMaterial->SetTextureParameterValue("MapTexture", MapTexture);
	}
}

void UWorldMap::GenerateMapTexture()
{
	TRACE_CPUPROFILER_EVENT_SCOPE(UWorldMap_TextureGen);
	
	UGridManagerComponent* GridManager = UZXUtils::GetGridManager(this);
	if (!IsValid(GridManager))
	{
		return;
	}
	
	MapResolution_X = GridManager->GetNumRows();
	MapResolution_Y = GridManager->GetNumColumns();
	
	// Adapted from: 
	//  - https://dev.epicgames.com/community/learning/tutorials/ow9v/unreal-engine-creating-a-runtime-editable-texture-in-c
	
	// TODO: incorporate FUpdateTextureRegion2D when map becomes larger..
	
	// Create the texture object
	MapTexture = UTexture2D::CreateTransient(MapResolution_X, MapResolution_Y, PF_B8G8R8A8, "DynamicTexture");
	// valid check:
	if (!IsValid(MapTexture) || MapTexture->GetPlatformData() == nullptr || !MapTexture->GetPlatformData()->Mips.IsValidIndex(0))
	{
		LOGZXEF("Could not create map texture..");
		return;
	}
	// Disable compression and mips for clean pixel control
	MapTexture->CompressionSettings = TextureCompressionSettings::TC_VectorDisplacementmap;
	MapTexture->SRGB = 1;
	MapTexture->Filter = TextureFilter::TF_Nearest;
	MapTexture->UpdateResource();
	
	// Get first mip:
	FTexture2DMipMap& Mip = MapTexture->GetPlatformData()->Mips[0];
	uint8* TextureData = static_cast<uint8*>(Mip.BulkData.Lock(LOCK_READ_WRITE));
	if (TextureData == nullptr)
	{
		LOGZXEF("invalid texture data..");
		return;
	}

	// Set pixel color from grid manager:
	for (int32 x = 0; x < MapResolution_X; x++) {
		for (int32 y = 0; y < MapResolution_Y; y++) {
			// Color - we have to translate from linear color (32 bit) to fcolor (8 bit):
			const FColor PixelColor = GridManager->GetColorForMapTile(GridManager->CoordinatesToIndex(x,y));
			
			// NOTE: we are kinda just writing data off the end of a ptr but we just have to trust Epic on this one..
			int32 Index = ((y * MapResolution_X) + x) * 4;
			TextureData[Index]     = PixelColor.B;   // Blue
			TextureData[Index + 1] = PixelColor.G;   // Green
			TextureData[Index + 2] = PixelColor.R; // Red
			TextureData[Index + 3] = PixelColor.A; // Alpha
		}
	}

	// final resource update:
	Mip.BulkData.Unlock();
	MapTexture->UpdateResource();
}