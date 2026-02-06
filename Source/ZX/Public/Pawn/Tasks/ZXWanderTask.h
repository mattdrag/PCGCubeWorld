// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "StateTreeTaskBase.h"
#include "ZXWanderTask.generated.h"

class AAIController;
enum class EStateTreeRunStatus : uint8;
struct FStateTreeTransitionResult;

USTRUCT()
struct FZXWanderTaskInstanceData
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, Category = Context)
	TObjectPtr<AActor> Actor = nullptr;

	UPROPERTY(EditAnywhere, Category = Context)
	TObjectPtr<AAIController> AIController = nullptr;

	// Radius of wander
	UPROPERTY(EditAnywhere, Category = Parameter)
	int32 WanderRadius = 4;

	// Acceptance Tolerance
	UPROPERTY(EditAnywhere, Category = Parameter)
	float AcceptanceTolerance = 0.001;

	// How often to check if we arrived..
	UPROPERTY(EditAnywhere, Category = Parameter)
	float TickRate = 1.f;

	// Keep track of our target destination:
	FVector TargetLocation;
	
	/** The handle of the scheduled tick request. */
	UE::StateTree::FScheduledTickHandle ScheduledTickHandle;
};

/**
 * Simple task to wander around on the grid.
 */
USTRUCT(meta = (DisplayName = "ZX Wander Task"))
struct FZXWanderTask : public FStateTreeTaskCommonBase
{
	GENERATED_BODY()

	using FInstanceDataType = FZXWanderTaskInstanceData;
	
	FZXWanderTask();

	virtual const UStruct* GetInstanceDataType() const override { return FInstanceDataType::StaticStruct(); }

	virtual EStateTreeRunStatus EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const override;
	virtual EStateTreeRunStatus Tick(FStateTreeExecutionContext& Context, const float DeltaTime) const override;
	virtual void ExitState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const override;
};