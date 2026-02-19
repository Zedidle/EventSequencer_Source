#include "DialogSequenceEvent.h"


FString FDialogSequenceEvent::GetDisplayName() const
{
	return Super::GetDisplayName() + FString::Printf(TEXT("Dialog Num: %d"), Property.DialogLines.Num());
}

void FDialogSequenceEvent::OnEnter()
{
	FBaseSequenceEvent::OnEnter();
	// 读表把对话内容加入到DialogItems
	// xxxxx
	GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Cyan, *FString::Printf(TEXT("FDialogSequenceEvent::OnEnter")));
	
}

bool FDialogSequenceEvent::Execute(int Index)
{
	GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Cyan, *FString::Printf(TEXT("FDialogSequenceEvent::Next")));
	return true;
}

void FDialogSequenceEvent::Tick(float DeltaTime)
{
	FBaseSequenceEvent::Tick(DeltaTime);
	GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Cyan, *FString::Printf(TEXT("FDialogSequenceEvent::Tick")));
}