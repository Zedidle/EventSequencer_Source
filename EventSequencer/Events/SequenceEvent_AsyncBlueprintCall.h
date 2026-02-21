#pragma once

#include "CoreMinimal.h"
#include "StructUtils/InstancedStruct.h"
#include "StructUtils/PropertyBag.h"
#include "EventSequenceAsyncBlueprintAction.h"
#include "EventSequencePorts.h"
#include "EventSequencer/DataAssets/PropertyBagWrapper.h"
#include "SequenceEvent/CommonStructs.h"
#include "SequenceEvent_AsyncBlueprintCall.generated.h"

/**
 * 异步蓝图调用事件
 * 允许在事件序列中执行自定义的异步蓝图行为
 */
USTRUCT(BlueprintType, meta = (DisplayName = "Async Blueprint Call"))
struct FSequenceEvent_AsyncBlueprintCall : public FBaseSequenceEvent
{
	GENERATED_BODY()


	// 蓝图类引用
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Blueprint", 
              meta = (DisplayName = "Blueprint Class", 
                      AllowedClasses = "/Script/EventSequencer.EventSequenceAsyncBlueprintAction"))
    TSubclassOf<UEventSequenceAsyncBlueprintAction> BlueprintClass;
    
    // 端口绑定配置
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Port Bindings", 
              meta = (TitleProperty = "BlueprintPropertyName"))
    TArray<FPortBinding> PortBindings;
    
    // 中断语义
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Async", meta = (DisplayName = "Interrupt Behavior"))
    EAsyncInterruptBehavior InterruptBehavior = EAsyncInterruptBehavior::Restart;
    
    // Catch块事件序列
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Catch", 
              meta = (BaseStruct = "/Script/EventSequencer.BaseSequenceEvent"))
    TArray<FInstancedStruct> CatchEvents;
    
    // 蓝图实例（运行时创建，不序列化）
    UPROPERTY(Transient)
    UEventSequenceAsyncBlueprintAction* AsyncInstance = nullptr;
    
    // 异步操作结果
    EAsyncActionResult AsyncResult = EAsyncActionResult::Pending;
    
    // 失败原因
    FString FailureReason;
    
    // 序列化状态
    FString SerializedState;
    
    // 是否正在执行Catch块
    bool bExecutingCatch = false;
    
    // Catch块开始索引
    int32 CatchStartIndex = -1;
    
    // 重置状态
    // virtual void Reset() override
    // {
    //     FBaseSequenceEvent::Reset();
    //     
    //     // 清理异步实例
    //     if (AsyncInstance)
    //     {
    //         AsyncInstance->OnCompleted.RemoveAll(this);
    //         AsyncInstance->OnFailed.RemoveAll(this);
    //         
    //         if (InterruptBehavior == EAsyncInterruptBehavior::Skip)
    //         {
    //             AsyncInstance->ConditionalBeginDestroy();
    //         }
    //     }
    //     
    //     AsyncInstance = nullptr;
    //     AsyncResult = EAsyncActionResult::Pending;
    //     FailureReason.Empty();
    //     bExecutingCatch = false;
    //     CatchStartIndex = -1;
    // }
    
    // 创建异步实例
    UEventSequenceAsyncBlueprintAction* CreateAsyncInstance(UObject* Outer) const;
    
    // 销毁异步实例
    void DestroyAsyncInstance();
    
    // 开始异步执行
    bool StartAsyncExecution(UEventSequenceAsyncBlueprintAction* Instance, 
                             const FAsyncActionContext& Context,
                             UPropertyBagWrapper* PropertyBag);
    
    // 同步端口数据
    bool SyncPortData(UEventSequenceAsyncBlueprintAction* Instance, 
                      UPropertyBagWrapper* PropertyBag,
                      bool bToBlueprint) const;
    
    // 处理异步完成
    void HandleAsyncComplete(EAsyncActionResult Result, const FString& Reason = TEXT(""));
    
    // 验证端口绑定
    bool ValidatePortBindings(const UClass* BlueprintClass) const;
    
    // 获取异步状态
    bool IsAsyncCompleted() const { return AsyncResult != EAsyncActionResult::Pending; }
    bool IsAsyncSuccessful() const { return AsyncResult == EAsyncActionResult::Success; }
    
    // 序列化当前状态
    FString SerializeAsyncState() const;
    
    // 反序列化状态
    void DeserializeAsyncState(const FString& StateString);
	

};