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



#define TRY_GET_EVENT_TITLE(Type) \
	if (EventTitle.IsEmpty()) EventTitle = GetSequenceEventTitle<Type>(Event);

void UEventSequenceDA::ResetDisplayName()
{
	CurNum = 0;
	DisplayName = "";

	for (auto& Event : EventSequence )
	{
		FString EventTitle;
		TRY_GET_EVENT_TITLE(F_SequenceEvent_BREAK)
		TRY_GET_EVENT_TITLE(F_SequenceEvent_GOTO)
		TRY_GET_EVENT_TITLE(F_SequenceEvent_IF)
		TRY_GET_EVENT_TITLE(F_SequenceEvent_LABEL)
		TRY_GET_EVENT_TITLE(F_SequenceEvent_LOOP)
		TRY_GET_EVENT_TITLE(F_SequenceEvent_RETURN)
		TRY_GET_EVENT_TITLE(F_SequenceEvent_SWITCH)
		
		TRY_GET_EVENT_TITLE(FMoveSequenceEvent)
		TRY_GET_EVENT_TITLE(FDialogSequenceEvent)
		TRY_GET_EVENT_TITLE(FWaitSequenceEvent)
		TRY_GET_EVENT_TITLE(FChoiceSequenceEvent)

		if (!EventTitle.IsEmpty())
		{
			FString FormattedNum = FString::Printf(TEXT("%03d"), CurNum);
			DisplayName += "\n" + FormattedNum + " " +EventTitle;
			CurNum++;
		}
	}

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
	ResetDisplayName();
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
