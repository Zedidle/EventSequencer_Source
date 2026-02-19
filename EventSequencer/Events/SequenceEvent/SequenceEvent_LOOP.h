#pragma once
#include "CoreMinimal.h"
#include "CommonStructs.h"
#include "StructUtils/PropertyBag.h"
#include "SequenceEvent_LOOP.generated.h"

USTRUCT(BlueprintType)
struct FSequenceEvent_LOOP: public FBaseSequenceEvent
{
	GENERATED_BODY()
    
	FSequenceEvent_LOOP(){ Type = EEventType::IF; }
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Loop Body",
			  meta = (BaseStruct = "/Script/EventSequencer.BaseSequenceEvent"))
	TArray<FInstancedStruct> LoopEvents;
    
	// 最大循环次数（0表示无限循环）
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Loop", meta = (ClampMin = "0"))
	int32 MaxIterations = 0;
    
	// 循环条件（可选，为空表示无条件循环）
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Loop")
	FSequenceCondition LoopCondition;
    
	// 当前迭代计数（运行时）
	int32 CurrentIteration = 0;
    
	// 检查是否应该继续循环
	bool ShouldContinue(const FInstancedPropertyBag& PropertyBag) const;
	
};
