#pragma once
#include "CoreMinimal.h"
#include "CommonStructs.h"
#include "StructUtils/PropertyBag.h"
#include "SequenceEvent_GOTO.generated.h"

USTRUCT(BlueprintType)
struct FSequenceEvent_GOTO : public FBaseSequenceEvent
{
	GENERATED_BODY()

	FSequenceEvent_GOTO(){ Type = EEventType::GOTO; }
	
	// 目标标签
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Goto")
	FName TargetLabel;
    
	// 跳转条件（可选，为空表示无条件跳转）
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Condition")
	FSequenceCondition Condition;
    
	// 是否应该跳转,传入 EventSequenceRunning 记录的 PropertyBagRuntime 
	bool CanGoto(const FInstancedPropertyBag& PropertyBag) const;

};
