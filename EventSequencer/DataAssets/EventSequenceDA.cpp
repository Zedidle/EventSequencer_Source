// Fill out your copyright notice in the Description page of Project Settings.

#include "EventSequenceDA.h"
#include "EventSequencer/CommonStructs.h"


void UEventSequenceDA::ResetDisplayName()
{
	DisplayName = "";

	for (auto& Event : EventSequence )
	{
		if (Event.GetScriptStruct()->IsChildOf(FMoveSequenceEvent::StaticStruct()))
		{
			if (const FMoveSequenceEvent* SourceEvent = Event.GetPtr<FMoveSequenceEvent>())
			{
				DisplayName += "\n" + SourceEvent->GetDisplayName();
			}
		}
		else if (Event.GetScriptStruct()->IsChildOf(FDialogSequenceEvent::StaticStruct()))
		{
			if (const FDialogSequenceEvent* SourceEvent = Event.GetPtr<FDialogSequenceEvent>())
			{
				DisplayName += "\n" + SourceEvent->GetDisplayName();
			}
		}
		else if (Event.GetScriptStruct()->IsChildOf(FWaitSequenceEvent::StaticStruct()))
		{
			if (const FWaitSequenceEvent* SourceEvent = Event.GetPtr<FWaitSequenceEvent>())
			{
				DisplayName += "\n" + SourceEvent->GetDisplayName();
			}
		}
		else if (Event.GetScriptStruct()->IsChildOf(FChoiceSequenceEvent::StaticStruct()))
		{
			if (const FChoiceSequenceEvent* SourceEvent = Event.GetPtr<FChoiceSequenceEvent>())
			{
				DisplayName += "\n" + SourceEvent->GetDisplayName();
			}
		}
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
	PropertyBagRuntime = PropertyBagDefault;

	PropertyBagRuntime.MigrateToNewBagInstance(PropertyBagInput);
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
