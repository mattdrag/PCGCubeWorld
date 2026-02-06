// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Runtime/AIModule/Classes/AIController.h"
#include "GridPawnAIController.generated.h"

struct FGridTile;
class UStateTreeAIComponent;

UCLASS()
class ZX_API AGridPawnAIController : public AAIController
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AGridPawnAIController();

	virtual void BeginPlay() override;

	void Command_MoveTo(const FVector& NewLocation);

	// Where did we start on the grid?
	int32 InitialGridTile = 0;
	
protected:
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI")
	TObjectPtr<UStateTreeAIComponent> StateTree;
};
