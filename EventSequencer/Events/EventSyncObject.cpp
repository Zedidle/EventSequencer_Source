// Fill out your copyright notice in the Description page of Project Settings.


#include "EventSyncObject.h"

UEventSyncObject::UEventSyncObject(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	if ( HasAnyFlags(RF_ClassDefaultObject) == false )
	{
		AddToRoot();
	}
}


UEventSyncObject* UEventSyncObject::MakeSyncEvent(EEventType Type)
{
	UEventSyncObject* SyncObj = NewObject<UEventSyncObject>();
	SyncObj->EventType = Type;
	return SyncObj;
}
