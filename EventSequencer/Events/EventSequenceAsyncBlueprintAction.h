// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "EventSequenceAsyncBlueprintAction.generated.h"

class UPropertyBagWrapper;
// 异步操作结果
UENUM(BlueprintType)
enum class EAsyncActionResult : uint8
{
	Pending,    // 等待中
	Success,    // 成功
	Failed,      // 失败
	Cancelled  // 取消
};

// 异步中断语义
UENUM(BlueprintType)
enum class EAsyncInterruptBehavior : uint8
{
	Restart,    // 重启语义：中断后重新执行
	Skip        // 跳过语义：中断后视为失败
};

// 异步操作中断状态
USTRUCT(BlueprintType)
struct FAsyncInterruptState
{
	GENERATED_BODY()
    
	UPROPERTY()
	FGuid OperationID;
    
	UPROPERTY()
	float InterruptTime = 0.0f;
    
	UPROPERTY()
	FString SerializedState;
    
	UPROPERTY()
	bool bWasActive = false;
};


// 异步蓝图基类委托
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnAsyncActionResolved);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAsyncActionRejected, FString, Reason);


UCLASS(Blueprintable, Abstract, meta = (DisplayName = "Blueprint Async Action"))
class EVENTSEQUENCER_API UEventSequenceAsyncBlueprintAction : public UObject
{
	GENERATED_BODY()

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnAsyncCompleted, EAsyncActionResult, Result, const FString&, Reason);

protected:

    
	// 执行结果
	EAsyncActionResult Result = EAsyncActionResult::Pending;
    
	// 失败原因
	FString FailureReason;
    
	// 中断语义
	EAsyncInterruptBehavior InterruptBehavior = EAsyncInterruptBehavior::Restart;
    
	// 开始时间
	float StartTime = 0.0f;

	
public:
	UEventSequenceAsyncBlueprintAction();

	// 可以在OnStart中进行属性值修改
    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Event Sequence")
	void OnStart(UPropertyBagWrapper* InPropertyWrapper);
	
    // 执行异步行为
    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Event Sequence")
    void OnExecute();
    
    // 完成异步操作（成功）
    UFUNCTION(BlueprintCallable, Category = "Event Sequence | AsyncBlueprintAction")
    void Resolve();
    
    // 完成异步操作（失败）
    UFUNCTION(BlueprintCallable, Category = "Event Sequence | AsyncBlueprintAction")
    void Reject(const FString& Reason = TEXT(""));
    
    // 获取结果
    EAsyncActionResult GetResult() const { return Result; }
    
    // 获取失败原因
    FString GetFailureReason() const { return FailureReason; }
    
    // 是否完成
    bool IsCompleted() const { return Result != EAsyncActionResult::Pending; }
    
    // 是否成功
    bool IsSuccessful() const { return Result == EAsyncActionResult::Success; }
    
    // 重置状态
    void ResetState();
    
    // 设置中断语义
    void SetInterruptBehavior(EAsyncInterruptBehavior Behavior) { InterruptBehavior = Behavior; }
    EAsyncInterruptBehavior GetInterruptBehavior() const { return InterruptBehavior; }
    
    // 委托
    UPROPERTY(BlueprintAssignable, Category = "Event Sequence")
    FOnAsyncActionResolved OnResolved;
    
    UPROPERTY(BlueprintAssignable, Category = "Event Sequence")
    FOnAsyncActionRejected OnRejected;
    

	
};
