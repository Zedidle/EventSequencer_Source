#pragma once

#pragma once

#include "CoreMinimal.h"
#include "StructUtils/PropertyBag.h"
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
    
	// 解释器中的属性名
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Port", meta = (DisplayName = "Interpreter Property"))
	FName PropertyName;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Port")
	EPropertyBagPropertyType PropertyType;

	// 端口方向
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Port")
	EPortDirection Direction = EPortDirection::Input;
    
	// 当 EPortDirection 为 Output 时生效
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Port")
	FString OutputValue;

};