// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "EventSequencer/Events/SequenceEvent/_SequenceEvent_LOOP.h"
#include "StructUtils/InstancedStruct.h"
#include "StructUtils/PropertyBag.h"
#include "EventSequenceDA.generated.h"



struct FBaseSequenceEvent;

UCLASS(BlueprintType)
class EVENTSEQUENCER_API UEventSequenceDA : public UPrimaryDataAsset
{
	GENERATED_BODY()

	// 解析所用循环状态栈
	TArray<FEventState_LOOP> ParseLoopStateStack;
	
	void PushDisplayTitle(const FString& Title);
	int GetEventSequenceLengthWithNested(TArray<FInstancedStruct>& Events);
	void ParseEventsToDisplayName(TArray<FInstancedStruct>& Events);
	void ResetDisplayName();

	int CurNum = 0;

	
public:
	virtual void Serialize(FStructuredArchiveRecord Record) override;

	virtual void PostInitProperties() override;
    
	// 当DataAsset被导入时调用
	virtual void PostEditImport() override;
	
	virtual void PostLoad() override;

	virtual void UpdateAssetBundleData() override;

	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString DisplayName = TEXT("999");
	
	UPROPERTY(EditAnywhere)
	FInstancedPropertyBag PropertyBagDefault;
	FInstancedPropertyBag PropertyBagInput;
	void SetPropertyBagInput(const FInstancedPropertyBag& PropertyBag);
	
	// 事件序列数组 - 使用FInstancedStruct支持多态事件
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (ShowOnlyInnerProperties, BaseStruct = "/Script/EventSequencer.BaseSequenceEvent"))
	TArray<FInstancedStruct> EventSequence;
	
	// 通过索引获取事件
	UFUNCTION(BlueprintCallable, Category = "Event Sequence")
	bool GetEventAtIndex(int32 Index, FInstancedStruct& OutEvent) const;

	// 添加新事件到序列
	UFUNCTION()
	void AddEvent(const FInstancedStruct& NewEvent);

	// 获取序列长度
	UFUNCTION(BlueprintPure, Category = "Event Sequence")
	int32 GetSequenceLength() const { return EventSequence.Num(); }

	// 查找特定类型的事件
	UFUNCTION(BlueprintCallable, Category = "Event Sequence")
	void FindEventsByType(const UScriptStruct* EventType, TArray<FInstancedStruct>& OutEvents) const;


};
