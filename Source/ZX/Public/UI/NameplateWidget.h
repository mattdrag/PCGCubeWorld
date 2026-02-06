// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "AbilitySystemInterface.h"
#include "Core/ZX.h"
#include "AttributeSet.h"
#include "Blueprint/UserWidget.h"
#include "NameplateWidget.generated.h"

struct FOnAttributeChangeData;
class UProgressBar;
/**
 * 
 */
UCLASS()
class ZX_API UNameplateWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	// sets up delegates from health attribute:
	void SetASC(UAbilitySystemComponent* InASC);
	
protected:
	virtual void NativeOnInitialized() override;
	virtual void NativeDestruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	void UpdateHealthBar(bool bOverrideWhiteBar = false);
	
	UPROPERTY(EditDefaultsOnly, Category = "Config")
	float WhiteBarLerpSpeed = 4.f;
	
	UPROPERTY(EditDefaultsOnly)
	FGameplayAttribute MaxHealthAttribute;
	
	UPROPERTY(EditDefaultsOnly)
	FGameplayAttribute HealthAttribute;
	
	UPROPERTY(meta = (BindWidget))
	const TObjectPtr<UProgressBar> HealthBar;
	
	UPROPERTY(meta = (BindWidget))
	const TObjectPtr<UProgressBar> WhiteBar;

private:
	TWeakObjectPtr<UAbilitySystemComponent> CachedASC;
	
	float MaxHealth = 1.f;
	float CurrentHealth = 1.f;
};
