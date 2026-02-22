#include "WaitSequenceEvent.h"



FString FWaitSequenceEvent::GetDisplayName() const
{
	return Super::GetDisplayName() + FString::Printf(TEXT("Wait For: %f"), Property.Duration);
}

void FWaitSequenceEvent::OnEnter()
{
	FBaseSequenceEvent::OnEnter();
	GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Cyan, *FString::Printf(TEXT("FDialogSequenceEvent::OnEnter")));
}

bool FWaitSequenceEvent::Execute(int Index)
{
	GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Cyan, *FString::Printf(TEXT("FWaitSequenceEvent::Next")));
	OnFinished();
	return true;
}

void FWaitSequenceEvent::Tick(float DeltaTime)
{
	CurTime += DeltaTime;
	if (CurTime >= Property.Duration)
	{
		Execute();
	}
	
	GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Cyan, *FString::Printf(TEXT("FWaitSequenceEvent::Tick Waiting")));
}

void FWaitSequenceEvent::OnFinished()
{
	FBaseSequenceEvent::OnFinished();
}


