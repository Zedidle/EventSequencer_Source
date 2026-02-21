#pragma once

#pragma once

#include "CoreMinimal.h"
#include "EventSequencePorts.generated.h"

// 端口方向
UENUM(BlueprintType)
enum class EPortDirection : uint8
{
	Input,      // 输入端口
	Output,     // 输出端口
	InOut       // 双向端口
};

// 端口绑定
USTRUCT(BlueprintType)
struct FPortBinding
{
	GENERATED_BODY()
    
	// 蓝图中的属性名
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Port", meta = (DisplayName = "Blueprint Property"))
	FName BlueprintPropertyName;
    
	// 解释器中的属性名
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Port", meta = (DisplayName = "Interpreter Property"))
	FName InterpreterPropertyName;
    
	// 端口方向
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Port")
	EPortDirection Direction = EPortDirection::Input;
    
	// 手动设置的常量值（当不使用属性绑定时）
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Port", meta = (EditCondition = "bUseConstant"))
	FString ConstantValue;
    
	// 是否使用常量值
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Port", meta = (DisplayName = "Use Constant"))
	bool bUseConstant = false;
    
	// 构造函数
	FPortBinding() = default;
    
	FPortBinding(FName InBlueprintProperty, FName InInterpreterProperty, EPortDirection InDirection = EPortDirection::Input)
		: BlueprintPropertyName(InBlueprintProperty)
		, InterpreterPropertyName(InInterpreterProperty)
		, Direction(InDirection)
	{}
    
	// 获取显示名称
	FString GetDisplayName() const
	{
		if (bUseConstant)
		{
			return FString::Printf(TEXT("%s = %s"), 
				*BlueprintPropertyName.ToString(), 
				*ConstantValue);
		}
		else
		{
			return FString::Printf(TEXT("%s -> %s"), 
				*BlueprintPropertyName.ToString(), 
				*InterpreterPropertyName.ToString());
		}
	}
};