#pragma once
#include "CoreMinimal.h"
#include "CommonStructs.h"
#include "StructUtils/PropertyBag.h"
#include "_SequenceEvent_LABEL.generated.h"

USTRUCT(BlueprintType, meta = (DisplayName = "LABEL"))
struct F_SequenceEvent_LABEL : public FBaseSequenceEvent
{
	GENERATED_BODY()
	
	F_SequenceEvent_LABEL(){}
	virtual FString GetDisplayName() const override
	{
		return "LABEL [" + LabelName.ToString() + "]";
	}

	
	// 标签名称
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Label")
	FName LabelName;
    
	// 标签描述
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Label")
	FString LabelDescription;

};