// Fill out your copyright notice in the Description page of Project Settings.

#include "Pawn/Tasks/ZXWanderTask.h"

#include "AIController.h"
#include "StateTreeExecutionContext.h"
#include "Core/ZXUtils.h"
#include "Pawn/GridPawnAIController.h"
#include "World/GridManagerComponent.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(ZXWanderTask)

#define LOCTEXT_NAMESPACE "StateTree"

FZXWanderTask::FZXWanderTask()
{
	bConsideredForScheduling = false;
	bShouldCopyBoundPropertiesOnTick = false;
	bShouldCopyBoundPropertiesOnExitState = false;
}

EStateTreeRunStatus FZXWanderTask::EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const
{
	FInstanceDataType& InstanceData = Context.GetInstanceData(*this);
	InstanceData.ScheduledTickHandle = Context.AddScheduledTickRequest(FStateTreeScheduledTick::MakeCustomTickRate(InstanceData.TickRate));
	UGridManagerComponent* GridManager = UZXUtils::GetGridManager(Context.GetWorld());
	AGridPawn* GridPawn = Cast<AGridPawn>(InstanceData.Actor);
	AGridPawnAIController* GridPawnAIC = Cast<AGridPawnAIController>(InstanceData.AIController);
	if (IsValid(GridManager) && IsValid(GridPawnAIC))
	{
		if (FGridTile* InitialGridTile = GridManager->GetGridTile(GridPawnAIC->InitialGridTile))
		{
			if (FGridTile* NearLeashTile = GridManager->GetOpenGridTile(InitialGridTile->MyIndex, InstanceData.WanderRadius))
			{
				InstanceData.TargetLocation = GridManager->CoordinatesToWorld(GridManager->IndexToCoordinates(NearLeashTile->MyIndex));
				InstanceData.TargetLocation = FVector(InstanceData.TargetLocation.X, InstanceData.TargetLocation.Y, InstanceData.TargetLocation.Z);
				GridPawnAIC->MoveToLocation(InstanceData.TargetLocation, InstanceData.AcceptanceTolerance/4, false, false); // MD-TODO: use index to world..
				return EStateTreeRunStatus::Running;
			}
		}
	}
	return EStateTreeRunStatus::Failed;
}

EStateTreeRunStatus FZXWanderTask::Tick(FStateTreeExecutionContext& Context, const float DeltaTime) const
{
	FInstanceDataType& InstanceData = Context.GetInstanceData(*this);
	AGridPawn* GridPawn = Cast<AGridPawn>(InstanceData.Actor);
	if (!GridPawn)
	{
		return EStateTreeRunStatus::Failed;
	}
	// are we there yet?
	const FVector CurrentLocation = GridPawn->GetActorLocation();
	if (FVector::Dist(InstanceData.TargetLocation, CurrentLocation) < InstanceData.AcceptanceTolerance)
	{
		return EStateTreeRunStatus::Succeeded;
	}
	return EStateTreeRunStatus::Running;
}

void FZXWanderTask::ExitState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const
{
	FInstanceDataType& InstanceData = Context.GetInstanceData(*this);
	Context.RemoveScheduledTickRequest(InstanceData.ScheduledTickHandle);
}

#undef LOCTEXT_NAMESPACE
