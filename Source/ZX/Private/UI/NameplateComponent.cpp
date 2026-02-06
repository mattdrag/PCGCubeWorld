// Fill out your copyright notice in the Description page of Project Settings.

#include "UI/NameplateComponent.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystemInterface.h"
#include "UI/NameplateWidget.h"

void UNameplateComponent::BeginPlay()
{
	Super::BeginPlay();

	// wow so generic much wow
	IAbilitySystemInterface* ASCOwner = Cast<IAbilitySystemInterface>(GetOwner());
	if (ASCOwner == nullptr || !IsValid(ASCOwner->GetAbilitySystemComponent()))
	{
		LOGZXEF("nameplate component attached to non asc interface..");
		return;
	}

	// get nameplate and set asc:
	UNameplateWidget* NameplateWidget = Cast<UNameplateWidget>(GetWidget());
	if (!IsValid(NameplateWidget))
	{
		LOGZXEF("nameplate widget invalid..");
		return;
	}
	
	NameplateWidget->SetASC(ASCOwner->GetAbilitySystemComponent());
}
