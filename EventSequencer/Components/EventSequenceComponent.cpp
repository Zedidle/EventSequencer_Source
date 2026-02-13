// Fill out your copyright notice in the Description page of Project Settings.


#include "EventSequenceComponent.h"

#include "EventSequencer/EventSequenceSystem.h"


// Sets default values for this component's properties
UEventSequenceComponent::UEventSequenceComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UEventSequenceComponent::BeginPlay()
{
	Super::BeginPlay();

	
}

void UEventSequenceComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	RemoveEventSequence();
}


void UEventSequenceComponent::DestroyComponent(bool bPromoteChildren)
{
	Super::DestroyComponent(bPromoteChildren);

	if (IsValid(CurEventSequence))
	{
		RemoveEventSequence();
	}
	
	CurEventSequence = nullptr;
}

// Called every frame
void UEventSequenceComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                            FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

UEventSequenceRunning* UEventSequenceComponent::CreateEventSequence(UEventSequenceDA* TargetDataAsset)
{
	AActor* Owner = GetOwner();
	if (!IsValid(Owner)) return nullptr;
	
	UWorld* World = Owner->GetWorld();
	if (UEventSequenceSystem* EventSequenceSystem = UEventSequenceSystem::GetInstance(World))
	{
		CurEventSequence = EventSequenceSystem->CreateEventSequence(TargetDataAsset);
	}
	
	return CurEventSequence;
}

bool UEventSequenceComponent::RemoveEventSequence()
{
	AActor* Owner = GetOwner();
	if (!IsValid(Owner)) return false;
	
	UWorld* World = Owner->GetWorld();
	if (UEventSequenceSystem* EventSequenceSystem = UEventSequenceSystem::GetInstance(World))
	{
		return EventSequenceSystem->RemoveEventSequence(CurEventSequence);
	}
	
	return false;
}

