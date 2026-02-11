#include "Framework/ZXPlayerController.h"

#include "AbilitySystemComponent.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "Core/ZXUtils.h"
#include "Data/SkillSetData.h"
#include "Kismet/GameplayStatics.h"
#include "World/GridManagerComponent.h"
#include "Pawn/ZXPawn.h"
#include "Pawn/GridPawnAIController.h"
#include "Player/ZXCameraManager.h"

AZXPlayerController::AZXPlayerController()
{
	PlayerCameraManagerClass = AZXCameraManager::StaticClass();
}

void AZXPlayerController::BeginPlay()
{
	Super::BeginPlay();

	// Set the mouse cursor to always be shown
	SetShowMouseCursor(true);

	// initialize delegates:
	UIDelegates = NewObject<UUIDelegates>();
	
	// We listen for map closing (broadcasted by worldmap):
	if (IsValid(UIDelegates))
	{
		UIDelegates->OnMapClosed.AddLambda([&]()
		{
			bIsWorldMapOpen = false;
		});
	}
}

void AZXPlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	// init our camera manager:
	AZXCameraManager* MyCameraManager = Cast<AZXCameraManager>(PlayerCameraManager);
	if (IsValid(MyCameraManager))
	{
		CurrentZoom = MyCameraManager->Init(Cast<AZXPawn>(InPawn), ZoomSpeed);
	}
}

// Called to bind functionality to input
void AZXPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();
	
	if (!IsLocalPlayerController())
	{
		return;
	}
	
	// Use enhanced input:
	UEnhancedInputLocalPlayerSubsystem* EnhancedInputSubsystem =  ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer());
	if (!IsValid(EnhancedInputSubsystem))
	{
		UE_LOG(LogZX, Error, TEXT("EnhancedInputSubsystem invalid.."));
		return;
	}
	
	UEnhancedInputComponent* EnhancedInputComp = Cast<UEnhancedInputComponent>(InputComponent);;
	if (!IsValid(EnhancedInputComp))
	{
		UE_LOG(LogZX, Error, TEXT("Attempting to bind input to non ZX Player controller.."));
		return;
	}
	
	// Bind Actions:
	EnhancedInputComp->BindAction(IAMove, ETriggerEvent::Triggered, this, &AZXPlayerController::Move);
	EnhancedInputComp->BindAction(IAGridMove, ETriggerEvent::Started, this, &AZXPlayerController::GridMove);
	EnhancedInputComp->BindAction(IACameraFollow, ETriggerEvent::Completed, this, &ThisClass::FollowGridPawn);
	EnhancedInputComp->BindAction(IACameraUnfollow, ETriggerEvent::Completed, this, &ThisClass::UnfollowGridPawn);
	EnhancedInputComp->BindAction(IACommandMoveTo, ETriggerEvent::Completed, this, &ThisClass::CommandMoveTo);
	EnhancedInputComp->BindAction(IACameraZoomIn, ETriggerEvent::Triggered, this, &AZXPlayerController::OnZoomIn);
	EnhancedInputComp->BindAction(IACameraZoomOut, ETriggerEvent::Triggered, this, &AZXPlayerController::OnZoomOut);
	EnhancedInputComp->BindAction(IAOpenMap, ETriggerEvent::Started,this, &AZXPlayerController::OnMapPressed);
	
	// Setup Mappings:
	EnhancedInputSubsystem->ClearAllMappings();
	EnhancedInputSubsystem->AddMappingContext(ZXMappingContext, 0);
}

void AZXPlayerController::Move(const FInputActionValue& InActionValue)
{
	// we cannot move while map is open:
	if (bIsWorldMapOpen)
	{
		return;
	}
	
	// Move on 2D plane (no Z input rn)
	const FVector2D Input = InActionValue.Get<FInputActionValue::Axis2D>();
	AZXPawn* CameraPawn = UZXUtils::GetZXPawn(this);
	if (!IsValid(CameraPawn))
	{
		return;
	}
	CameraPawn->AddMovementInput(FVector(Input.X, Input.Y, 0), CameraPawn->MovementSpeed);
}

void AZXPlayerController::GridMove(const FInputActionValue& InActionValue)
{
	// teleport movement:
	const FVector2D Input = InActionValue.Get<FInputActionValue::Axis2D>();
	const FIntPoint IntegerInput = FIntPoint(Input.X, Input.Y);
	
	AZXPawn* MyPawn = UZXUtils::GetZXPawn(this);
	UGridManagerComponent* GridManager = UZXUtils::GetGridManager(this);
	if (IsValid(MyPawn) && IsValid(GridManager))
	{
		// add input to Coordinates:
		const FIntPoint NewCoords = GridManager->IndexToCoordinates(MyPawn->GetGridLocation()) + FIntPoint(IntegerInput.X, IntegerInput.Y);
		
		// if map is open, send input to map:
		if (bIsWorldMapOpen)
		{
			if (IsValid(UIDelegates))
			{
				UIDelegates->OnMapGridMove.Broadcast(IntegerInput);
			}
		}
		
		// else move pawn:
		else
		{
			const FVector NewWorldLoc = GridManager->CoordinatesToWorld(NewCoords);
			MyPawn->SetActorLocation(FVector(NewWorldLoc.X, NewWorldLoc.Y, MyPawn->GetActorLocation().Z));
		}
	}
}

void AZXPlayerController::FollowGridPawn(const FInputActionValue& InActionValue)
{
	// Toggle the check to follow the grid pawn

	FHitResult Hit;
	GetHitResultUnderCursorByChannel(SelectionTraceChannel, false, Hit);
	AGridPawn* GPawn = Cast<AGridPawn>(Hit.GetActor());
	if (!IsValid(GPawn))
	{
		UE_LOG(LogZX, Error, TEXT("Attempting to follow non gridpawn"));
		return;
	}
	SetCameraGridPawn(GPawn);
}

void AZXPlayerController::UnfollowGridPawn(const FInputActionValue& InActionValue)
{
	SetCameraGridPawn(nullptr);
}

void AZXPlayerController::CommandMoveTo(const FInputActionValue& InActionValue)
{
	// Tell brain of guy we are looking at where to go:

	// are we looking at a guy?
	if (AGridPawn* TheGuyWeAreLookingAt = GetCameraGridPawn())
	{
		if (AGridPawnAIController* GPAIC = Cast<AGridPawnAIController>(TheGuyWeAreLookingAt->Controller))
		{
			// yes, hit scan for cube:
			FHitResult Hit;
			if (GetHitResultUnderCursorByChannel(CommandTraceChannel, false, Hit))
			{
				GPAIC->Command_MoveTo(Hit.Location);
			}
		}
	}
}

void AZXPlayerController::OnZoomIn()
{
	AZXCameraManager* MyCameraManager = Cast<AZXCameraManager>(PlayerCameraManager);
	if (IsValid(MyCameraManager))
	{
		const float TargetZoom = CurrentZoom - ZoomAmount;
		
		// Zoom input gets routed to either our camera, or the world map:
		
		// Map:
		if (bIsWorldMapOpen)
		{
			if (IsValid(UIDelegates))
			{
				UIDelegates->OnMapZoom.Broadcast(TargetZoom - CurrentZoom);
			}
		}
		
		// Camera:
		else
		{
			// only set our current zoom if routing to camera:
			CurrentZoom = FMath::Clamp(TargetZoom, ZoomMin, ZoomMax);
			MyCameraManager->SetTargetOrthoWidth(CurrentZoom);
		}
	}
}

void AZXPlayerController::OnZoomOut()
{
	AZXCameraManager* MyCameraManager = Cast<AZXCameraManager>(PlayerCameraManager);
	if (IsValid(MyCameraManager))
	{
		const float TargetZoom = CurrentZoom + ZoomAmount;
		CurrentZoom = FMath::Clamp(TargetZoom, ZoomMin, ZoomMax);
		
		// Zoom input gets routed to either our camera, or the world map:
		
		// Map:
		if (bIsWorldMapOpen)
		{
			if (IsValid(UIDelegates))
			{
				UIDelegates->OnMapZoom.Broadcast(TargetZoom - CurrentZoom);
			}
		}
		
		// Camera:
		else
		{
			MyCameraManager->SetTargetOrthoWidth(CurrentZoom);
			
			// we trigger map opening if we are attempting to extend beyond camera max:
			if (TargetZoom > CurrentZoom)
			{
				bIsWorldMapOpen = true;
				if (IsValid(UIDelegates))
				{
					UIDelegates->OnMapOpened.Broadcast();
				}
			}
		}
	}
}

void AZXPlayerController::OnMapPressed()
{
	// TODO: for now, only trigger map through zooming..
}

AGridPawn* AZXPlayerController::GetCameraGridPawn() const
{
	return CameraFollowPawn;
}

AGridPawn* AZXPlayerController::SetCameraGridPawn(AGridPawn* GPawn)
{
	CameraFollowPawn = GPawn;
	return CameraFollowPawn;
}

void AZXPlayerController::DebugAttackEveryone()
{
	// note: every combat pawn has debug damage ability in their defaults..
	TArray<AActor*> AllCombatPawns;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ACombatPawn::StaticClass(), AllCombatPawns);
	for (AActor* CombatPawnActor : AllCombatPawns)
	{
		if (ACombatPawn* CombatPawn = Cast<ACombatPawn>(CombatPawnActor))
		{
			// stop hitting yourself xd
			CombatPawn->UseAbilityOnTarget(FGameplayTag::RequestGameplayTag("Ability.DebugDamage"), CombatPawn);
		}
	}
}
