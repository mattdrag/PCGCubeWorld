#include "UI/ZXHUD.h"

#include "Blueprint/UserWidget.h"

void AZXHUD::BeginPlay()
{
	Super::BeginPlay();

	ReloadUI();
}

void AZXHUD::ReloadUI()
{
	if (!IsValid(LayoutClass))
	{
		UE_LOG(LogZX, Error, TEXT("Invalid HUD LayoutClass.."));
		return;
	}
	
	// Create Layout:
	if (APlayerController* PC = Cast<APlayerController>(GetOwner()))
	{
		Layout = CreateWidget<UUserWidget>(PC, LayoutClass);
		if (!IsValid(Layout))
		{
			UE_LOG(LogZX, Error, TEXT("Failed to create HUD Layout.."));
		}
		Layout->AddToViewport();
	}
}
