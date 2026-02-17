// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "GridEntityInterface.generated.h"

UINTERFACE()
class UGridEntityInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class ZX_API IGridEntityInterface
{
	GENERATED_BODY()

public:
	// pure virtual:
	virtual int32 GetCurrentGridCell() = 0;
};