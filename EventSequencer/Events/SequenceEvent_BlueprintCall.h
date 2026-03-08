#pragma once

#pragma once

#include "CoreMinimal.h"
#include "EventSequenceBlueprintAction.h"
#include "EventSequencePorts.h"
#include "EventSequencer/FuncLibs/EventSequenceFuncLib.h"
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

	virtual FString GetDisplayName() const override;
	
	UPROPERTY()
	TWeakObjectPtr<UEventSequenceRunning> EventSequenceRunning;
	void SetEventSequenceRunning(UEventSequenceRunning* EventSequenceInstance);
	
    // 蓝图类引用
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Blueprint", 
              meta = (DisplayName = "Blueprint Class", 
                      AllowedClasses = "/Script/EventSequencer.EventSequenceBlueprintAction"))
    TSubclassOf<UEventSequenceBlueprintAction> BlueprintClass;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ports", 
              meta = (DisplayName = "Port Bindings", TitleProperty = "{PropertyName} = {TargetValue}"))
    TArray<FPortBinding> OutPropertyValues;
    
    // 蓝图实例（运行时使用）
    UPROPERTY(Transient)
    UEventSequenceBlueprintAction* ActionInstance = nullptr;
    
    // 执行结果
    EBlueprintActionResult LastResult = EBlueprintActionResult::Success;
    
    // 重置状态
    virtual void Reset() override
    {
        FBaseSequenceEvent::Reset();
    }

	virtual float Tick(float DeltaTime, float PreRemainTime = 0) override;
	virtual bool Execute(int Index = 0) override;
	
    // 获取蓝图实例（如果不存在则创建）
    UEventSequenceBlueprintAction* GetOrCreateBlueprintInstance(UObject* Outer);

};