// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ZXCommonButtonBase.h"
#include "IconTextButton.generated.h"

// Settings for icon text button:
USTRUCT(BlueprintType)
struct FIconTextButtonSetting
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere)
	FSlateBrush Brush;

	UPROPERTY(EditAnywhere)
	FWidgetTransform Transform;

	UPROPERTY(EditAnywhere)
	FText Text;
};


class UTextBlock;
class UImage;
/**
 * 
 */
UCLASS()
class ZX_API UIconTextButton : public UZXCommonButtonBase
{
	GENERATED_BODY()

public:
	virtual void NativePreConstruct() override;
	virtual void NativeConstruct() override;
	virtual void NativeOnInitialized() override;
	
	UPROPERTY(EditAnywhere)
	bool bStartToggled = false;
	
protected:
	UPROPERTY(EditAnywhere, Category = "Default")
	FIconTextButtonSetting DefaultSetting;

	UPROPERTY(EditAnywhere, Category = "Default")
	FIconTextButtonSetting PressedSetting;
	
	UPROPERTY(meta = (BindWidget))
	const TObjectPtr<UImage> Icon;

	UPROPERTY(meta = (BindWidget))
	const TObjectPtr<UTextBlock> TextBlock;
	
	void ToggleAppearance();
	
	bool bIconTextToggle = false;
};
