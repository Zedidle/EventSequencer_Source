#include "EventSequenceBlueprintAction.h"
#include "EventSequenceRunning.h"

// EBlueprintActionResult UEventSequenceBlueprintAction::Execute_Implementation(const FSequenceBlueprintContext& Context)
// {
// 	// 默认实现：返回成功
// 	return EBlueprintActionResult::Success;
// }

void UEventSequenceBlueprintAction::ExecuteTest_Implementation(const UPropertyBagWrapper* InProperty,
	UPropertyBagWrapper* OutProperty)
{
	InPropertyBag = NewObject<UPropertyBagWrapper>();
	OutPropertyBag = NewObject<UPropertyBagWrapper>();
	// InPropertyBag->SetPropertyBag(InProperty);
}

void UEventSequenceBlueprintAction::Execute_Implementation(UPropertyBagWrapper* InPropertyWrapper)
{
}

FSequenceBlueprintContext UEventSequenceBlueprintAction::GetContext() const
{
	return CurrentContext;
}

void UEventSequenceBlueprintAction::SetContext(const FSequenceBlueprintContext& InContext)
{
	CurrentContext = InContext;
}