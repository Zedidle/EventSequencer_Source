// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "StructUtils/InstancedStruct.h"
#include "StructUtils/PropertyBag.h"
#include "EventSequenceDA.generated.h"

// 前置声明：确保所有序列事件结构体都有 GetDisplayName() 方法
template<typename T>
concept HasGetDisplayName = requires(const T* Event) {
	{ Event->GetDisplayName() } -> std::convertible_to<FString>;
};

/**
 * 通用模板函数：提取指定类型序列事件的显示标题
 * @tparam T 序列事件结构体类型（如 FMoveSequenceEvent、FDialogSequenceEvent 等）
 * @param Event 待判断的结构体实例
 * @return 事件的显示标题（若类型不匹配/指针获取失败，返回空字符串）
 */
template<typename T>
requires HasGetDisplayName<T>
FString GetSequenceEventTitle(const FInstancedStruct& Event)
{
	// 1. 判断结构体类型是否匹配（是否是 T 或 T 的子类）
	if (Event.GetScriptStruct()->IsChildOf(T::StaticStruct()))
	{
		// 2. 安全获取指定类型的指针
		if (const T* SourceEvent = Event.GetPtr<T>())
		{
			// 3. 提取显示标题
			return SourceEvent->GetDisplayName();
		}
	}
	// 类型不匹配/指针获取失败，返回空字符串
	return TEXT("");
}


struct FBaseSequenceEvent;

UCLASS(BlueprintType)
class EVENTSEQUENCER_API UEventSequenceDA : public UPrimaryDataAsset
{
	GENERATED_BODY()
	

	void ResetDisplayName();

	int CurNum = 0;

	
public:
	virtual void Serialize(FStructuredArchiveRecord Record) override;

	virtual void PostInitProperties() override;
    
	// 当DataAsset被导入时调用
	virtual void PostEditImport() override;
	
	virtual void PostLoad() override;

	virtual void UpdateAssetBundleData() override;

	virtual void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override;

	UPROPERTY(EditAnywhere)
	FInstancedPropertyBag PropertyBagDefault;
	FInstancedPropertyBag PropertyBagInput;

	void SetPropertyBagInput(const FInstancedPropertyBag& PropertyBag);
	
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
