// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "EventSequencer/DataAssets/EventSequenceDA.h"
#include "StructUtils/InstancedStruct.h"
#include "EventSequenceComponent.generated.h"


class UEventSequenceRunning;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class EVENTSEQUENCER_API UEventSequenceComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UEventSequenceComponent();

	UPROPERTY()
	UEventSequenceRunning* CurEventSequence;
	
protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void DestroyComponent(bool bPromoteChildren = false) override;

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintCallable, Category = "Event Factory")
	UEventSequenceRunning* CreateEventSequence(UEventSequenceDA* TargetDataAsset);
	
	UFUNCTION(BlueprintCallable, Category = "Event Factory")
	bool RemoveEventSequence();
		
};
