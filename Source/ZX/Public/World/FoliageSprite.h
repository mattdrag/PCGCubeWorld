// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GridEntityInterface.h"
#include "ZXSprite.h"
#include "FoliageSprite.generated.h"

class UFoliageData;

UCLASS()
class ZX_API AFoliageSprite : public AZXSprite, public IGridEntityInterface
{
	GENERATED_BODY()

public:
	void SetData(const UFoliageData& InData, int32 InGridCell);
	virtual int32 GetCurrentGridCell() override;

protected:
	int32 CurrentGridCell = -1;
};
