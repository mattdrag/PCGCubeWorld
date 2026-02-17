// Fill out your copyright notice in the Description page of Project Settings.


#include "World/FoliageSprite.h"

#include "Data/FoliageData.h"

void AFoliageSprite::SetData(const UFoliageData& InData, int32 InGridCell)
{
	SetSprite(InData.Sprite);
	CurrentGridCell = InGridCell;
}

int32 AFoliageSprite::GetCurrentGridCell()
{
	return CurrentGridCell;
}
