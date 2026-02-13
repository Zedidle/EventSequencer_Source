// Fill out your copyright notice in the Description page of Project Settings.


#include "EventSequenceFuncLib.h"

#include "EventSequencer/CommonStructs.h"
#include "EventSequencer/EventSequenceSystem.h"

FInstancedStruct UEventSequenceFuncLib::CreateMoveEvent(const FVector& TargetLocation, float ApproachDistance)
{
	FInstancedStruct MoveEvent;
	MoveEvent.InitializeAs<FMoveSequenceEvent>();
    
	if (auto* Event = MoveEvent.GetMutablePtr<FMoveSequenceEvent>())
	{
		Event->TargetLocation = TargetLocation;
		Event->ApproachDistance = ApproachDistance;
	}
    
	return MoveEvent;
}

FInstancedStruct UEventSequenceFuncLib::CreateDialogEvent(const TArray<FString>& DialogLines, float TextSpeed)
{
	FInstancedStruct DialogEvent;
	DialogEvent.InitializeAs<FDialogSequenceEvent>();
    
	if (auto* Event = DialogEvent.GetMutablePtr<FDialogSequenceEvent>())
	{
		Event->DialogLines = DialogLines;
		Event->TextDisplaySpeed = TextSpeed;
	}
    
	return DialogEvent;
}

FInstancedStruct UEventSequenceFuncLib::CreateDelayEvent(float Duration)
{
	FInstancedStruct DelayEvent;
	DelayEvent.InitializeAs<FWaitSequenceEvent>();
    
	if (auto* Event = DelayEvent.GetMutablePtr<FWaitSequenceEvent>())
	{
		Event->Duration = Duration;
	}
    
	return DelayEvent;
}

UEventSequenceRunning* UEventSequenceFuncLib::CreateEventSequence(UObject* WorldContextObject, UEventSequenceDA* TargetDataAsset)
{
	if (!WorldContextObject) return nullptr;
	
	UWorld* World = WorldContextObject->GetWorld();
	if (UEventSequenceSystem* EventSequenceSystem = UEventSequenceSystem::GetInstance(World))
	{
		return EventSequenceSystem->CreateEventSequence(TargetDataAsset);
	}
	
	return nullptr;
}

bool UEventSequenceFuncLib::RemoveEventSequence(UObject* WorldContextObject, UEventSequenceRunning* EventSequence)
{
	if (!WorldContextObject) return false;
	
	UWorld* World = WorldContextObject->GetWorld();
	if (UEventSequenceSystem* EventSequenceSystem = UEventSequenceSystem::GetInstance(World))
	{
		return EventSequenceSystem->RemoveEventSequence(EventSequence);
	}
	return false;
}
