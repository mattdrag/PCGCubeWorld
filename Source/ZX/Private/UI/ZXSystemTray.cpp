// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/ZXSystemTray.h"

#include "UI/ZXCommonButtonBase.h"

void UZXSystemTray::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	DebugButton->OnClicked().AddLambda([&]()
	{
		// TODO: general function for other buttons in the tray:
		const bool bLocalToggle = !DebugButton->GetSelected();
		if (bLocalToggle)
		{
			// populate content if none:
			if (!ContentPanel->HasAnyChildren())
			{
				CurrentContent = CreateWidget<UUserWidget>(this, DebugPanelClass);
				ContentPanel->AddChild(CurrentContent.Get());
			}
		}
		// collapse/show panel
		ContentPanel->SetVisibility(bLocalToggle ? ESlateVisibility::SelfHitTestInvisible : ESlateVisibility::Collapsed);
	});
}
