// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ZX.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "ZXUtils.generated.h"

class UPawnManagerComponent;
class UUIDelegates;
class UZXAssetManager;
class AZXPlayerController;
class AZXPawn;
class UGridManagerComponent;
/**
 * 
 */
UCLASS()
class ZX_API UZXUtils : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	static UGridManagerComponent* GetGridManager(const UObject* WorldContextObject);

	UFUNCTION(BlueprintCallable)
	static UPawnManagerComponent* GetPawnManager(const UObject* WorldContextObject);

	UFUNCTION(BlueprintCallable)
	static AZXPlayerController* GetZXController(const UObject* WorldContextObject);
	
	UFUNCTION(BlueprintCallable)
	static AZXPawn* GetZXPawn(const UObject* WorldContextObject);

	UFUNCTION(BlueprintCallable)
	static UZXAssetManager* GetZXAssetManager();

	UFUNCTION(BlueprintCallable)
	static UUIDelegates* GetUIDelegates(const UObject* WorldContextObject);
};

UCLASS()
class ZX_API UUIDelegates : public UObject
{
	GENERATED_BODY()
	
public:
	/*
	 * Map:
	 */
	FSimpleMulticastDelegate OnMapGenerationComplete;
	DECLARE_MULTICAST_DELEGATE_OneParam(FOnMapOpened, FVector WorldLocation) FOnMapOpened OnMapOpened;
	FSimpleMulticastDelegate OnMapClosed;
	DECLARE_MULTICAST_DELEGATE_OneParam(FOnMapZoom, float ZoomAmount) FOnMapZoom OnMapZoom;
	DECLARE_MULTICAST_DELEGATE_OneParam(FMapGridMove, FIntPoint InMovementInput) FMapGridMove OnMapGridMove;
};