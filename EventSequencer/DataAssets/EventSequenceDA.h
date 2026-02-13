// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "StructUtils/InstancedStruct.h"
#include "EventSequenceDA.generated.h"


struct FBaseSequenceEvent;

UCLASS(BlueprintType,  meta = (TitleProperty = "DisplayName"))
class EVENTSEQUENCER_API UEventSequenceDA : public UPrimaryDataAsset
{
	GENERATED_BODY()
	

	void ResetDisplayName();
	
public:
	virtual void Serialize(FStructuredArchiveRecord Record) override;

	virtual void PostInitProperties() override;
    
	// 当DataAsset被导入时调用
	virtual void PostEditImport() override;
	
	virtual void PostLoad() override;

	virtual void UpdateAssetBundleData() override;

	virtual void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override;

	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	FString DisplayName = TEXT("666666666666666");  
	
	// 事件序列数组 - 使用FInstancedStruct支持多态事件
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Event Sequence", meta = (ShowOnlyInnerProperties, BaseStruct = "/Script/EventSequencer.BaseSequenceEvent"))
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
