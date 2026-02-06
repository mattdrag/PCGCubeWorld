

#pragma once

#include "Core/ZX.h"
#include "GameFramework/HUD.h"
#include "ZXHUD.generated.h"

/**
 * 
 */
UCLASS()
class ZX_API AZXHUD : public AHUD
{
	GENERATED_BODY()

public:
	virtual void BeginPlay() override;

	UFUNCTION(Exec)
	void ReloadUI();

protected:
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UUserWidget> LayoutClass;
	
	UPROPERTY()
	TObjectPtr<UUserWidget> Layout;
};
