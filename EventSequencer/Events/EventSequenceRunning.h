// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EventSequencer/CommonStructs.h"
#include "EventSequencer/DataAssets/EventSequenceDA.h"
#include "StructUtils/InstancedStruct.h"
#include "UObject/Object.h"
#include "EventSequenceRunning.generated.h"


UCLASS(BlueprintType)
class EVENTSEQUENCER_API UEventSequenceRunning : public UObject
{
	GENERATED_BODY()

public:
	// 可能某些Event标记有Label，则需要记录下来方便跳转
	TMap<FName, int> Label2Index;
	
	bool bPause = false;
	int CurEventIndex = 0;
	TArray<FInstancedStruct> EventQueue;

	// 初始化定义的DA，存储了 InstancedPropertyBag 数据
	UPROPERTY()
	UEventSequenceDA* InitDataAsset;

	UFUNCTION()
	void SetDataAsset(UEventSequenceDA* DataAsset);
	
	void Tick(float DeltaTime);
	
	void AddEventStruct(FInstancedStruct& EventStruct);
    
	bool IsCompleted() const;
    
	FInstancedStruct& GetCurEvent();
	
	UFUNCTION(BlueprintCallable, Category = "Event Sequence")
	void Goto(FName Label);
	
	UFUNCTION(BlueprintCallable, Category = "Event Sequence")
	void Start();
	
	UFUNCTION(BlueprintCallable, Category = "Event Sequence")
	void Stop();
	
	UFUNCTION(BlueprintCallable, Category = "Event Sequence")
	void Next();
	
	UFUNCTION(BlueprintCallable, Category = "Event Sequence")
	void Destroy();
	
	

	
};
