// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Core/ZX.h"
#include "PaperSpriteComponent.h"
#include "ZXSprite.generated.h"

/**
 * 
 */
UCLASS()
class ZX_API AZXSprite : public AActor
{
	GENERATED_BODY()
	
public:
	AZXSprite();
	
	void SetSprite(UPaperSprite* InSprite);
	
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UPaperSpriteComponent* SpriteComponent;
};
