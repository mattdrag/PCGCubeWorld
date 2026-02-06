// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/ZXDebugPanel.h"

#include "Core/ZXUtils.h"
#include "Components/CheckBox.h"
#include "Components/EditableTextBox.h"
#include "Components/WidgetSwitcher.h"
#include "UI/ZXCommonButtonBase.h"
#include "World/GridManagerComponent.h"

namespace ZXDebugPanelConsts
{
	constexpr int32 PLAYER_INDEX = 0;
	constexpr int32 WORLD_INDEX = 1;
}

void UZXDebugPanel::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	PlayerButton->OnClicked().AddLambda([&]()
	{
		ContentSwitcher->SetActiveWidgetIndex(ZXDebugPanelConsts::PLAYER_INDEX);
	});
	
	ShowPlayerInfo->OnCheckStateChanged.AddDynamic(this, &ThisClass::OnShowPlayerInfoToggled);
	
	
	WorldButton->OnClicked().AddLambda([&]()
	{
		ContentSwitcher->SetActiveWidgetIndex(ZXDebugPanelConsts::WORLD_INDEX);
	});
	
	RestyleButton->OnClicked().AddUObject(this, &ThisClass::OnRestyleClicked);
	
	// start w/ player:
	PlayerButton->SetIsSelected(true, false);
}

void UZXDebugPanel::OnShowPlayerInfoToggled(bool bIsChecked)
{
	AZXPlayerController* ZXController = UZXUtils::GetZXController(this);
}

void UZXDebugPanel::OnRestyleClicked()
{
	if (UGridManagerComponent* GridManager = UZXUtils::GetGridManager(this))
	{
		FCellularAutomataOptions CAOptions;
		CAOptions.NumIterations = FCString::Atoi(*NumCellularAutomataText->GetText().ToString());
		GridManager->SpawnEntireGrid(FCString::Atoi(*RestyleSeedText->GetText().ToString()), CAOptions);
	}
}
