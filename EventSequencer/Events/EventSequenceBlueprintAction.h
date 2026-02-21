// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "EventSequenceBlueprintAction.generated.h"


// 蓝图同步行为执行结果
UENUM(BlueprintType)
enum class EBlueprintActionResult : uint8
{
	Success,    // 执行成功
	Failure,    // 执行失败
	Running     // 执行中（异步）
};

// 蓝图同步行为上下文（注意：引擎已有FBlueprintActionContext，所以重命名）
USTRUCT(BlueprintType)
struct FSequenceBlueprintContext
{
	GENERATED_BODY()
    
	// 执行时间
	UPROPERTY(BlueprintReadOnly, Category = "Context")
	float DeltaTime = 0.0f;
    
	// 当前标签
	UPROPERTY(BlueprintReadOnly, Category = "Context")
	FName CurrentLabel;
    
	// 当前事件索引
	UPROPERTY(BlueprintReadOnly, Category = "Context")
	int32 CurrentEventIndex = -1;
    
	// 序列ID
	UPROPERTY(BlueprintReadOnly, Category = "Context")
	FGuid SequenceID;
    
	// 世界上下文
	UPROPERTY(BlueprintReadOnly, Category = "Context")
	TWeakObjectPtr<UObject> WorldContextObject;
    
	// 构造函数
	FSequenceBlueprintContext() = default;
    
	FSequenceBlueprintContext(UObject* InWorldContext)
		: WorldContextObject(InWorldContext)
	{}
};

UCLASS(Blueprintable, Abstract, meta = (DisplayName = "Blueprint Sync Action"))
class EVENTSEQUENCER_API UEventSequenceBlueprintAction : public UObject
{
	GENERATED_BODY()

	    
public:
	// 执行蓝图行为
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Event Sequence")
	EBlueprintActionResult Execute(const FSequenceBlueprintContext& Context);
	virtual EBlueprintActionResult Execute_Implementation(const FSequenceBlueprintContext& Context);
    
	// 获取执行上下文
	UFUNCTION(BlueprintPure, Category = "Event Sequence")
	virtual FSequenceBlueprintContext GetContext() const;
    
	// 设置上下文
	void SetContext(const FSequenceBlueprintContext& InContext);
    
	// 获取世界上下文
	UObject* GetWorldContext() const { return WorldContextObject.Get(); }
    
	// 设置世界上下文
	void SetWorldContext(UObject* InWorldContext) { WorldContextObject = InWorldContext; }
    
protected:
	// 执行上下文
	FSequenceBlueprintContext CurrentContext;
    
	// 世界上下文
	TWeakObjectPtr<UObject> WorldContextObject;
};
