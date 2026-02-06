

#pragma once

#include "Core/ZX.h"
#include "GameFramework/GameStateBase.h"
#include "ZXGameState.generated.h"

class UPawnManagerComponent;
class UGridManagerComponent;

/**
 * 
 */
UCLASS()
class ZX_API AZXGameState : public AGameStateBase
{
	GENERATED_BODY()

public:
	AZXGameState();

	// Debug, spawn a random guy.
	UFUNCTION(BlueprintCallable, CallInEditor, Exec, Category = "Grid")
	void SpawnGridPawn();
	
	// use only for debug purposes, will load all chunks.
	UFUNCTION(BlueprintCallable, CallInEditor, Category = "Grid")
	void DestroyGrid();

	// use only for debug purposes, will load all chunks.
	UFUNCTION(Exec)
	void ZXDisplayGridDebugText(uint8 Mode);
	
	FORCEINLINE UGridManagerComponent* GetGridManager() { return GridManager; }
	FORCEINLINE UPawnManagerComponent* GetPawnManager() { return PawnManager; }
	
protected:
	virtual void BeginPlay() override;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UGridManagerComponent> GridManager;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TSubclassOf<UGridManagerComponent> GridManagerClass;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UPawnManagerComponent> PawnManager;
	
	// Spawns entire grid on startup.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Grid")
	bool bSpawnGridOnStart = true;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Grid")
	int32 DebugSpawnRadius = 2;
};
