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
	void ParseEventsToDisplayName(TArray<FEventWrapper>& _EventWrappers);
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
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (TitleProperty = "{EventTitle}"))
	TArray<FEventWrapper> EventWrappers;
	
	// 通过索引获取事件
	UFUNCTION(BlueprintCallable, Category = "Event Sequence")
	bool GetEventAtIndex(int32 Index, FInstancedStruct& OutEvent) const;

	// 获取序列长度
	UFUNCTION(BlueprintPure, Category = "Event Sequence")
	int32 GetSequenceLength() const { return EventWrappers.Num(); }

	// 查找特定类型的事件
	UFUNCTION(BlueprintCallable, Category = "Event Sequence")
	void FindEventsByType(const UScriptStruct* EventType, TArray<FInstancedStruct>& OutEvents) const;


};
