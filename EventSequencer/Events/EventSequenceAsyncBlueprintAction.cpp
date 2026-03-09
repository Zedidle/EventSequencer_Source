// Fill out your copyright notice in the Description page of Project Settings.
#include "EventSequenceAsyncBlueprintAction.h"

UEventSequenceAsyncBlueprintAction::UEventSequenceAsyncBlueprintAction()
{
}

void UEventSequenceAsyncBlueprintAction::OnStart_Implementation(UPropertyBagWrapper* InPropertyWrapper)
{
}

// 可以与BlueprintCall一样 对 InPropertyWrapper 进行修改属性
void UEventSequenceAsyncBlueprintAction::OnExecute_Implementation()
{
	Resolve();
}

void UEventSequenceAsyncBlueprintAction::Resolve()
{
	if (Result == EAsyncActionResult::Pending)
	{
		Result = EAsyncActionResult::Success;
		OnResolved.Broadcast();
	}
}

void UEventSequenceAsyncBlueprintAction::Reject(const FString& Reason)
{
	if (Result == EAsyncActionResult::Pending)
	{
		Result = EAsyncActionResult::Failed;
		FailureReason = Reason;
		OnRejected.Broadcast(Reason);
	}
}

void UEventSequenceAsyncBlueprintAction::ResetState()
{
	Result = EAsyncActionResult::Pending;
	FailureReason.Empty();
	StartTime = 0.0f;
}
