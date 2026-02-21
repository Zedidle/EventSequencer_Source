// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EventSequenceAsyncBlueprintAction.h"
#include "SequenceEvent_AsyncBlueprintCall.h"
#include "SequenceEvent_BlueprintCall.h"
#include "EventSequencer/DataAssets/EventSequenceDA.h"
#include "SequenceEvent/CommonStructs.h"
#include "SequenceEvent/_SequenceEvent_LOOP.h"
#include "StructUtils/InstancedStruct.h"
#include "UObject/Object.h"
#include "EventSequenceRunning.generated.h"


// 序列状态
UENUM(BlueprintType)
enum class ESequenceState : uint8
{
	Idle,               // 空闲
	Running,            // 运行中
	WaitingForAsync,    // 等待异步操作
	Paused,             // 已暂停
	Completed,          // 已完成
	Failed              // 失败
};


UCLASS(BlueprintType)
class EVENTSEQUENCER_API UEventSequenceRunning : public UObject
{
	GENERATED_BODY()

	bool EvaluateCondition(const FSequenceCondition& Condition);
	bool CompareBool(bool Left, bool Right, ESequenceConditionOperator Op);
	bool CompareInt32(int32 Left, int32 Right, ESequenceConditionOperator Op);
	bool CompareFloat(const float& Left, const float& Right, ESequenceConditionOperator Op);
	bool CompareDouble(const double& Left, const double& Right, ESequenceConditionOperator Op);
	bool CompareString(const FString& Left, const FString& Right, ESequenceConditionOperator Op);
	bool CompareName(const FName& Left, const FName& Right, ESequenceConditionOperator Op);
	bool CompareText(const FText& Left, const FText& Right, ESequenceConditionOperator Op);

	bool TryParseBool(const FString& StringValue, bool& OutValue);
	bool TryParseInt32(const FString& StringValue, int32& OutValue);
	bool TryParseFloat(const FString& StringValue, float& OutValue);
	bool TryParseDouble(const FString& StringValue, double& OutValue);
	bool TryParseString(const FString& StringValue, FString& OutValue);
	bool TryParseName(const FString& StringValue, FName& OutValue);
	bool TryParseText(const FString& StringValue, FText& OutValue);
	
public:
	// 可能某些Event标记有Label，则需要记录下来方便跳转
	TMap<FName, int> Label2Index;
	
	bool bPause = false;
	int CurEventIndex = 0;
	TArray<FInstancedStruct> EventQueue;

	int GetEventsNum() { return EventQueue.Num(); }
	
	// 当前循环状态栈
	TArray<FEventState_LOOP> LoopStateStack;
	
	// 初始化定义的DA，存储了 InstancedPropertyBag 数据
	UPROPERTY()
	UEventSequenceDA* InitDataAsset;
	FInstancedPropertyBag PropertyBagRuntime;
	
	UFUNCTION()
	void SetDataAsset(UEventSequenceDA* DataAsset);

	void Tick(float DeltaTime);
	
	void AddEvent(FInstancedStruct& Event);
	void AppendEvents(TArray<FInstancedStruct>& Events);
	
	bool IsCompleted() const;
    
	FInstancedStruct& GetCurEvent();

	UFUNCTION(BlueprintCallable, Category = "Event Sequence")
	void AddLabel(FName Label);
	
	UFUNCTION(BlueprintCallable, Category = "Event Sequence")
	void GOTO(FName Label);
	void GOTO(int Index);
	
	void LOOP(const FEventState_LOOP& LoopState);
	void BREAK();
	
	UFUNCTION(BlueprintCallable, Category = "Event Sequence")
	void Start();
	
	// 暂停执行
	UFUNCTION(BlueprintCallable, Category = "Event Sequence")
	bool Pause();
    
	// 恢复执行
	UFUNCTION(BlueprintCallable, Category = "Event Sequence")
	bool Resume();
	
	UFUNCTION(BlueprintCallable, Category = "Event Sequence")
	void Stop();

	// 执行单步
	UFUNCTION(BlueprintCallable, Category = "Event Sequence")
	bool ExecuteStep();
	
	UFUNCTION(BlueprintCallable, Category = "Event Sequence")
	void Next();

	UFUNCTION(BlueprintCallable, Category = "Event Sequence")
	void Exit();
	
	void Destroy();

	// 获取序列状态
	UFUNCTION(BlueprintCallable, Category = "Event Sequence")
	ESequenceState GetState() const { return State; }
 
    // 获取序列ID
    FGuid GetSequenceID() const { return SequenceID; }
    
    // 获取属性包
    UFUNCTION(BlueprintCallable, Category = "Event Sequence")
    UPropertyBagWrapper* GetPropertyBagWrapper() const;
    
    // 获取当前事件索引
    UFUNCTION(BlueprintCallable, Category = "Event Sequence")
    int32 GetCurrentEventIndex() const { return CurrentEventIndex; }
    
    // 获取事件数量
    UFUNCTION(BlueprintCallable, Category = "Event Sequence")
    int32 GetEventCount() const { return EventSequenceRuntime.Num(); }
    
    // 检查是否在等待异步操作
    bool IsWaitingForAsync() const;
    
    // 异步操作完成回调
    void OnAsyncOperationCompleted(int32 EventIndex, EAsyncActionResult Result, const FString& Reason = TEXT(""));
    
    // 获取所有标签
    UFUNCTION(BlueprintCallable, Category = "Event Sequence")
    TArray<FName> GetAllLabels() const;
    
    // 查找标签索引
    UFUNCTION(BlueprintCallable, Category = "Event Sequence")
    int32 FindLabelIndex(const FName& LabelName) const;
    
    // 获取执行统计
    UFUNCTION(BlueprintCallable, Category = "Event Sequence")
    int32 GetExecutedInstructionCount() const { return ExecutedInstructionCount; }
    
    // 获取序列数据资产
    UEventSequenceDA* GetSequenceDA() const { return SequenceDA; }
    
    // 事件执行委托
    DECLARE_MULTICAST_DELEGATE_TwoParams(FOnEventExecuted, UEventSequenceRunning*, const FInstancedStruct&);
    FOnEventExecuted OnEventExecuted;
    
    // 状态变化委托
    DECLARE_MULTICAST_DELEGATE_TwoParams(FOnStateChanged, UEventSequenceRunning*, ESequenceState);
    FOnStateChanged OnStateChanged;
    
protected:
    // 序列ID
    FGuid SequenceID;
    
    // 序列数据资产
    UPROPERTY()
    UEventSequenceDA* SequenceDA = nullptr;
    
    // 运行时属性包
    // UPROPERTY()
    // UPropertyBagWrapper* PropertyBagRuntime = nullptr;
    
    // 运行时事件序列
    TArray<FInstancedStruct> EventSequenceRuntime;
    
    // 当前事件索引
    int32 CurrentEventIndex = -1;
    
    // 执行状态
    ESequenceState State = ESequenceState::Idle;
    
    // 等待中的异步事件索引
    int32 WaitingForAsyncEventIndex = -1;
    
    // 等待中的异步操作ID
    FGuid WaitingForAsyncOperationID;
    
    // 标签映射
    TMap<FName, int32> LabelIndexMap;
    
    // 已执行指令计数
    int32 ExecutedInstructionCount = 0;
    
    // 每帧执行指令限制
    int32 InstructionLimitPerFrame = 1000;
    
    // 异步实例缓存
    TMap<FName, UEventSequenceAsyncBlueprintAction*> AsyncInstanceCache;
    
    // 执行事件
    bool ExecuteEvent(FInstancedStruct& Event, int32 EventIndex);
    
    // 执行异步蓝图调用事件
    bool ExecuteAsyncBlueprintCallEvent(FSequenceEvent_AsyncBlueprintCall& AsyncEvent, int32 EventIndex);
    
    // 执行Catch块
    bool ExecuteCatchBlock(FSequenceEvent_AsyncBlueprintCall& AsyncEvent, int32 EventIndex);
    
    // 开始等待异步操作
    void StartWaitingForAsync(int32 EventIndex, const FGuid& AsyncOperationID);
    
    // 结束等待异步操作
    void StopWaitingForAsync();
    
    // 同步端口数据
    bool SyncPortData(UEventSequenceAsyncBlueprintAction* Instance, 
                      const TArray<FPortBinding>& PortBindings,
                      bool bToBlueprint);
    
    
    // 处理控制流事件
    bool HandleControlFlowEvent(FInstancedStruct& Event, int32 EventIndex);
    
    // 处理具体操作事件
    bool HandleActionEvent(FInstancedStruct& Event, int32 EventIndex);
    
    // 设置状态
    void SetState(ESequenceState NewState);

	bool ExecuteBlueprintCallEvent(FSequenceEvent_BlueprintCall& BlueprintCallEvent, int32 EventIndex);
	
};
