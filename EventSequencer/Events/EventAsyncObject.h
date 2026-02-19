// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SequenceEvent/CommonStructs.h"
#include "Kismet/BlueprintAsyncActionBase.h"
#include "EventAsyncObject.generated.h"

UENUM(BlueprintType)
enum class EEventExecuteResult : uint8
{
	None        UMETA(DisplayName = "None"),
	// 事件类型预设
	Success		UMETA(DisplayName = "成功"), 
	Failed		UMETA(DisplayName = "失败"),
	Canceled	UMETA(DisplayName = "已取消")
};

// 中断处理语义
UENUM(BlueprintType)
enum class EInterruptBehavior : uint8
{
	Restart     UMETA(DisplayName = "重启"),
	Skip        UMETA(DisplayName = "跳过")
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FEventExecuteDelegate, EEventExecuteResult, Result);
// 异步完成委托
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FAsyncCompleteDelegate, EEventExecuteResult, Result, const FString&, Reason);




UCLASS(Blueprintable, MinimalAPI)
class UEventAsyncObject : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly)
	EEventType EventType = EEventType::None;
    
	
public:

	// 构造
	UEventAsyncObject();
    
	// 工厂函数 - 创建异步行为
	UFUNCTION(BlueprintCallable, Category = "Async", 
		meta = (BlueprintInternalUseOnly = "true", WorldContext = "WorldContextObject", 
				DisplayName = "Create Async Action", AutoCreateRefTerm = "Context"))
	static UEventAsyncObject* CreateAsyncEvent(UObject* WorldContextObject,
												const TMap<FString, FString>& Context,
												EInterruptBehavior InterruptBehavior = EInterruptBehavior::Restart);

    
    // 激活异步行为
    virtual void Activate() override;
    
    // 取消异步行为
    UFUNCTION(BlueprintCallable, Category = "Async")
    void Cancel(const FString& Reason = TEXT("Canceled"));
    
    // 重新启动（用于中断后恢复）
    UFUNCTION(BlueprintCallable, Category = "Async")
    void Restart();
    
    // 跳过当前异步行为
    UFUNCTION(BlueprintCallable, Category = "Async")
    void Skip(const FString& Reason = TEXT("Skipped"));
    
    // 设置上下文
    UFUNCTION(BlueprintCallable, Category = "Async")
    void SetContext(const TMap<FString, FString>& InContext);
    
    // 获取上下文值
    UFUNCTION(BlueprintPure, Category = "Async")
    FString GetContextValue(const FString& Key, const FString& DefaultValue = TEXT("")) const;
    
    // 设置上下文值
    UFUNCTION(BlueprintCallable, Category = "Async")
    void SetContextValue(const FString& Key, const FString& Value);
    
    // 委托
    UPROPERTY(BlueprintAssignable, Category = "Async|Events")
    FAsyncCompleteDelegate OnSuccess;
    
    UPROPERTY(BlueprintAssignable, Category = "Async|Events")
    FAsyncCompleteDelegate OnFailure;
    
    UPROPERTY(BlueprintAssignable, Category = "Async|Events")
    FAsyncCompleteDelegate OnCompleted;  // 无论成功失败都会触发
    
protected:
    // 重写此函数实现异步逻辑
    UFUNCTION(BlueprintImplementableEvent, Category = "Async|Events")
    void OnExecute();
    
    // 成功完成
    UFUNCTION(BlueprintCallable, Category = "Async|Control", meta = (DisplayName = "Resolve"))
    void Resolve(const FString& Reason = TEXT("Success"));
    
    // 失败
    UFUNCTION(BlueprintCallable, Category = "Async|Control", meta = (DisplayName = "Reject"))
    void Reject(const FString& Reason = TEXT("Failed"));
    
    // 内部完成处理
    virtual void InternalComplete(EEventExecuteResult Result, const FString& Reason);
    
    // 序列化支持
    virtual void Serialize(FArchive& Ar) override;
    
    // 获取序列化数据
    UFUNCTION(BlueprintPure, Category = "Async|State")
    TMap<FString, FString> GetSerializeData() const;
    
    // 从序列化数据恢复
    UFUNCTION(BlueprintCallable, Category = "Async|State")
    void RestoreFromData(const TMap<FString, FString>& Data);


	// 上下文数据
	UPROPERTY()
	TMap<FString, FString> ContextData;
	
	// 中断处理行为
	EInterruptBehavior InterruptBehavior = EInterruptBehavior::Restart;
	
private:
    
    // 当前状态
    EEventExecuteResult CurrentResult = EEventExecuteResult::None;
    
    // 是否正在执行
    bool bIsExecuting = false;
    
    // 是否被取消
    bool bIsCanceled = false;
    
    // 序列化键前缀
    static const FString SerializeKeyPrefix;



	
	

	
//     UFUNCTION(BlueprintCallable, Category = "Event Sync", meta = (BlueprintInternalUseOnly = "true"))
// 	static UEventAsyncObject* CreateAsyncEvent(EEventType Type);
//  
// 	UPROPERTY(BlueprintAssignable)
// 	FEventExecuteDelegate Resolve;
//  
// 	UPROPERTY(BlueprintAssignable)
// 	FEventExecuteDelegate Reject;
// 	
//  
// 	UFUNCTION(BlueprintCallable, Category = "Event Sync", meta = (BlueprintInternalUseOnly = "true"))
// 	virtual void Execute();


// protected:
// 	// 在蓝图中可重写的函数
// 	UFUNCTION(BlueprintImplementableEvent, Category = "Interpreter|SyncAction")
// 	void OnExecute();
//
// 	
};
