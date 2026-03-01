#include "WaitSequenceEvent.h"



FString FWaitSequenceEvent::GetDisplayName() const
{
	return Super::GetDisplayName() + FString::Printf(TEXT("Wait For: %f"), Property.Duration);
}

int FWaitSequenceEvent::GetEventsCount()
{
	return Super::GetEventsCount() + 1;
}

void FWaitSequenceEvent::OnEnter()
{
	FBaseSequenceEvent::OnEnter();
	GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Cyan, *FString::Printf(TEXT("FWaitSequenceEvent::OnEnter")));
}

bool FWaitSequenceEvent::Execute(int Index)
{
	GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Cyan, *FString::Printf(TEXT("FWaitSequenceEvent::Execute")));
	OnFinished();
	return true;
}

float FWaitSequenceEvent::Tick(float DeltaTime, float PreRemainTime)
{
	CurTime += DeltaTime + PreRemainTime;
	if (CurTime >= Property.Duration)
	{
		Execute();
		return CurTime - Property.Duration;
	}
	
	GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Cyan, *FString::Printf(TEXT("FWaitSequenceEvent::Tick Waiting")));
	return 0;
}

void FWaitSequenceEvent::OnFinished()
{
	FBaseSequenceEvent::OnFinished();
	GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Cyan, *FString::Printf(TEXT("FWaitSequenceEvent::OnFinished")));
}

void FWaitSequenceEvent::Reset()
{
	Super::Reset();
	CurTime = 0;
}


