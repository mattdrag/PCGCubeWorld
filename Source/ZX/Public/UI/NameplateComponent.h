// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Core/ZX.h"
#include "Components/WidgetComponent.h"
#include "NameplateComponent.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class ZX_API UNameplateComponent : public UWidgetComponent
{
	GENERATED_BODY()
	
protected:
	virtual void BeginPlay() override;
	
	// Note: Comp maybe wants to handle visibility, occlusion, dist culling etc
};
