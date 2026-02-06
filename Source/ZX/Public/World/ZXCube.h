// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Core/ZX.h"

#include "ZXCube.generated.h"

struct FGridTile;
class UTextRenderComponent;
enum class ETileType : uint8;

UENUM()
enum class ECubeDebugTextMode : uint8
{
	None,
	Index,
	Coordinate,
	TileType,
	Moisture
};

UCLASS()
class ZX_API AZXCube : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AZXCube();

	void SetData(const FGridTile& GridTile);
	
	UMaterialInstanceDynamic* GetDynamicMaterial(ETileType InTileType);
	
	void ToggleGridDebugText(uint8 Mode);
	
	int32 MyGridTileIdx = -1;
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* CubeMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UTextRenderComponent* GridIndexDebugText;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Materials")
	TMap<ETileType, UMaterialInterface*> TileTypeToMaterialMap;

private:
	UPROPERTY()
	UMaterialInstanceDynamic* DynamicCubeMaterial;
};
