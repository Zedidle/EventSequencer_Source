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
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Loop Body", meta = (BaseStruct = "/Script/EventSequencer.BaseSequenceEvent"))
	TArray<FInstancedStruct> LoopEvents;
    
	// 最大循环次数（0表示无限循环）
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Loop", meta = (ClampMin = "0"))
	int32 MaxLoopTimes = 0;
    
	// 循环条件（可选，为空表示无条件循环）
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Loop")
	FSequenceCondition LoopCondition;
    
	// 当前迭代计数（运行时）
	int32 CurLoopTimes = 0;
	// 循环体开始位置（扁平化序列中）
	int32 LoopStartIndex = -1;
	// 循环结束位置（跳出后的位置）
	int32 LoopEndIndex = -1;    
	
	// 检查是否应该继续循环
	bool ShouldContinue(const FInstancedPropertyBag& PropertyBag) const;
	
};
