// Fill out your copyright notice in the Description page of Project Settings.


#include "World/ZXSprite.h"

AZXSprite::AZXSprite()
{
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	
	SpriteComponent = CreateDefaultSubobject<UPaperSpriteComponent>(TEXT("SpriteComponent"));
	SpriteComponent->SetupAttachment(RootComponent);
}

void AZXSprite::SetSprite(UPaperSprite* InSprite)
{
	SpriteComponent->SetSprite(InSprite);
}
