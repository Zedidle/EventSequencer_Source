
#include "EventSequenceBlueprintAction.h"

EBlueprintActionResult UEventSequenceBlueprintAction::Execute_Implementation(const FSequenceBlueprintContext& Context)
{
	// 默认实现：返回成功
	return EBlueprintActionResult::Success;
}

FSequenceBlueprintContext UEventSequenceBlueprintAction::GetContext() const
{
	return CurrentContext;
}

void UEventSequenceBlueprintAction::SetContext(const FSequenceBlueprintContext& InContext)
{
	CurrentContext = InContext;
}