// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ZXSystemTray.generated.h"

class UZXCommonButtonBase;
/**
 * 
 */
UCLASS()
class ZX_API UZXSystemTray : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeOnInitialized() override;

	UPROPERTY(meta = (BindWidget))
	const TObjectPtr<UPanelWidget> ContentPanel;
	
	UPROPERTY(meta = (BindWidget))
	const TObjectPtr<UZXCommonButtonBase> DebugButton;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UUserWidget> DebugPanelClass;
	
	TWeakObjectPtr<UUserWidget> CurrentContent;
};
