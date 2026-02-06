// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ZXDebugPanel.generated.h"

class UEditableTextBox;
class UCheckBox;
class UWidgetSwitcher;
class UZXCommonButtonBase;
/**
 * 
 */
UCLASS()
class ZX_API UZXDebugPanel : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeOnInitialized() override;

	UPROPERTY(meta = (BindWidget))
	const TObjectPtr<UWidgetSwitcher> ContentSwitcher;

	
#pragma region Player
	// header:
	UPROPERTY(meta = (BindWidget))
	const TObjectPtr<UZXCommonButtonBase> PlayerButton;

	// content:
	UPROPERTY(meta = (BindWidget))
	const TObjectPtr<UCheckBox> ShowPlayerInfo;
	
	
	UFUNCTION()
	void OnShowPlayerInfoToggled(bool bIsChecked);
#pragma endregion Player
	
#pragma region World
	// header:
	UPROPERTY(meta = (BindWidget))
	const TObjectPtr<UZXCommonButtonBase> WorldButton;

	// content:
	UPROPERTY(meta = (BindWidget))
	const TObjectPtr<UZXCommonButtonBase> RestyleButton;

	UPROPERTY(meta = (BindWidget))
	const TObjectPtr<UEditableTextBox> RestyleSeedText;

	UPROPERTY(meta = (BindWidget))
	const TObjectPtr<UEditableTextBox> NumCellularAutomataText;

	void OnRestyleClicked();
#pragma endregion World
};
