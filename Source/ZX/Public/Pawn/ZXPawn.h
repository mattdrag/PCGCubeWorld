#pragma once

#include "Core/ZX.h"
#include "GameFramework/Pawn.h"
#include "Pawn/GridPawn.h"
#include "ZXPawn.generated.h"

class UCameraComponent;
class USpringArmComponent;
class UGridManagerComponent;
class UPawnManagerComponent;
struct FInputActionValue;
class UZXPawnMovementComponent;

/*
 * This is the players primary pawn. 
 * - you can think of it like a disembodied flying camera that attaches itself to other pawns and follows them around
 */
UCLASS()
class ZX_API AZXPawn : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	AZXPawn();
	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	UCameraComponent* CameraComponent;

	// Basic movement speed into movement input action rn:
	UPROPERTY(EditAnywhere, Category = Movement)
	float MovementSpeed = 1.f;
	
	// route location updates through here for chunk loading purposes:
	void SetGridLocation(const FIntPoint& InGridCoord);
	FORCEINLINE FIntPoint GetGridLocation() const { return LastGridLocation; }

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	void BufferCubes(const FIntPoint& OldLocation, const FIntPoint& NewLocation);
	
	void BufferLoad(int32 InIdx);
	void BufferUnload(int32 InIdx);

	UPROPERTY(EditDefaultsOnly, Category = "Loading")
	int32 LoadedCubesPerFrame = 10;
	
	UPROPERTY(EditDefaultsOnly, Category = "Loading")
	FIntPoint CubeLoadRange = FIntPoint(16,9);
	
	UPROPERTY(EditAnywhere)
	UZXPawnMovementComponent* MovementComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	USceneComponent* DefaultSceneRoot;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	USpringArmComponent* SpringArmComponent;

private:
	// we load grid tiles based on our location:
	FIntPoint LastGridLocation;
	
	FBox2D LoadedCubeWindow;
	
	TSet<int32> LoadedCubes;
	TSet<int32> CubeBuffer_Load;
	TSet<int32> CubeBuffer_Unload;
	
	// cache grid manager bc we check on tick:
	TWeakObjectPtr<UGridManagerComponent> CachedGridManager;
};
