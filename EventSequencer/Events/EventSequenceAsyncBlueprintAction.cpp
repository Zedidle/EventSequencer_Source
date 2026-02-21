// Fill out your copyright notice in the Description page of Project Settings.
#include "EventSequenceAsyncBlueprintAction.h"

UEventSequenceAsyncBlueprintAction::UEventSequenceAsyncBlueprintAction()
{
}

void UEventSequenceAsyncBlueprintAction::OnExecute_Implementation(const FAsyncActionContext& Context)
{
	// 默认实现：立即完成
	CompleteSuccess();
}

FAsyncActionContext UEventSequenceAsyncBlueprintAction::GetContext() const
{
	return CurrentContext;
}

void UEventSequenceAsyncBlueprintAction::SetContext(const FAsyncActionContext& InContext)
{
	CurrentContext = InContext;
}

void UEventSequenceAsyncBlueprintAction::CompleteSuccess()
{
	if (Result == EAsyncActionResult::Pending)
	{
		Result = EAsyncActionResult::Success;
		OnCompleted.Broadcast();
	}
}

void UEventSequenceAsyncBlueprintAction::CompleteFailure(const FString& Reason)
{
	if (Result == EAsyncActionResult::Pending)
	{
		Result = EAsyncActionResult::Failed;
		FailureReason = Reason;
		OnFailed.Broadcast(Reason);
	}
}

void UEventSequenceAsyncBlueprintAction::ResetState()
{
	Result = EAsyncActionResult::Pending;
	FailureReason.Empty();
	SerializedState.Empty();
	StartTime = 0.0f;
}

bool UEventSequenceAsyncBlueprintAction::SupportsStateSerialization_Implementation() const
{
	return false; // 默认不支持状态序列化
}

FString UEventSequenceAsyncBlueprintAction::SerializeState_Implementation() const
{
	return TEXT(""); // 默认返回空状态
}

void UEventSequenceAsyncBlueprintAction::DeserializeState_Implementation(const FString& StateString)
{
	// 默认实现：不做任何事
}