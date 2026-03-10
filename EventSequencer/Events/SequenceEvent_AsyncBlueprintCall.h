#pragma once

#include "CoreMinimal.h"
#include "StructUtils/InstancedStruct.h"
#include "StructUtils/PropertyBag.h"
#include "EventSequenceAsyncBlueprintAction.h"
#include "EventSequencePorts.h"
#include "EventSequencer/DataAssets/PropertyBagWrapper.h"
#include "SequenceEvent/CommonStructs.h"
#include "SequenceEvent_AsyncBlueprintCall.generated.h"

class UEventSequenceRunning;
/**
 * 异步蓝图调用事件
 * 允许在事件序列中执行自定义的异步蓝图行为
 */
USTRUCT(BlueprintType, meta = (DisplayName = "Async Blueprint Call"))
struct FSequenceEvent_AsyncBlueprintCall : public FBaseSequenceEvent
{
	GENERATED_BODY()
	
	bool bAsyncInstanceCreated = false;
	
	// 触发Catch的跳转下标
	int CatchStartIndex = -1;
	// 结束下标，包括平铺 CatchEvents 之后的位置
	int EndIndex = -1;
	
	virtual FString GetDisplayName() const override;
	
	virtual int GetEventsCount() override;
	
	UPROPERTY()
	TWeakObjectPtr<UEventSequenceRunning> EventSequenceRunning;
	void SetEventSequenceRunning(UEventSequenceRunning* EventSequenceInstance);

	// 蓝图类引用
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Blueprint", 
              meta = (DisplayName = "Blueprint Class", 
                      AllowedClasses = "/Script/EventSequencer.EventSequenceAsyncBlueprintAction"))
    TSubclassOf<UEventSequenceAsyncBlueprintAction> BlueprintClass;
    
    // 端口绑定配置
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Port Bindings",
    	meta = (TitleProperty = "{PropertyName} = {TargetValue}"))
    TArray<FPortBinding> OutPropertyValues;
    
    // 中断语义
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Async", meta = (DisplayName = "Interrupt Behavior"))
    EAsyncInterruptBehavior InterruptBehavior = EAsyncInterruptBehavior::Restart;
    
    // Catch块事件序列
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Catch", 
              meta = (BaseStruct = "/Script/EventSequencer.BaseSequenceEvent"))
    TArray<FEventWrapper> CatchEvents;
    
    // 蓝图实例（运行时创建，不序列化）
    UPROPERTY(Transient)
    UEventSequenceAsyncBlueprintAction* ActionInstance = nullptr;
    
    // 异步操作结果
    EAsyncActionResult AsyncResult = EAsyncActionResult::Pending;
    
    // 失败原因
    FString FailureReason;
    
    // 序列化状态
    FString SerializedState;
    
    // 是否正在执行Catch块
    bool bExecutingCatch = false;

	virtual bool Execute(int Index = 0) override;

	
    virtual void Reset() override
    {
        FBaseSequenceEvent::Reset();
    }
    
    // 创建异步实例
    UEventSequenceAsyncBlueprintAction* CreateAsyncInstance(UEventSequenceRunning* Outer) const;
    
    // 销毁异步实例
    void DestroyAsyncInstance();
    
    // 同步端口数据
    // bool SyncPortData(UEventSequenceAsyncBlueprintAction* Instance, 
    //                   UPropertyBagWrapper* PropertyBag,
    //                   bool bToBlueprint) const;
    
    // 处理异步完成
    // void HandleAsyncComplete(EAsyncActionResult Result, const FString& Reason = TEXT(""));
    
    // 验证端口绑定
    // bool ValidatePortBindings(const UClass* BlueprintClass) const;
    
    // 获取异步状态
    bool IsAsyncCompleted() const { return AsyncResult != EAsyncActionResult::Pending; }
    bool IsAsyncSuccessful() const { return AsyncResult == EAsyncActionResult::Success; }
	
	bool OnExecute();

};