#pragma once
#include "CoreMinimal.h"
#include "CommonStructs.h"
#include "StructUtils/PropertyBag.h"
#include "_SequenceEvent_BREAK.generated.h"

USTRUCT(BlueprintType, meta = (DisplayName = "BREAK"))
struct F_SequenceEvent_BREAK : public FBaseSequenceEvent
{
	GENERATED_BODY()

	F_SequenceEvent_BREAK(){ Type = EEventType::BREAK; }
	
	// 循环退出的条件
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Loop")
	FSequenceCondition Condition;
	
};