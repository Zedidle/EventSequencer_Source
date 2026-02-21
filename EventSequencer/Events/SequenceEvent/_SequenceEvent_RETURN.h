#pragma once
#include "CoreMinimal.h"
#include "CommonStructs.h"
#include "StructUtils/PropertyBag.h"
#include "_SequenceEvent_RETURN.generated.h"

USTRUCT(BlueprintType, meta = (DisplayName = "RETURN"))
struct F_SequenceEvent_RETURN : public FBaseSequenceEvent
{
	GENERATED_BODY()

	F_SequenceEvent_RETURN(){ Type = EEventType::RETURN; }
	
	// 返回值（可选）
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Return")
	FString ReturnValue;

	// 条件定义
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Condition")
	FSequenceCondition Condition;
};