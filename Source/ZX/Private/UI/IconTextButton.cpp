// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/IconTextButton.h"

#include "Components/Image.h"
#include "Components/TextBlock.h"

void UIconTextButton::NativePreConstruct()
{
	Super::NativePreConstruct();
	
#if WITH_EDITOR
	// need to be togglable:
	SetIsSelectable(true);
	SetIsToggleable(true);
	
	Icon->SetBrush(DefaultSetting.Brush);
	Icon->SetRenderTransform(DefaultSetting.Transform);
	TextBlock->SetText(DefaultSetting.Text);
#endif
}

void UIconTextButton::NativeConstruct()
{
	Super::NativeConstruct();
	
	// toggle icon + text on click:
	OnClicked().AddUObject(this, &ThisClass::ToggleAppearance);
}

void UIconTextButton::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	
	if (bStartToggled)
	{
		ToggleAppearance();
	}
}

void UIconTextButton::ToggleAppearance()
{
	bIconTextToggle = !bIconTextToggle;
	Icon->SetBrush(bIconTextToggle ? PressedSetting.Brush : DefaultSetting.Brush);
	Icon->SetRenderTransform(bIconTextToggle ? PressedSetting.Transform : DefaultSetting.Transform);
	TextBlock->SetText(bIconTextToggle ? PressedSetting.Text : DefaultSetting.Text);
}
