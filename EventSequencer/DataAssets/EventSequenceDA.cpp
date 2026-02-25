// Fill out your copyright notice in the Description page of Project Settings.

#include "EventSequenceDA.h"

#include "EventSequencer/Events/SequenceEvent/_SequenceEvent_BREAK.h"
#include "EventSequencer/Events/SequenceEvent/_SequenceEvent_GOTO.h"
#include "EventSequencer/Events/SequenceEvent/_SequenceEvent_IF.h"
#include "EventSequencer/Events/SequenceEvent/_SequenceEvent_LABEL.h"
#include "EventSequencer/Events/SequenceEvent/_SequenceEvent_LOOP.h"
#include "EventSequencer/Events/SequenceEvent/_SequenceEvent_RETURN.h"
#include "EventSequencer/Events/SequenceEvent/_SequenceEvent_SWITCH.h"
#include "EventSequencer/Events/SequenceEvent/SpecificEvents/ChoiceSequenceEvent.h"
#include "EventSequencer/Events/SequenceEvent/SpecificEvents/DialogSequenceEvent.h"
#include "EventSequencer/Events/SequenceEvent/SpecificEvents/MoveSequenceEvent.h"
#include "EventSequencer/Events/SequenceEvent/SpecificEvents/WaitSequenceEvent.h"


void UEventSequenceDA::PushDisplayTitle(const FString& Title)
{
	if (!Title.IsEmpty())
	{
		FString FormattedNum = FString::Printf(TEXT("%03d"), CurNum);
		DisplayName += "\n" + FormattedNum + " " + Title;
		CurNum++;
	}
}

int UEventSequenceDA::GetEventSequenceLengthWithNested(TArray<FInstancedStruct>& Events)
{
	if (Events.IsEmpty()) return 0;

	int Result = 0;
	
	// int EventSequenceNum = GetEventSequenceNumWithNested(Events);
	return Result;
}

void UEventSequenceDA::ParseEventsToDisplayName(TArray<FEventWrapper>& _EventWrappers)
{
	if (_EventWrappers.IsEmpty()) return;
	
	for (auto& EventWrapper : _EventWrappers)
    {
    	FString EventTitleString;
		if (FBaseSequenceEvent* DestEvent = EventWrapper.Event.GetMutablePtr<FBaseSequenceEvent>())
		{
			EventTitleString = DestEvent->GetDisplayName();
		}
		
		PushDisplayTitle(EventTitleString);
		
		if (F_SequenceEvent_LABEL* SourceEvent_LABEL = EventWrapper.Event.GetMutablePtr<F_SequenceEvent_LABEL>())
		{
		}
		else if (F_SequenceEvent_GOTO* SourceEvent_GOTO = EventWrapper.Event.GetMutablePtr<F_SequenceEvent_GOTO>())
		{
		}
		else if (F_SequenceEvent_IF* SourceEvent_IF = EventWrapper.Event.GetMutablePtr<F_SequenceEvent_IF>())
		{
			SourceEvent_IF->TrueEventsStartIndex = CurNum;
			SourceEvent_IF->FalseEventsStartIndex = CurNum + FBaseSequenceEvent::GetEventListEventsCount(SourceEvent_IF->TrueEvents) + 1;
			SourceEvent_IF->EndIndex = CurNum + FBaseSequenceEvent::GetEventListEventsCount(SourceEvent_IF->TrueEvents) +
												FBaseSequenceEvent::GetEventListEventsCount(SourceEvent_IF->FalseEvents) + 1;
			
			if (!SourceEvent_IF->TrueEvents.IsEmpty())
			{
				ParseEventsToDisplayName(SourceEvent_IF->TrueEvents);
			}

			PushDisplayTitle(F_SequenceEvent_GOTO(SourceEvent_IF->EndIndex).GetDisplayName());
			
			if (!SourceEvent_IF->FalseEvents.IsEmpty())
			{
				ParseEventsToDisplayName(SourceEvent_IF->FalseEvents);
			}
		}
		else if (F_SequenceEvent_SWITCH* SourceEvent_SWITCH = EventWrapper.Event.GetMutablePtr<F_SequenceEvent_SWITCH>())
		{
			SourceEvent_SWITCH->StartIndex = CurNum;
			SourceEvent_SWITCH->EndIndex = CurNum + SourceEvent_SWITCH->GetEventsCount() - 2;
			for (FEventCase& Case : SourceEvent_SWITCH->EventCases)
			{
				ParseEventsToDisplayName(Case.CaseEvents);
			}
		}
		else if (F_SequenceEvent_LOOP* SourceEvent_LOOP = EventWrapper.Event.GetMutablePtr<F_SequenceEvent_LOOP>())
		{
			SourceEvent_LOOP->State.LoopStartIndex = CurNum;
			SourceEvent_LOOP->State.LoopEndIndex = CurNum + FBaseSequenceEvent::GetEventListEventsCount(SourceEvent_LOOP->LoopEvents);

			ParseLoopStateStack.Push(SourceEvent_LOOP->State);
			ParseEventsToDisplayName(SourceEvent_LOOP->LoopEvents);
			PushDisplayTitle(F_SequenceEvent_GOTO(SourceEvent_LOOP->State.LoopStartIndex).GetDisplayName());
			ParseLoopStateStack.Pop();
		}
		else if (F_SequenceEvent_BREAK* SourceEvent_BREAK = EventWrapper.Event.GetMutablePtr<F_SequenceEvent_BREAK>())
		{
			if (!ParseLoopStateStack.IsEmpty())
			{
				const FEventState_LOOP& State_LOOP = ParseLoopStateStack.Last();
				SourceEvent_BREAK->InLoopEndIndex = State_LOOP.LoopEndIndex + 1;
			}
		}
		else if (F_SequenceEvent_RETURN* SourceEvent_RETURN = EventWrapper.Event.GetMutablePtr<F_SequenceEvent_RETURN>())
		{
		}
		// 具体事件
		else
		{
			if (FNestedSequenceEvent* DestEvent = EventWrapper.Event.GetMutablePtr<FNestedSequenceEvent>())
			{
				ParseEventsToDisplayName(DestEvent->NestedEvents);
			}
		}
		

    }
}

void UEventSequenceDA::ResetDisplayName()
{
	CurNum = 0;
	DisplayName = "";

	ParseEventsToDisplayName(EventWrappers);

	if (DisplayName.StartsWith(TEXT("\n")))
	{
		DisplayName = DisplayName.RightChop(1);
	}

}

void UEventSequenceDA::Serialize(FStructuredArchiveRecord Record)
{
	Super::Serialize(Record);
	// if (GEngine)
	// {
	// 	GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Cyan, *FString::Printf(TEXT("UEventSequenceDA Serialize EventSequenceNum: %d"), EventSequence.Num()));
	// }
}

void UEventSequenceDA::PostInitProperties()
{
	Super::PostInitProperties();
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Cyan, *FString::Printf(TEXT("UEventSequenceDA PostInitProperties EventSequenceNum: %d"), EventWrappers.Num()));
	}
}

void UEventSequenceDA::PostEditImport()
{
	Super::PostEditImport();
}

void UEventSequenceDA::PostLoad()
{
	Super::PostLoad();

	FCoreDelegates::OnEndFrame.AddLambda([this]
	{
		ResetDisplayName();
	});
}

void UEventSequenceDA::UpdateAssetBundleData()
{
	Super::UpdateAssetBundleData();
	
}

void UEventSequenceDA::PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	FCoreDelegates::OnEndFrame.AddLambda([this]
	{
		ResetDisplayName();
	});
	
}


void UEventSequenceDA::SetPropertyBagInput(const FInstancedPropertyBag& PropertyBag)
{
	PropertyBagInput = PropertyBag;
}

bool UEventSequenceDA::GetEventAtIndex(int32 Index, FInstancedStruct& OutEvent) const
{
	if (EventWrappers.IsValidIndex(Index))
	{
		OutEvent = EventWrappers[Index].Event;
		return true;
	}
	return false;
}


void UEventSequenceDA::FindEventsByType(const UScriptStruct* EventType,
	TArray<FInstancedStruct>& OutEvents) const
{
	OutEvents.Empty();
    
	for (const FEventWrapper& EventWrapper : EventWrappers)
	{
		const FInstancedStruct& Event = EventWrapper.Event;
		if (Event.IsValid() && Event.GetScriptStruct()->IsChildOf(EventType))
		{
			OutEvents.Add(Event);
		}
	}
}
