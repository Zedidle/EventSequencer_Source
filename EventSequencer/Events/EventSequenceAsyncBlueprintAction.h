// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "EventSequenceAsyncBlueprintAction.generated.h"

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

// 异步操作上下文
USTRUCT(BlueprintType)
struct FAsyncActionContext
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
    
	// 是否从中断中恢复
	UPROPERTY(BlueprintReadOnly, Category = "Context")
	bool bRecoveringFromInterrupt = false;
    
	// 构造函数
	FAsyncActionContext() = default;
    
	FAsyncActionContext(UObject* InWorldContext)
		: WorldContextObject(InWorldContext)
	{}
};


// 异步蓝图基类委托
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnAsyncActionCompleted);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAsyncActionFailed, const FString&, Reason);


/**
 * 异步蓝图行为基类
 * 所有自定义的异步行为都应该继承此类
 */
UCLASS()
class EVENTSEQUENCER_API UEventSequenceAsyncBlueprintAction : public UObject
{
	GENERATED_BODY()

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnAsyncCompleted, EAsyncActionResult, Result, const FString&, Reason);
    
public:
	UEventSequenceAsyncBlueprintAction();
	
    // 执行异步行为
    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Event Sequence")
    void OnExecute(const FAsyncActionContext& Context);
    virtual void OnExecute_Implementation(const FAsyncActionContext& Context);
    
    // 获取当前上下文
    UFUNCTION(BlueprintPure, Category = "Event Sequence")
    FAsyncActionContext GetContext() const;
    
    // 设置上下文
    void SetContext(const FAsyncActionContext& InContext);
    
    // 完成异步操作（成功）
    UFUNCTION(BlueprintCallable, Category = "Event Sequence", meta = (DisplayName = "Resolve"))
    void CompleteSuccess();
    
    // 完成异步操作（失败）
    UFUNCTION(BlueprintCallable, Category = "Event Sequence", meta = (DisplayName = "Reject"))
    void CompleteFailure(const FString& Reason = TEXT(""));
    
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
    
    // 可序列化状态
    UPROPERTY()
    FString SerializedState;
    
    // 是否支持状态序列化
    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Event Sequence")
    bool SupportsStateSerialization() const;
    virtual bool SupportsStateSerialization_Implementation() const;
    
    // 序列化当前状态
    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Event Sequence")
    FString SerializeState() const;
    virtual FString SerializeState_Implementation() const;
    
    // 反序列化状态
    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Event Sequence")
    void DeserializeState(const FString& StateString);
    virtual void DeserializeState_Implementation(const FString& StateString);
    
    // 委托
    UPROPERTY(BlueprintAssignable, Category = "Event Sequence")
    FOnAsyncActionCompleted OnCompleted;
    
    UPROPERTY(BlueprintAssignable, Category = "Event Sequence")
    FOnAsyncActionFailed OnFailed;
    
protected:
    // 执行上下文
    FAsyncActionContext CurrentContext;
    
    // 执行结果
    EAsyncActionResult Result = EAsyncActionResult::Pending;
    
    // 失败原因
    FString FailureReason;
    
    // 中断语义
    EAsyncInterruptBehavior InterruptBehavior = EAsyncInterruptBehavior::Restart;
    
    // 开始时间
    float StartTime = 0.0f;
	
};
