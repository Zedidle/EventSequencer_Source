#pragma once

#pragma once

#include "CoreMinimal.h"
#include "EventSequenceBlueprintAction.h"
#include "EventSequencePorts.h"
#include "SequenceEvent/CommonStructs.h"
#include "StructUtils/PropertyBag.h"
#include "SequenceEvent_BlueprintCall.generated.h"


/**
 * 蓝图调用事件
 * 用于在事件序列中调用蓝图定义的同步行为
 */
USTRUCT(BlueprintType, meta = (DisplayName = "Blueprint Call"))
struct FSequenceEvent_BlueprintCall : public FBaseSequenceEvent
{
    GENERATED_BODY()
    
public:
    // 蓝图类引用
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Blueprint", 
              meta = (DisplayName = "Blueprint Class", 
                      AllowedClasses = "/Script/EventSequencer.EventSequenceBlueprintAction"))
    TSubclassOf<UEventSequenceBlueprintAction> BlueprintClass;
    
    // 端口绑定配置
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ports", 
              meta = (DisplayName = "Port Bindings", TitleProperty = "BlueprintPropertyName"))
    TArray<FPortBinding> PortBindings;
    
    // 蓝图实例（运行时使用）
    UPROPERTY(Transient)
    UEventSequenceBlueprintAction* BlueprintInstance = nullptr;
    
    // 执行结果
    EBlueprintActionResult LastResult = EBlueprintActionResult::Success;
    
    // 重置状态
    // virtual void Reset() override
    // {
    //     FBaseSequenceEvent::Reset();
    //     
    //     // 清理蓝图实例
    //     if (BlueprintInstance && BlueprintInstance->bDestroyAfterExecution)
    //     {
    //         BlueprintInstance->MarkAsGarbage();
    //     }
    //     BlueprintInstance = nullptr;
    //     LastResult = EBlueprintActionResult::Success;
    // }
    
    // 获取蓝图实例（如果不存在则创建）
    UEventSequenceBlueprintAction* GetOrCreateBlueprintInstance(UObject* Outer);
    
    // 同步端口数据
    bool SyncPortData(UEventSequenceBlueprintAction* Instance, 
                      FInstancedPropertyBag& PropertyBag,
                      bool bToBlueprint);
    
    // 执行蓝图
    bool ExecuteBlueprint(UEventSequenceBlueprintAction* Instance, 
                          const FSequenceBlueprintContext& Context,
                          FInstancedPropertyBag& PropertyBag);
    
    // 验证端口绑定
    bool ValidatePortBindings(const UClass* _BlueprintClass) const;
    
    // 获取端口信息
    void GetPortInfo(const UClass* _BlueprintClass, 
                     TArray<FPortBinding>& InputPorts, 
                     TArray<FPortBinding>& OutputPorts) const;
};