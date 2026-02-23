// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/EventSequenceComponent.h"
#include "DataAssets/EventSequenceDA.h"
#include "Subsystems/WorldSubsystem.h"
#include "UObject/Object.h"
#include "Engine/DataAsset.h"
#include "Events/EventSequenceAsyncBlueprintAction.h"
#include "Events/EventSequenceRunning.h"
#include "Events/SequenceEvent_BlueprintCall.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "EventSequenceSystem.generated.h"


// 异步操作信息
USTRUCT(BlueprintType)
struct FAsyncOperationInfo
{
    GENERATED_BODY()
    
    FGuid SequenceID;
    int32 EventIndex = -1;
    TWeakObjectPtr<UEventSequenceAsyncBlueprintAction> AsyncAction;
    FGuid AsyncActionID;
    float StartTime = 0.0f;
    
    // 序列化状态
    FString SerializedState;
    
    // 是否从中断恢复
    bool bRecoveringFromInterrupt = false;
};



class UPropertyBagWrapper;

UCLASS()
class EVENTSEQUENCER_API UEventSequenceSystem : public UTickableWorldSubsystem
{
    GENERATED_BODY()
	
    UPROPERTY()
    TArray<UEventSequenceRunning*> EventSequencesRunning;

    // 序列实例映射
    UPROPERTY()
    TMap<FGuid, UEventSequenceRunning*> SequenceInstances;
    
    UPROPERTY()
    TArray<TWeakObjectPtr<UEventSequenceComponent>> EventSequenceComponents;

    void ParseEventSequence(UEventSequenceRunning* EventSequenceRunning, const TArray<FInstancedStruct>& EventSequence);
    
public:
    static UEventSequenceSystem* GetInstance(UWorld* World);
    
    virtual void Tick(float DeltaTime) override;
    
    virtual TStatId GetStatId() const override;
    
    UFUNCTION()
    void TickSequences(float DeltaTime);

    // PropertyWrapper 开放给蓝图的话，需要在蓝图中创建并设置值，值多的话会占用不少面积
    UFUNCTION(BlueprintCallable, Category = "Event Sequencer")
    UEventSequenceRunning* CreateEventSequence(UEventSequenceDA* TargetDataAsset, UEventSequenceComponent* Component = nullptr, UPropertyBagWrapper* PropertyBagInput = nullptr);

    UFUNCTION(BlueprintCallable, Category = "Event Sequencer")
    bool RemoveEventSequence(UEventSequenceRunning* EventSequence);

    UFUNCTION()
    bool RemoveComponent(const UEventSequenceComponent* Component);


    // 异步操作管理
    UFUNCTION(BlueprintCallable, Category = "Event Sequence|Async")
    FGuid StartAsyncOperation(UEventSequenceRunning* Instance, int32 EventIndex, 
                             UEventSequenceAsyncBlueprintAction* AsyncAction);
    
    UFUNCTION(BlueprintCallable, Category = "Event Sequence|Async")
    bool CancelAsyncOperation(const FGuid& AsyncOperationID, const FString& Reason = TEXT("Cancelled"));
    
    UFUNCTION(BlueprintCallable, Category = "Event Sequence|Async")
    bool HasPendingAsyncOperation(const FGuid& SequenceID) const;
    
    UFUNCTION(BlueprintCallable, Category = "Event Sequence|Async")
    TArray<FGuid> GetPendingAsyncOperations(const FGuid& SequenceID) const;
    
    // 异步回调
    void OnAsyncActionCompleted(const FGuid& AsyncActionID);
    void OnAsyncActionFailed(const FString& Reason, const FGuid& AsyncActionID);
    
    // 序列化支持
    UFUNCTION(BlueprintCallable, Category = "Event Sequence|Serialization")
    void SerializeAsyncOperations(const FGuid& SequenceID, TArray<FString>& OutSerializedStates);
    
    UFUNCTION(BlueprintCallable, Category = "Event Sequence|Serialization")
    void DeserializeAsyncOperations(const FGuid& SequenceID, const TArray<FString>& SerializedStates);
    
    // 全局属性包
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Event Sequence")
    UPropertyBagWrapper* GlobalPropertyBag = nullptr;
    
    // 事件总线
    DECLARE_MULTICAST_DELEGATE_ThreeParams(FOnAsyncOperationStarted, const FGuid&, const FGuid&, UEventSequenceAsyncBlueprintAction*);
    FOnAsyncOperationStarted OnAsyncOperationStarted;
    
    DECLARE_MULTICAST_DELEGATE_ThreeParams(FOnAsyncOperationCompleted, const FGuid&, const FGuid&, EAsyncActionResult);
    FOnAsyncOperationCompleted OnAsyncOperationCompleted;
    
    // 获取异步操作信息
    const FAsyncOperationInfo* GetAsyncOperationInfo(const FGuid& AsyncOperationID) const;

    UFUNCTION()
    UEventSequenceRunning* GetAsyncEventSequence(const FGuid& Guid);
    
private:

    UPROPERTY()
    TMap<FGuid, UEventSequenceRunning*> AsyncEventSequenceMap;
    
    // 异步操作映射
    TMap<FGuid, FAsyncOperationInfo> AsyncOperations;
    
    // 序列到异步操作的映射
    TMap<FGuid, TArray<FGuid>> SequenceToAsyncOperations;
    
    // 帧时间跟踪
    float FrameTimeAccumulator = 0.0f;
    
    // 异步操作ID计数器
    uint64 AsyncOperationIDCounter = 0;
    
    // 内部函数
    FGuid GenerateAsyncOperationID();
    void CleanupCompletedAsyncOperations();
    void ProcessAsyncOperations(float DeltaTime);

    
    
    // 处理中断恢复
    void HandleInterruptRecovery(UEventSequenceRunning* Instance, FSequenceEvent_AsyncBlueprintCall& AsyncEvent);


    template<typename TEvent>
    bool CopySequenceEventProperty(const FInstancedStruct& SourceEventStruct, FInstancedStruct& RuntimeEventStruct)
    {
        // 1. 尝试获取源结构体的const指针
        if (const TEvent* SourceEvent = SourceEventStruct.GetPtr<TEvent>())
        {
            // 2. 尝试获取目标结构体的可变指针
            if (TEvent* DestEvent = RuntimeEventStruct.GetMutablePtr<TEvent>())
            {
                // 3. 拷贝Property（核心逻辑）
                DestEvent->Property = SourceEvent->Property;
                return true; // 拷贝成功
            }
        }
        return false; // 任一指针为空，拷贝失败
    }
};
