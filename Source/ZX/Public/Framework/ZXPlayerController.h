

#pragma once

#include "CoreMinimal.h"
#include "EnhancedInputSubsystemInterface.h"
#include "GameFramework/PlayerController.h"
#include "ZXPlayerController.generated.h"

class UUIDelegates;
class AGridPawn;
class UInputAction;
/**
 * 
 */
UCLASS()
class ZX_API AZXPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	AZXPlayerController();
	
	virtual void BeginPlay() override;

	virtual void OnPossess(APawn* InPawn) override;
	
	// Called to bind functionality to input
	virtual void SetupInputComponent() override;
	
	void Move(const FInputActionValue& InActionValue);
	void GridMove(const FInputActionValue& InActionValue);
	void FollowGridPawn(const FInputActionValue& InActionValue);
	void UnfollowGridPawn(const FInputActionValue& InActionValue);
	void CommandMoveTo(const FInputActionValue& InActionValue);

	/*
	 * Input: using ue5 new Enhanced input subsystem bc old one is trash.
	 */

	// Mappings for our main camera pawn:
	UPROPERTY(EditAnywhere, Category = Input)
	UInputMappingContext* ZXMappingContext;

	// Input Actions:
	UPROPERTY(EditAnywhere, Category = Input)
	UInputAction* IAMove;
	
	UPROPERTY(EditAnywhere, Category = Input)
	UInputAction* IAGridMove;
	
	UPROPERTY(EditAnywhere, Category = Input)
	UInputAction* IACameraFollow;

	UPROPERTY(EditAnywhere, Category = Input)
	UInputAction* IACameraUnfollow;
	
	UPROPERTY(EditAnywhere, Category = Input)
	UInputAction* IACommandMoveTo;

	UPROPERTY(EditAnywhere, Category = Input)
	UInputAction* IACameraZoomIn;

	UPROPERTY(EditAnywhere, Category = Input)
	UInputAction* IACameraZoomOut;

	UPROPERTY(EditAnywhere, Category = Input)
	UInputAction* IAOpenMap;
	
	UPROPERTY(EditAnywhere, Category = Camera)
	float ZoomSpeed = 1.f;

	// how much to zoom per scroll:
	UPROPERTY(EditAnywhere, Category = Camera)
	float ZoomAmount = 100.f;
	
	UPROPERTY(EditAnywhere, Category = Camera)
	float ZoomMin = 300.f;
	
	UPROPERTY(EditAnywhere, Category = Camera)
	float ZoomMax = 1300.f;
	
	void OnZoomIn();
	void OnZoomOut();
	void OnMapPressed();
	
	AGridPawn* GetCameraGridPawn() const;
	AGridPawn* SetCameraGridPawn(AGridPawn* GPawn);

	UPROPERTY()
	TObjectPtr<UUIDelegates> UIDelegates;
	
protected:
	UPROPERTY(EditAnywhere, Category = "Command")
	TEnumAsByte<ETraceTypeQuery> CommandTraceChannel;

	UPROPERTY(EditAnywhere, Category = "Selection")
	TEnumAsByte<ETraceTypeQuery> SelectionTraceChannel;

	// Cheats:
	UFUNCTION(Exec)
	void DebugAttackEveryone();
	
private:
	//Pawn the camera is currently following
	UPROPERTY()
	AGridPawn* CameraFollowPawn = nullptr;
	
	// current camera zoom, is clamped by min/max
	float CurrentZoom = 0.f;
	
	// map is managed through zoom. we broadcast open, listen for close:
	bool bIsWorldMapOpen = false;
};
