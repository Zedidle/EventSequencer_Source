#pragma once
#include "CoreMinimal.h"
#include "CommonStructs.h"
#include "StructUtils/PropertyBag.h"
#include "SequenceEvent_BREAK.generated.h"

USTRUCT(BlueprintType)
struct FSequenceEvent_BREAK : public FBaseSequenceEvent
{
	GENERATED_BODY()

	FSequenceEvent_BREAK(){ Type = EEventType::BREAK; }
	
	// 循环退出的条件
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Loop")
	FSequenceCondition Condition;
	
};