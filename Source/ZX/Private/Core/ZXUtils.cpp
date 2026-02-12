// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/ZXUtils.h"

#include "Framework/ZXGameState.h"
#include "Framework/ZXPlayerController.h"
#include "Data/ZXAssetManager.h"
#include "Pawn/ZXPawn.h"

UGridManagerComponent* UZXUtils::GetGridManager(const UObject* WorldContextObject)
{
	if (IsValid(WorldContextObject))
	{
		if (UWorld* World = WorldContextObject->GetWorld())
		{
			if (AZXGameState* MyGameState = Cast<AZXGameState>(World->GetGameState()))
			{
				return MyGameState->GetGridManager();
			}
		}
	}

	LOGZXE("Failed to get Grid Manager..");
	return nullptr;
}

UPawnManagerComponent* UZXUtils::GetPawnManager(const UObject* WorldContextObject)
{
	if (IsValid(WorldContextObject))
	{
		if (UWorld* World = WorldContextObject->GetWorld())
		{
			if (AZXGameState* MyGameState = Cast<AZXGameState>(World->GetGameState()))
			{
				return MyGameState->GetPawnManager();
			}
		}
	}

	LOGZXE("Failed to get Pawn Manager..");
	return nullptr;
}

AZXPlayerController* UZXUtils::GetZXController(const UObject* WorldContextObject)
{
	if (IsValid(WorldContextObject))
	{
		if (UWorld* World = WorldContextObject->GetWorld())
		{
			return Cast<AZXPlayerController>(World->GetFirstPlayerController());
		}
	}

	LOGZXE("Failed to get ZX Controller..");
	return nullptr;
}

AZXPawn* UZXUtils::GetZXPawn(const UObject* WorldContextObject)
{
	if (AZXPlayerController* PC = GetZXController(WorldContextObject))
	{
		return Cast<AZXPawn>(PC->GetPawn());
	}

	LOGZXE("Failed to get ZX Pawn..");
	return nullptr;
}

UZXAssetManager* UZXUtils::GetZXAssetManager()
{
	if (IsValid(GEngine))
	{
		UZXAssetManager* AssetManager = Cast<UZXAssetManager>(GEngine->AssetManager);
		if (IsValid(AssetManager))
		{
			return AssetManager;
		}	
	}

	LOGZXE("Failed to get ZX Asset Manager..");
	return nullptr;
}

UUIDelegates* UZXUtils::GetUIDelegates(const UObject* WorldContextObject, bool bLogError)
{
	if (AZXPlayerController* PC = GetZXController(WorldContextObject))
	{
		return PC->UIDelegates;
	}

	if (bLogError)
	{
		LOGZXE("Failed to get UIDelegates..");
	}
	
	return nullptr;
}
