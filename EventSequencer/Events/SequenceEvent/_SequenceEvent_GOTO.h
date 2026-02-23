#pragma once
#include "CoreMinimal.h"
#include "CommonStructs.h"
#include "StructUtils/PropertyBag.h"
#include "_SequenceEvent_GOTO.generated.h"

USTRUCT(BlueprintType, meta = (DisplayName = "GOTO"))
struct F_SequenceEvent_GOTO : public FBaseSequenceEvent
{
	GENERATED_BODY()

	F_SequenceEvent_GOTO(){ }
	F_SequenceEvent_GOTO(int _TargetIndex){ TargetIndex = _TargetIndex; }
	virtual FString GetDisplayName() const override
	{
		FString TargetIndexString = FString::Printf(TEXT("%03d"), TargetIndex);
		if (TargetLabel.IsNone())
		{
			return "GOTO " + TargetIndexString;
		}

		return "GOTO " + TargetIndexString + " [Label " + TargetLabel.ToString() + "]";
		
	}
	virtual int GetEventsCount() override
	{
		return Super::GetEventsCount();
	}

	int TargetIndex = -1;
	
	// 目标标签
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Goto")
	FName TargetLabel;
    
	// 跳转条件（可选，为空表示无条件跳转）
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Condition")
	FSequenceCondition Condition;

};
