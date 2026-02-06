// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/NameplateWidget.h"

#include "AbilitySystemComponent.h"
#include "Components/ProgressBar.h"

void UNameplateWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	
	UpdateHealthBar(true);
}

void UNameplateWidget::NativeDestruct()
{
	// Unbind health changes:
	if (UAbilitySystemComponent* ASC = CachedASC.Get())
	{
		ASC->GetGameplayAttributeValueChangeDelegate(HealthAttribute).RemoveAll(this);
	}
	
	Super::NativeDestruct();
}

void UNameplateWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
	
	// interp white bar to health bar:
	const float HealthBarPercent = HealthBar->GetPercent();
	const float WhiteBarPercent = WhiteBar->GetPercent();
	if (WhiteBarPercent > HealthBarPercent)
	{
		const float NewPercent = FMath::FInterpTo(WhiteBarPercent, HealthBarPercent, InDeltaTime, WhiteBarLerpSpeed);
		WhiteBar->SetPercent(NewPercent);
	}
}

void UNameplateWidget::SetASC(UAbilitySystemComponent* InASC)
{
	if (!IsValid(InASC))
	{
		LOGZXEF("invalid asc..");
		return;
	}
	
	// cache asc so we can unbind later:
	CachedASC = InASC;
	bool bDidFind; // MD-TODO: consider logging an error if stat not found

	// Max HP:
	MaxHealth = InASC->GetGameplayAttributeValue(MaxHealthAttribute, bDidFind);
	InASC->GetGameplayAttributeValueChangeDelegate(MaxHealthAttribute).AddLambda([&](const FOnAttributeChangeData& OnAttributeChangeData)
	{
		MaxHealth = OnAttributeChangeData.NewValue;
		UpdateHealthBar();
	});
	
	// HP:
	CurrentHealth = InASC->GetGameplayAttributeValue(MaxHealthAttribute, bDidFind);
	InASC->GetGameplayAttributeValueChangeDelegate(HealthAttribute).AddLambda([&](const FOnAttributeChangeData& OnAttributeChangeData)
	{
		CurrentHealth = OnAttributeChangeData.NewValue;
		UpdateHealthBar();
	});

	// update once:
	UpdateHealthBar(true);
}

void UNameplateWidget::UpdateHealthBar(bool bOverrideWhiteBar)
{
	const float HealthPercent = MaxHealth == 0 ? 1.0 : CurrentHealth / MaxHealth;
	HealthBar->SetPercent(HealthPercent);
	if (bOverrideWhiteBar)
	{
		WhiteBar->SetPercent(HealthPercent);
	}
}
