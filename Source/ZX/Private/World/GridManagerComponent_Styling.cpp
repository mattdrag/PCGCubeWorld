#include "Core/ZXUtils.h"
#include "Data/BiomeData.h"
#include "Data/TileSetData.h"
#include "Data/ZXAssetManager.h"
#include "World/GridTypes.h"
#include "World/GridManagerComponent.h"
#include "World/ZXCube.h"

namespace GridManagerStylingConsts
 {
 	constexpr int32 NumFoliageSlots = 8;
	const FName TestGrasslandsName = "Data.Biome.TestGrasslands";
 }

void UGridManagerComponent::LoadBiomes()
{
	UZXAssetManager* AssetManager = UZXUtils::GetZXAssetManager();
	if (AssetManager == nullptr)
	{
		return;
	}
	
	TArray<FPrimaryAssetId> TileStyleIdList;
	AssetManager->GetPrimaryAssetIdList(UZXAssetManager::BiomeDataType, TileStyleIdList);
	for (const FPrimaryAssetId& Id : TileStyleIdList)
	{
		// Get the uobject:
		FAssetData AssetDataToParse;
		AssetManager->GetPrimaryAssetData(Id, AssetDataToParse);

		// Add to map:
		UBiomeData* LocalBiomeData = Cast<UBiomeData>(AssetDataToParse.GetAsset());
		if (LocalBiomeData != nullptr && LocalBiomeData->GameplayTag.IsValid())
		{
			BiomeData.Add(LocalBiomeData->GameplayTag, LocalBiomeData);
		}
	}
}

ETileType UGridManagerComponent::DetermineTileType(float InMoisture)
{
	// TODO: right now we have 1 biome, eventually we want to pass it in as a parameter or something..
	TObjectPtr<UBiomeData>* CurrentBiome = BiomeData.Find(FGameplayTag::RequestGameplayTag(GridManagerStylingConsts::TestGrasslandsName));
	if (CurrentBiome == nullptr || !IsValid(*CurrentBiome))
	{
		LOGZXEF("CurrentBiome configured incorrectly");
		return ETileType::Grass;
	}
	
	// TODO: biome tile type configs are simple at the moment:
	const float SandThresh = (*CurrentBiome)->TileTypeConfig.SandThreshold;
	const float WaterThresh = (*CurrentBiome)->TileTypeConfig.WaterThreshold;
	if (InMoisture > WaterThresh)
	{
		return ETileType::Water;
	}
	if (InMoisture > SandThresh)
	{
		return ETileType::Dirt; // TODO: make a sand texture.. for now use dirt.
	}
	return (*CurrentBiome)->TileTypeConfig.BaseType;
}

void UGridManagerComponent::StyleCube(AZXCube* InCube)
{
	TRACE_CPUPROFILER_EVENT_SCOPE(UGridManagerComponent_StyleCube);
	
	if (!IsValid(InCube))
	{
		LOGZXEF("null cube..");
		return;
	}
	// get tile:
	FGridTile* InTile = GetGridTile(InCube->MyGridTileIdx);
	if (InTile == nullptr)
	{
		LOGZXEF("null tile..");
		return;
	}

	const FIntPoint InCoordinate = IndexToCoordinates(InTile->MyIndex);

	// MD-TODO: current biome:
	TObjectPtr<UBiomeData>* CurrentBiome = BiomeData.Find(FGameplayTag::RequestGameplayTag(GridManagerStylingConsts::TestGrasslandsName));
	if (CurrentBiome == nullptr || !IsValid(*CurrentBiome) || !IsValid((*CurrentBiome)->TileSet))
	{
		LOGZXEF("CurrentBiome configured incorrectly");
		return;
	}
	UTileSetData* TileSet = (*CurrentBiome)->TileSet;
	
	// make sure it has a material we can style:
	UMaterialInstanceDynamic* DynCubeMat = InCube->GetDynamicMaterial(InTile->Type);
	if (!IsValid(DynCubeMat))
	{
		LOGZXEF("null cube material..");
		return;
	}
	
	auto CalcShading = [](const FColorRange& ColorRange, float DarkestVal, float LightestVal, float Moisture)
	{
		// grass will go from -1 to sand thresh:
		const float MidPoint = (DarkestVal + LightestVal) / 2;
			
		// Transform to linear colors in order to take diff:
		const bool bDarken = Moisture < MidPoint;
		const FLinearColor TargetColor = bDarken ? ColorRange.Darkest : ColorRange.Lightest;
		const FLinearColor BaseColor = ColorRange.Base;
		const float Alpha = (Moisture - MidPoint) / ((bDarken ? DarkestVal : LightestVal) - MidPoint) ;
		const FLinearColor LerpedColor = FMath::Lerp(BaseColor, TargetColor, Alpha);

		// shading is additive, so we actually want the diff:
		return LerpedColor - BaseColor;
	};
	
	// Base is either dirt or sand. if there are any adjacent sand tiles, it becomes sand.
	//const bool bIsSand = HasAnyNeighborsOfType(ETileType::Sand, InCoordinate);
	const bool bIsSand = InTile->Moisture > (*CurrentBiome)->TileTypeConfig.SandThreshold;
	DynCubeMat->SetTextureParameterValue("TopTexture", bIsSand ? TileSet->Sand : TileSet->Dirt);
	DynCubeMat->SetTextureParameterValue("SideTexture", bIsSand ? TileSet->Sand : TileSet->Dirt);

	// Grass:
	if (InTile->Type == ETileType::Grass)
	{
		const uint8 TileSheetIndex = Autotile(ETileType::Grass, InCoordinate);
	
		// Top:
		if (TileSet->Layer1.IsValidIndex(TileSheetIndex))
		{
			DynCubeMat->SetTextureParameterValue("GrassTexture", TileSet->Layer1[TileSheetIndex]);
		}
		
		// Foliage
		//  - NOTE: since we are styling on 1 dim, lowest moisture will have most foliage:
		const float FoliageLB = -1.f; // TODO: moisture curve
		const float FoliageUB = -0.3f; // TODO: moisture curve
		if (InTile->Moisture < FoliageUB)
		{
			TArray<FVector2D> FoliageUVs;
			GetJitteredGridForTile(InTile, FoliageUVs, FoliageLB, FoliageUB);
	
			// we only have so many foliage slots:
			const int32 NumFoliage = FMath::Clamp(FoliageUVs.Num(), 0, GridManagerStylingConsts::NumFoliageSlots);
	
			// plant foliage for each of the slots:
			for (int32 i = 0; i < NumFoliage; i++)
			{
				const FVector2D& FoliageUV = FoliageUVs[i];
		
				// TODO: get random weighted index
				const int32 FoliageIdx = FMath::RandRange(0, TileSet->Layer2.Num() - 1);
		
				// Set material params:
				const FString FoliageStr = FString::Printf(TEXT("Foliage%d"), i);
				const FString FoliageUStr = FString::Printf(TEXT("Foliage%dU"), i);
				const FString FoliageVStr = FString::Printf(TEXT("Foliage%dV"), i);
				DynCubeMat->SetTextureParameterValue(FName(*FoliageStr), TileSet->Layer2[FoliageIdx]);
				DynCubeMat->SetScalarParameterValue(FName(*FoliageUStr), FoliageUV.X);
				DynCubeMat->SetScalarParameterValue(FName(*FoliageVStr), FoliageUV.Y);
			}
		}
		
		// Grass shading:
		if (FColorRange* ColorRange = (*CurrentBiome)->TileColorRanges.Find(InTile->Type))
		{
			DynCubeMat->SetVectorParameterValue("Shading", CalcShading(*ColorRange, -1, (*CurrentBiome)->TileTypeConfig.SandThreshold, InTile->Moisture));
		}
	}
	
	// Water:
	if (InTile->Type == ETileType::Water)
	{
		const uint8 TileSheetIndex = Autotile(ETileType::Water, InCoordinate);
		
		if (TileSet->Water.IsValidIndex(TileSheetIndex))
		{
			DynCubeMat->SetTextureParameterValue("WaterTexture", TileSet->Water[TileSheetIndex]);
		}
		
		// Water gets shaded:
		if (FColorRange* ColorRange = (*CurrentBiome)->TileColorRanges.Find(InTile->Type))
		{
			auto WaterShading = CalcShading(*ColorRange, 1, (*CurrentBiome)->TileTypeConfig.WaterThreshold, InTile->Moisture);
			DynCubeMat->SetVectorParameterValue("Shading", WaterShading);
			LOGZXWF("WaterShading=(%f,%f,%f)", WaterShading.R, WaterShading.G, WaterShading.B);
		}
	}
}

uint8 UGridManagerComponent::Autotile(ETileType InType, const FIntPoint& InCoord)
{
	// Get bitmask:
	uint8 NeighborBitmask = 0;
	uint8 i = 0;
	for (const FIntPoint& Neighbor : GridTypesConsts::NeighborArray)
	{
		const FIntPoint NeighborCoordinate = InCoord + Neighbor;
		const FGridTile* NeighborTile = GetGridTile(NeighborCoordinate);
		if (NeighborTile != nullptr && NeighborTile->Type == InType)
		{
			NeighborBitmask |= (1 << (7 - i));
		}
		i++;
	}
	// Convert to tile sheet:
	if (!BitmaskToTileStyle.IsValidIndex(NeighborBitmask))
	{
		UE_LOG(LogZX, Error, TEXT("%s: BitmaskToTileStyle incomplete.."), *FString(__FUNCTION__));
		return 0;
	}
	return BitmaskToTileStyle[NeighborBitmask];
}

bool UGridManagerComponent::HasAnyNeighborsOfType(ETileType InType, const FIntPoint& InCoord)
{
	for (const FIntPoint& Neighbor : GridTypesConsts::NeighborArray)
	{
		const FIntPoint NeighborCoordinate = InCoord + Neighbor;
		const FGridTile* NeighborTile = GetGridTile(NeighborCoordinate);
		if (NeighborTile != nullptr && NeighborTile->Type == InType)
		{
			return true;
		}
	}
	return false;
}

int32 UGridManagerComponent::GetJitteredGridForTile(FGridTile* InTile, TArray<FVector2D>& OutPoints, float FoliageLB, float FoliageUB)
{
	// NOTE: we already passed the foliage check.
	
	// check intile anyway:
	if (InTile == nullptr)
	{
		return 0;
	}
	
	// we can have between 0-8 foliage. split the range from LB -> UB into 8 discrete thresholds:
	const float FoliageIncrementInterval = (FoliageLB - FoliageUB) / 8;
	
	// now normalize this tiles moisture against it and LB:
	const int32 NumFoliage =  (InTile->Moisture - FoliageUB) / FoliageIncrementInterval;
	
	for (int32 i = 0; i < NumFoliage; i++)
	{
		// For some reason, -1.5, -1.5 will be bottom right, while 1.5, 1.5 will be top left.
		// TODO: jitter. for now, completely randomize:
		OutPoints.Add(FVector2D(FMath::FRandRange(-1.5, 1.5),FMath::FRandRange(-1.5, 1.5)));
	}

	return NumFoliage;
}


FColor UGridManagerComponent::GetColorForTile(int32 InIdx)
{
	FGridTile* GridTile = GetGridTile(InIdx);
	if (GridTile == nullptr)
	{
		return FColor::Black;
	}
	
	switch (GridTile->Type) {
	case ETileType::Grass:
		return FColor::Green;
	case ETileType::Dirt:
		return FColor(124,124, 0);
	case ETileType::Sand:
		return FColor(124,124, 0);
	case ETileType::Water:
		return FColor::Blue;
	default:
		return FColor::Black;
	}
}
