// Fill out your copyright notice in the Description page of Project Settings.


#include "EventSequenceRunning.h"
#include "../Events/SequenceEvent/CommonStructs.h"
#include "EventSequencer/EventSequenceSystem.h"

void UEventSequenceRunning::AddEvent(FInstancedStruct& Event)
{
	if(!Event.IsValid()) return;
    
	if (auto* DestEvent = Event.GetMutablePtr<FBaseSequenceEvent>())
	{
		EventQueue.Add(Event);
	}
}

void UEventSequenceRunning::AppendEvents(TArray<FInstancedStruct>& Events)
{
	if (Events.IsEmpty()) return;
	
	for (auto& Event : Events)
	{
		AddEvent(Event);
	}
}

void UEventSequenceRunning::Goto(FName Label)
{
	if(int* index = Label2Index.Find(Label))
	{
		CurEventIndex = *index;
		for (int i = CurEventIndex; i < EventQueue.Num(); i++)
		{
			if(EventQueue.IsValidIndex(i))
			{
				if (auto* DestEvent =  EventQueue[i].GetMutablePtr<FBaseSequenceEvent>())
				{
					DestEvent->SetState(EEventState::Idle);
				}
			}
		} 
	}
}


bool UEventSequenceRunning::IsCompleted() const
{
	for (auto Event : EventQueue)
	{
		if (const FBaseSequenceEvent* E = Event.GetPtr<FBaseSequenceEvent>())
		{
			if (E->GetState() != EEventState::Completed)
			{
				return false;
			}
		}
	}
	return true;
}

FInstancedStruct& UEventSequenceRunning::GetCurEvent()
{
	return EventQueue[CurEventIndex];
}

void UEventSequenceRunning::Start()
{
	bPause = false;
}

void UEventSequenceRunning::Stop()
{
	bPause = true;
}

void UEventSequenceRunning::Next()
{
	FInstancedStruct& CurEvent = GetCurEvent();
	if (FBaseSequenceEvent* E = CurEvent.GetMutablePtr<FBaseSequenceEvent>())
	{
		if (E->Execute())
		{
			CurEventIndex++;
			if (CurEventIndex >= EventQueue.Num())
			{
				UE_LOG(LogTemp, Log, TEXT("Sequence Completed!"));
				CurEventIndex = INDEX_NONE;
			}
		}
	}
}

void UEventSequenceRunning::Destroy()
{
	if (UEventSequenceSystem* EventSequenceSystem = UEventSequenceSystem::GetInstance(GetWorld()))
	{
		EventSequenceSystem->RemoveEventSequence(this);
	}
}


void UEventSequenceRunning::SetDataAsset(UEventSequenceDA* DataAsset)
{
	if (!DataAsset) return;
	
	InitDataAsset = DataAsset;
	PropertyBagRuntime = InitDataAsset->PropertyBagDefault;
	PropertyBagRuntime.MigrateToNewBagInstance(InitDataAsset->PropertyBagInput);
}

void UEventSequenceRunning::Tick(float DeltaTime)
{
	if (bPause) return;
        
	FInstancedStruct& CurrentEvent = EventQueue[CurEventIndex];
	if (FBaseSequenceEvent* Event = CurrentEvent.GetMutablePtr<FBaseSequenceEvent>())
	{
		switch (Event->GetState())
		{
		case EEventState::Idle:
			Event->OnPending();
			break;
			
		case EEventState::Pending:
			Event->OnEnter();
			break;
            
		case EEventState::Active:
			Event->Tick(DeltaTime);
			break;
            
		case EEventState::Completed:
			Event->OnExit();
			CurEventIndex++;
            
			if (CurEventIndex >= EventQueue.Num())
			{
				UE_LOG(LogTemp, Log, TEXT("Sequence Completed!"));
				CurEventIndex = INDEX_NONE;
			}
			break;
		}
	}
}
