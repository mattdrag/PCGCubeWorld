#include "Core/ZXUtils.h"
#include "Data/BiomeData.h"
#include "Data/TileSetData.h"
#include "Data/ZXAssetManager.h"
#include "World/GridTypes.h"
#include "World/GridManagerComponent.h"
#include "World/ZXCube.h"

namespace GridManagerStylingConsts
{
	const int32 NumFoliageSlots = 8;
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

void UGridManagerComponent::StyleCube(AZXCube* InCube)
{
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
	TObjectPtr<UBiomeData>* CurrentBiome = BiomeData.Find(FGameplayTag::RequestGameplayTag("Data.Biome.TestGrasslands"));
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
	
	// Base (dirt):
	DynCubeMat->SetTextureParameterValue("TopTexture", TileSet->Layer0);
	DynCubeMat->SetTextureParameterValue("SideTexture", TileSet->Layer0);
	
	// Darken based on perlin val:
	DynCubeMat->SetScalarParameterValue("Darken", InTile->Moisture);

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
		//  - has to pass a moisture check:
		if (InTile->Moisture > MoistureThresh_FoliageLB)
		{
			TArray<FVector2D> FoliageUVs;
			GetJitteredGridForTile(InTile, FoliageUVs);
	
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
	}
	
	// Water:
	if (InTile->Type == ETileType::Water)
	{
		const uint8 TileSheetIndex = Autotile(ETileType::Water, InCoordinate);
		
		if (TileSet->Water.IsValidIndex(TileSheetIndex))
		{
			DynCubeMat->SetTextureParameterValue("WaterTexture", TileSet->Water[TileSheetIndex]);
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

int32 UGridManagerComponent::GetJitteredGridForTile(FGridTile* InTile, TArray<FVector2D>& OutPoints)
{
	// NOTE: we already passed the foliage check.
	
	// check intile anyway:
	if (InTile == nullptr)
	{
		return 0;
	}
	
	// we can have between 0-8 foliage. split the range from LB -> UB into 8 discrete thresholds:
	const float FoliageIncrementInterval = (MoistureThresh_FoliageUB - MoistureThresh_FoliageLB) / 8;
	
	// now normalize this tiles moisture against it and LB:
	const int32 NumFoliage =  (InTile->Moisture - MoistureThresh_FoliageLB) / FoliageIncrementInterval;
	
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
