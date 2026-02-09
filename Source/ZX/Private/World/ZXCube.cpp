// Fill out your copyright notice in the Description page of Project Settings.


#include "World/ZXCube.h"

#include "Core/ZXUtils.h"
#include "Components/TextRenderComponent.h"
#include "World/GridManagerComponent.h"
#include "World/GridTypes.h"


// Sets default values
AZXCube::AZXCube()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Setup Root
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
	SetRootComponent(RootComponent);

	// Setup cube mesh:
	CubeMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("CubeMesh"));
	CubeMesh->SetupAttachment(RootComponent);

	// Debug text:
	GridIndexDebugText = CreateDefaultSubobject<UTextRenderComponent>(TEXT("GridIndexDebugText"));
	GridIndexDebugText->SetupAttachment(RootComponent);
	GridIndexDebugText->SetText(FText::FromString("GridIdx"));
}

void AZXCube::SetData(const FGridTile& GridTile)
{
	// cache our grid tile:
	MyGridTileIdx = GridTile.MyIndex;

	// Also setup our debug text:
	if (UGridManagerComponent* GridManager = UZXUtils::GetGridManager(this))
	{
		GridIndexDebugText->SetText(FText::FromString(FString::FromInt(GridTile.MyIndex)));
		const FIntPoint MyCoords = GridManager->IndexToCoordinates(GridTile.MyIndex);
	}
}

void AZXCube::ToggleGridDebugText(uint8 Mode)
{
	const ECubeDebugTextMode TextMode = static_cast<ECubeDebugTextMode>(Mode);
	const bool bIsVis = GridIndexDebugText->IsVisible();
	GridIndexDebugText->SetVisibility(TextMode != ECubeDebugTextMode::None);
	switch (TextMode)
	{
	case ECubeDebugTextMode::None:
		GridIndexDebugText->SetText(FText::GetEmpty());
		break;
	case ECubeDebugTextMode::Index:
		GridIndexDebugText->SetText(FText::AsNumber(MyGridTileIdx));
		break;
	case ECubeDebugTextMode::Coordinate:
		if (UGridManagerComponent* GridManager = UZXUtils::GetGridManager(this))
		{
			const FIntPoint MyCoords = GridManager->IndexToCoordinates(MyGridTileIdx);
			GridIndexDebugText->SetText(FText::FromString(FString::Printf(TEXT("(%d, %d)"), MyCoords.X, MyCoords.Y)));
		}
		break;
	case ECubeDebugTextMode::TileType:
		if (UGridManagerComponent* GridManager = UZXUtils::GetGridManager(this))
		{
			if (FGridTile* MyGridTile = GridManager->GetGridTile(MyGridTileIdx))
			{
				GridIndexDebugText->SetText(FText::AsNumber(MyGridTile->Type == ETileType::Grass ? 1 : 0));
			}
		}
		break;
	case ECubeDebugTextMode::Altitude:
		if (UGridManagerComponent* GridManager = UZXUtils::GetGridManager(this))
		{
			if (FGridTile* MyGridTile = GridManager->GetGridTile(MyGridTileIdx))
			{
				GridIndexDebugText->SetText(FText::AsNumber(MyGridTile->Altitude));
			}
		}
		break;
	}
}

UMaterialInstanceDynamic* AZXCube::GetDynamicMaterial(ETileType InTileType)
{
	UMaterialInterface** MaterialInterface = TileTypeToMaterialMap.Find(InTileType);
	if (MaterialInterface == nullptr || !IsValid(*MaterialInterface))
	{
		LOGZXEF("Invalid material in tile map!");
		return nullptr;
	}
	
	// always create a new one:
	// NOTE: maybe we optimize this
	UMaterialInterface* CubeMaterial = *MaterialInterface;
	DynamicCubeMaterial = UMaterialInstanceDynamic::Create(CubeMaterial, this);
	CubeMesh->SetMaterial(0, DynamicCubeMaterial);
	
	return DynamicCubeMaterial;
}

// Called when the game starts or when spawned
void AZXCube::BeginPlay()
{
	Super::BeginPlay();

	GridIndexDebugText->SetVisibility(false);
}

