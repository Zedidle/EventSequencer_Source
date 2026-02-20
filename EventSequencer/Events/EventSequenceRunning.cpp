// Fill out your copyright notice in the Description page of Project Settings.


#include "EventSequenceRunning.h"
#include "../Events/SequenceEvent/CommonStructs.h"
#include "EventSequencer/EventSequenceSystem.h"
#include "SequenceEvent/SequenceEvent_BREAK.h"
#include "SequenceEvent/SequenceEvent_GOTO.h"
#include "SequenceEvent/SequenceEvent_IF.h"
#include "SequenceEvent/SequenceEvent_LOOP.h"
#include "SequenceEvent/SequenceEvent_RETURN.h"

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
		Goto(*index);
	}
}

void UEventSequenceRunning::Goto(int Index)
{
	if(EventQueue.IsValidIndex(Index))
	{
		CurEventIndex = Index;
		if (auto* DestEvent =  EventQueue[Index].GetMutablePtr<FBaseSequenceEvent>())
		{
			DestEvent->SetState(EEventState::Idle);
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

void UEventSequenceRunning::AddLabel(FName Label)
{
	// 暂不考虑Label重复问题
	Label2Index.Add(Label,EventQueue.Num());
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
        
	FInstancedStruct& CurEventStruct = EventQueue[CurEventIndex];
	if (const FSequenceEvent_GOTO* CurEvent_GOTO = CurEventStruct.GetPtr<FSequenceEvent_GOTO>())
	{
		Goto(CurEvent_GOTO->TargetLabel);
	}
	else if (const FSequenceEvent_BREAK* CurEvent_BREAK = CurEventStruct.GetPtr<FSequenceEvent_BREAK>())
	{
		// 跳出一层循环状态
		
	}
	else if (const FSequenceEvent_LOOP* CurEvent_LOOP = CurEventStruct.GetPtr<FSequenceEvent_LOOP>())
	{
		// 进入循环状态
		
	}
	else if (const FSequenceEvent_IF* CurEvent_IF = CurEventStruct.GetPtr<FSequenceEvent_IF>())
	{
		// 进入条件分支判断
		
	}
	else if (const FSequenceEvent_RETURN* CurEvent_RETURN = CurEventStruct.GetPtr<FSequenceEvent_RETURN>())
	{
		// 结束该序列
		
	}
	else if (FBaseSequenceEvent* Event = CurEventStruct.GetMutablePtr<FBaseSequenceEvent>())
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
