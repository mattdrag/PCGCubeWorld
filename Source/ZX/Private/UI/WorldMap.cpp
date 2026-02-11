// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/WorldMap.h"

#include "Blueprint/WidgetLayoutLibrary.h"
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
	
	// listen for viewport size changes:
	if (UWorld* World = GetWorld())
	{
		if (UGameViewportClient* ViewportClient = World->GetGameViewport())
		{
			if (ViewportClient->Viewport != nullptr)
			{
				ViewportClient->Viewport->ViewportResizedEvent.AddUObject(this, &ThisClass::OnViewportResized);	
			}
		}
	}
	
	// bind a couple of our own delegate:
	if (auto UIDelegates = UZXUtils::GetUIDelegates(this))
	{
		UIDelegates->OnMapGenerationComplete.AddUObject(this, &ThisClass::HandleMapGenerationComplete);
		UIDelegates->OnMapZoom.AddUObject(this, &ThisClass::HandleMapZoom);
		
		// Map listens for open, broadcasts close:
		UIDelegates->OnMapOpened.AddUObject(this, &ThisClass::HandleMapOpened);
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

void UWorldMap::NativeDestruct()
{
	// cleanup delegates:
	if (UWorld* World = GetWorld())
	{
		if (UGameViewportClient* ViewportClient = World->GetGameViewport())
		{
			if (ViewportClient->Viewport != nullptr)
			{
				ViewportClient->Viewport->ViewportResizedEvent.RemoveAll(this);	
			}
		}
	}
	
	if (auto UIDelegates = UZXUtils::GetUIDelegates(this))
	{
		UIDelegates->OnMapOpened.RemoveAll(this);
		UIDelegates->OnMapGenerationComplete.RemoveAll(this);
		UIDelegates->OnMapZoom.RemoveAll(this);
	}
	
	Super::NativeDestruct();
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

void UWorldMap::HandleMapOpened()
{
	// tick uses this bool to handle fading in:
	bIsMapOpening = true;
	SetVisibility(ESlateVisibility::Visible);
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
	
	// set map scale and UV scale:
	BaseZoom = InitialZoom - MapImageScale;
	UpdateMapDimensions();
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
	MapTexture->AddressX = TextureAddress::TA_Clamp;
	MapTexture->AddressY = TextureAddress::TA_Clamp;
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
	for (int32 x = 0; x < MapResolution_X; x++) 
	{
		for (int32 y = 0; y < MapResolution_Y; y++) 
		{
			// TODO: remove this later:
			// Trim map texture in black so that the image doesnt stretch when zooming out:
			const bool bTrimBlack = x == 0 || x == MapResolution_X - 1 || y == 0 || y == MapResolution_Y - 1;
			
			// Color - we have to translate from linear color (32 bit) to fcolor (8 bit):
			const FColor PixelColor = bTrimBlack ? FColor::Black : GridManager->GetColorForMapTile(GridManager->CoordinatesToIndex(x,y));
			
			// NOTE: we are kinda just writing data off the end of a ptr but we just have to trust Epic on this one..
			const int32 Index = ((y * MapResolution_X) + x) * 4;
			TextureData[Index]     = PixelColor.B;  // Blue
			TextureData[Index + 1] = PixelColor.G;  // Green
			TextureData[Index + 2] = PixelColor.R;	// Red
			TextureData[Index + 3] = PixelColor.A;	// Alpha
		}
	}

	// final resource update:
	Mip.BulkData.Unlock();
	MapTexture->UpdateResource();
}

void UWorldMap::OnViewportResized(FViewport* Viewport, uint32 UnusedInt)
{
	// just wrap UpdateMapDimensions for now..
	UpdateMapDimensions();
}

void UWorldMap::HandleMapZoom(float ZoomInput)
{
	// clamp additional zoom based on target UVs:
	const float MaxAdditionalZoom = BaseZoom + MapImageScale - MinUVScale;
	AdditionalZoom = FMath::Clamp(AdditionalZoom + (ZoomInput * ZoomScaleModifier * UVScale), 0.f, MaxAdditionalZoom);
	
	// zero additional zoom and a negative zoom input should trigger a map closing:
	if (AdditionalZoom == 0.f && ZoomInput < 0.f)
	{
		// tell PC to give zoom input back to camera:
		if (auto UIDelegates = UZXUtils::GetUIDelegates(this))
		{
			UIDelegates->OnMapClosed.Broadcast();
		}
		// for dissolve out:
		bIsMapOpening = false;
		return;
	}
	
	// the rest goes into the UVs:
	UVScale = BaseZoom + MapImageScale - AdditionalZoom;
	if (UMaterialInstanceDynamic* MapMaterial = MapImage->GetDynamicMaterial())
	{
		MapMaterial->SetScalarParameterValue("ScaleUV", UVScale);
	}
	
	LOGZXSCREEN("AdditionalZoom: %f", AdditionalZoom);
}

void UWorldMap::UpdateMapDimensions()
{
	// using the major axis of the viewport, calc how much we need to scale the map image:
	const FVector2D ViewportSize = UWidgetLayoutLibrary::GetViewportSize(this);
	if (ViewportSize.X == 0.f || ViewportSize.Y == 0.f)
	{
		LOGZXEF("invalid viewport size..");
		return;
	}
	
	// scale map image:
	MapImageScale = ViewportSize.X > ViewportSize.Y ? ViewportSize.X / ViewportSize.Y : ViewportSize.Y / ViewportSize.X;
	
	// when MapImage zoom adjusts, so does BaseZoom:
	MapImage->SetRenderScale(FVector2D(MapImageScale, MapImageScale));
	BaseZoom = InitialZoom - MapImageScale;
	
	// call a zoom with no input. this will adjust the UVs:
	HandleMapZoom(0.f);
}