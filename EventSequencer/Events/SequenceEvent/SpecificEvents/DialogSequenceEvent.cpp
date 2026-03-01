#include "DialogSequenceEvent.h"


FString FDialogSequenceEvent::GetDisplayName() const
{
	return Super::GetDisplayName() + FString::Printf(TEXT("Dialog Num: %d"), Property.DialogLines.Num());
}

int FDialogSequenceEvent::GetEventsCount()
{
	return Super::GetEventsCount() + 1;
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
	GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Cyan, *FString::Printf(TEXT("FDialogSequenceEvent::Execute")));
	return true;
}

float FDialogSequenceEvent::Tick(float DeltaTime, float PreRemainTime)
{
	GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Cyan, *FString::Printf(TEXT("FDialogSequenceEvent::Tick")));
	return 0;
}

void FDialogSequenceEvent::Reset()
{
	
}
