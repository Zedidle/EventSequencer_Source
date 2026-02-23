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



int UEventSequenceDA::GetEventSequenceLengthWithNested(TArray<FInstancedStruct>& Events)
{
	if (Events.IsEmpty()) return 0;

	int Result = 0;
	
	// int EventSequenceNum = GetEventSequenceNumWithNested(Events);
	return Result;
}

void UEventSequenceDA::ParseEventsToDisplayName(TArray<FInstancedStruct>& Events)
{
	if (Events.IsEmpty()) return;
	
	for (auto& Event : Events)
    {
    	FString EventTitle;
		if (Event.GetScriptStruct()->IsChildOf(FBaseSequenceEvent::StaticStruct()))
		{
			if (const FBaseSequenceEvent* SourceEvent = Event.GetPtr<FBaseSequenceEvent>())
			{
				EventTitle = SourceEvent->GetDisplayName();
			}
		}

    	if (!EventTitle.IsEmpty())
    	{
    		FString FormattedNum = FString::Printf(TEXT("%03d"), CurNum);
    		DisplayName += "\n" + FormattedNum + " " +EventTitle;
    		CurNum++;
    	}

		if (F_SequenceEvent_IF* SourceEvent_IF = Event.GetMutablePtr<F_SequenceEvent_IF>())
		{
			SourceEvent_IF->TrueEventsStartIndex = CurNum;
			SourceEvent_IF->FalseEventsStartIndex = CurNum + FBaseSequenceEvent::GetEventListEventsCount(SourceEvent_IF->TrueEvents);

			EventTitle = SourceEvent_IF->GetDisplayName();
			
			if (!SourceEvent_IF->TrueEvents.IsEmpty())
			{
				ParseEventsToDisplayName(SourceEvent_IF->TrueEvents);
			}
			if (!SourceEvent_IF->FalseEvents.IsEmpty())
			{
				ParseEventsToDisplayName(SourceEvent_IF->FalseEvents);
			}
		}
		
		if (FBaseSequenceEvent* DestEvent = Event.GetMutablePtr<FBaseSequenceEvent>())
		{
			ParseEventsToDisplayName(DestEvent->NestedEvents);
		}
    }
}

void UEventSequenceDA::ResetDisplayName()
{
	CurNum = 0;
	DisplayName = "";

	ParseEventsToDisplayName(EventSequence);

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
		GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Cyan, *FString::Printf(TEXT("UEventSequenceDA PostInitProperties EventSequenceNum: %d"), EventSequence.Num()));
	}
}

void UEventSequenceDA::PostEditImport()
{
	Super::PostEditImport();
	UE_LOG(LogTemp, Warning , TEXT("UEventSequenceDA PostEditImport EventSequenceNum: %d"), EventSequence.Num());
}

void UEventSequenceDA::PostLoad()
{
	Super::PostLoad();
	UE_LOG(LogTemp, Warning , TEXT("UEventSequenceDA PostLoad EventSequenceNum: %d"), EventSequence.Num());

	ResetDisplayName();

}

void UEventSequenceDA::UpdateAssetBundleData()
{
	Super::UpdateAssetBundleData();
	UE_LOG(LogTemp, Warning , TEXT("UEventSequenceDA UpdateAssetBundleData EventSequenceNum: %d"), EventSequence.Num());
	
}

void UEventSequenceDA::PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);
	UE_LOG(LogTemp, Warning , TEXT("UEventSequenceDA PostEditChangeProperty EventSequenceNum: %d"), EventSequence.Num());
	// ResetDisplayName();

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
	if (EventSequence.IsValidIndex(Index))
	{
		OutEvent = EventSequence[Index];
		return true;
	}
	return false;
}

void UEventSequenceDA::AddEvent(const FInstancedStruct& NewEvent)
{
	if (NewEvent.IsValid() && NewEvent.GetPtr<FBaseSequenceEvent>())
	{
		EventSequence.Add(NewEvent);
	}
}

void UEventSequenceDA::FindEventsByType(const UScriptStruct* EventType,
	TArray<FInstancedStruct>& OutEvents) const
{
	OutEvents.Empty();
    
	for (const FInstancedStruct& Event : EventSequence)
	{
		if (Event.IsValid() && Event.GetScriptStruct()->IsChildOf(EventType))
		{
			OutEvents.Add(Event);
		}
	}
}
