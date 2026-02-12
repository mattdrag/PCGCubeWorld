// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Data/ZXAssetManager.h"
#include "GameplayTagContainer.h"
#include "Net/UnrealNetwork.h"

DECLARE_LOG_CATEGORY_EXTERN(LogZX, Log, All);

#define LOGZX(Format, ...) UE_LOG(LogZX, Log, TEXT(Format), ##__VA_ARGS__)
#define LOGZXW(Format, ...) UE_LOG(LogZX, Warning, TEXT(Format), ##__VA_ARGS__)
#define LOGZXE(Format, ...) UE_LOG(LogZX, Error, TEXT(Format), ##__VA_ARGS__)

#define LOGZXF(Format, ...) UE_LOG(LogZX, Log, TEXT("%s : " Format), ANSI_TO_TCHAR(__FUNCTION__), ##__VA_ARGS__)
#define LOGZXWF(Format, ...) UE_LOG(LogZX, Warning, TEXT("%s : " Format), ANSI_TO_TCHAR(__FUNCTION__), ##__VA_ARGS__)
#define LOGZXEF(Format, ...) UE_LOG(LogZX, Error, TEXT("%s : " Format), ANSI_TO_TCHAR(__FUNCTION__), ##__VA_ARGS__)

// for debugging:
#define LOGZXSCR(Format, ...) \
if (GEngine) \
{ \
	GEngine->AddOnScreenDebugMessage( \
		-1, \
		4.f, \
		FColor::Yellow, \
		FString::Printf(TEXT(Format), ##__VA_ARGS__) \
	); \
}

// cvars:
namespace ZXCVars
{
	inline bool bCVarExample = false;
	inline TAutoConsoleVariable ExampleCvar(
		TEXT("zx.ExampleCvar"),
		bCVarExample,
		TEXT("example cvar."),
		ECVF_Default
	);
}