// Fill out your copyright notice in the Description page of Project Settings.


#include "EventSequenceFuncLib.h"
#include "EventSequencer/EventSequenceSystem.h"


FInstancedStruct UEventSequenceFuncLib::CreateMoveEvent(const FMoveSequenceProperty& Property)
{
	FInstancedStruct ResultEvent;
	ResultEvent.InitializeAs<FMoveSequenceEvent>();
    
	if (auto* Event = ResultEvent.GetMutablePtr<FMoveSequenceEvent>())
	{
		Event->Property.TargetLocation = Property.TargetLocation;
		Event->Property.ApproachDistance = Property.ApproachDistance;
	}
    
	return ResultEvent;
}

FInstancedStruct UEventSequenceFuncLib::CreateDialogEvent(const FDialogSequenceProperty& Property)
{
	FInstancedStruct ResultEvent;
	ResultEvent.InitializeAs<FDialogSequenceEvent>();
    
	if (auto* Event = ResultEvent.GetMutablePtr<FDialogSequenceEvent>())
	{
		Event->Property.DialogLines = Property.DialogLines;
		Event->Property.TextDisplaySpeed = Property.TextDisplaySpeed;
	}
    
	return ResultEvent;
}

FInstancedStruct UEventSequenceFuncLib::CreateWaitEvent(const FWaitSequenceProperty& Property)
{
	FInstancedStruct ResultEvent;
	ResultEvent.InitializeAs<FWaitSequenceEvent>();
    
	if (auto* Event = ResultEvent.GetMutablePtr<FWaitSequenceEvent>())
	{
		Event->Property.Duration = Property.Duration;
	}
    
	return ResultEvent;
}

FInstancedStruct UEventSequenceFuncLib::CreateChoiceEvent(const FChoiceSequenceProperty& Property)
{
	FInstancedStruct ResultEvent;
	ResultEvent.InitializeAs<FChoiceSequenceEvent>();
    
	if (auto* Event = ResultEvent.GetMutablePtr<FChoiceSequenceEvent>())
	{
		Event->Property.Choices = Property.Choices;
		Event->Property.ChoiceWidgetClass = Property.ChoiceWidgetClass;
	}
    
	return ResultEvent;
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
