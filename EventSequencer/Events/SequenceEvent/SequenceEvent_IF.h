#pragma once
#include "CoreMinimal.h"
#include "CommonStructs.h"
#include "StructUtils/PropertyBag.h"
#include "SequenceEvent_IF.generated.h"


USTRUCT(BlueprintType)
struct FSequenceEvent_IF : public FBaseSequenceEvent
{
	GENERATED_BODY()
    
	FSequenceEvent_IF(){ Type = EEventType::IF; }

	// True事件开始下标
	int TrueEventsStartIndex = 0;
	// False事件开始下标
	int FalseEventsStartIndex = 0;
	// 条件分支结束下标
	int EndIndex = 0;
	
	// 条件定义
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Condition")
	FSequenceCondition Condition;
    
	// 条件为真时执行的事件序列
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Branches",
			  meta = (BaseStruct = "/Script/EventSequencer.BaseSequenceEvent"))
	TArray<FInstancedStruct> TrueEvents;
    
	// 条件为假时执行的事件序列
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Branches",
			  meta = (BaseStruct = "/Script/EventSequencer.BaseSequenceEvent"))
	TArray<FInstancedStruct> FalseEvents;
    
	// 评估条件
	bool EvaluateCondition(const FInstancedPropertyBag& PropertyBag) const;
};
