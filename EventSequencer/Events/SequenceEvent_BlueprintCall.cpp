#include "SequenceEvent_BlueprintCall.h"
#include "EventSequenceBlueprintAction.h"
#include "StructUtils/PropertyBag.h"

UEventSequenceBlueprintAction* FSequenceEvent_BlueprintCall::GetOrCreateBlueprintInstance(UObject* Outer)
{
    if (!BlueprintClass)
    {
        return nullptr;
    }
    
    // 如果已存在实例，返回现有实例
    if (BlueprintInstance && IsValid(BlueprintInstance))
    {
        return BlueprintInstance;
    }
    
    // 创建新实例
    BlueprintInstance = NewObject<UEventSequenceBlueprintAction>(Outer, BlueprintClass);
    if (!BlueprintInstance)
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to create blueprint instance for class: %s"), 
               *BlueprintClass->GetName());
    }
    
    return BlueprintInstance;
}

bool FSequenceEvent_BlueprintCall::SyncPortData(UEventSequenceBlueprintAction* Instance, 
                                               FInstancedPropertyBag& PropertyBag,
                                               bool bToBlueprint)
{
    if (!Instance || !IsValid(Instance))
    {
        return false;
    }
    
    bool bSuccess = true;
    
    for (const FPortBinding& Binding : PortBindings)
    {
        if (Binding.BlueprintPropertyName.IsNone())
        {
            continue;
        }
        
        // 根据方向决定同步方向
        bool bShouldSync = false;
        if (bToBlueprint && (Binding.Direction == EPortDirection::Input || 
                             Binding.Direction == EPortDirection::InOut))
        {
            bShouldSync = true;
        }
        else if (!bToBlueprint && (Binding.Direction == EPortDirection::Output || 
                                   Binding.Direction == EPortDirection::InOut))
        {
            bShouldSync = true;
        }
        
        if (!bShouldSync)
        {
            continue;
        }
        
        if (bToBlueprint)
        {
            // 从解释器属性包同步到蓝图实例
            if (Binding.bUseConstant)
            {
                // 使用常量值
                // 这里需要根据属性类型解析常量字符串
                // 简化处理：通过反射设置字符串值
                FProperty* Property = Instance->GetClass()->FindPropertyByName(Binding.BlueprintPropertyName);
                if (Property)
                {
                    if (FStrProperty* StrProperty = CastField<FStrProperty>(Property))
                    {
                        StrProperty->SetPropertyValue_InContainer(Instance, Binding.ConstantValue);
                    }
                    else if (FNameProperty* NameProperty = CastField<FNameProperty>(Property))
                    {
                        NameProperty->SetPropertyValue_InContainer(Instance, FName(*Binding.ConstantValue));
                    }
                    else if (FIntProperty* IntProperty = CastField<FIntProperty>(Property))
                    {
                        int32 Value = FCString::Atoi(*Binding.ConstantValue);
                        IntProperty->SetPropertyValue_InContainer(Instance, Value);
                    }
                    else if (FFloatProperty* FloatProperty = CastField<FFloatProperty>(Property))
                    {
                        float Value = FCString::Atof(*Binding.ConstantValue);
                        FloatProperty->SetPropertyValue_InContainer(Instance, Value);
                    }
                    else if (FBoolProperty* BoolProperty = CastField<FBoolProperty>(Property))
                    {
                        bool bValue = Binding.ConstantValue.ToBool();
                        BoolProperty->SetPropertyValue_InContainer(Instance, bValue);
                    }
                }
            }
            else
            {
                // 从属性包同步
                FProperty* Property = Instance->GetClass()->FindPropertyByName(Binding.BlueprintPropertyName);
                if (Property)
                {
                    // 根据属性类型从属性包获取值
                    if (FStrProperty* StrProperty = CastField<FStrProperty>(Property))
                    {
                        TValueOrError<FString, EPropertyBagResult> V = PropertyBag.GetValueString(Binding.InterpreterPropertyName);
                        if (V.GetError() == EPropertyBagResult::Success)
                        {
                            StrProperty->SetPropertyValue_InContainer(Instance, V.GetValue());
                        }
                    }
                    else if (FNameProperty* NameProperty = CastField<FNameProperty>(Property))
                    {
                        TValueOrError<FName, EPropertyBagResult> V = PropertyBag.GetValueName(Binding.InterpreterPropertyName);
                        if (V.GetError() == EPropertyBagResult::Success)
                        {
                            NameProperty->SetPropertyValue_InContainer(Instance, V.GetValue());
                        }
                    }
                    else if (FIntProperty* IntProperty = CastField<FIntProperty>(Property))
                    {
                        TValueOrError<int32, EPropertyBagResult> V = PropertyBag.GetValueInt32(Binding.InterpreterPropertyName);
                        if (V.GetError() == EPropertyBagResult::Success)
                        {
                            IntProperty->SetPropertyValue_InContainer(Instance, V.GetValue());
                        }
                    }
                    else if (FFloatProperty* FloatProperty = CastField<FFloatProperty>(Property))
                    {
                        TValueOrError<float, EPropertyBagResult> V = PropertyBag.GetValueFloat(Binding.InterpreterPropertyName);
                        if (V.GetError() == EPropertyBagResult::Success)
                        {
                            FloatProperty->SetPropertyValue_InContainer(Instance, V.GetValue());
                        }
                    }
                    else if (FBoolProperty* BoolProperty = CastField<FBoolProperty>(Property))
                    {
                        TValueOrError<bool, EPropertyBagResult> V = PropertyBag.GetValueBool(Binding.InterpreterPropertyName);
                        if (V.GetError() == EPropertyBagResult::Success)
                        {
                            BoolProperty->SetPropertyValue_InContainer(Instance, V.GetValue());
                        }
                    }
                }
            }
        }
        else
        {
            // 从蓝图实例同步到解释器属性包
            FProperty* Property = Instance->GetClass()->FindPropertyByName(Binding.BlueprintPropertyName);
            if (Property && !Binding.InterpreterPropertyName.IsNone())
            {
                // 根据属性类型设置到属性包
                if (FStrProperty* StrProperty = CastField<FStrProperty>(Property))
                {
                    FString Value = StrProperty->GetPropertyValue_InContainer(Instance);
                    PropertyBag.SetValueString(Binding.InterpreterPropertyName, Value);
                }
                else if (FNameProperty* NameProperty = CastField<FNameProperty>(Property))
                {
                    FName Value = NameProperty->GetPropertyValue_InContainer(Instance);
                    PropertyBag.SetValueName(Binding.InterpreterPropertyName, Value);
                }
                else if (FIntProperty* IntProperty = CastField<FIntProperty>(Property))
                {
                    int32 Value = IntProperty->GetPropertyValue_InContainer(Instance);
                    PropertyBag.SetValueInt32(Binding.InterpreterPropertyName, Value);
                }
                else if (FFloatProperty* FloatProperty = CastField<FFloatProperty>(Property))
                {
                    float Value = FloatProperty->GetPropertyValue_InContainer(Instance);
                    PropertyBag.SetValueFloat(Binding.InterpreterPropertyName, Value);
                }
                else if (FBoolProperty* BoolProperty = CastField<FBoolProperty>(Property))
                {
                    bool bValue = BoolProperty->GetPropertyValue_InContainer(Instance);
                    PropertyBag.SetValueBool(Binding.InterpreterPropertyName, bValue);
                }
            }
        }
    }
    
    return bSuccess;
}

bool FSequenceEvent_BlueprintCall::ExecuteBlueprint(UEventSequenceBlueprintAction* Instance, 
                                                   const FSequenceBlueprintContext& Context,
                                                   FInstancedPropertyBag& PropertyBag)
{
    if (!Instance || !IsValid(Instance))
    {
        return false;
    }
    
    // 1. 设置上下文
    Instance->SetContext(Context);
    
    // 2. 同步输入数据
    if (!SyncPortData(Instance, PropertyBag, true))
    {
        UE_LOG(LogTemp, Warning, TEXT("Failed to sync input data to blueprint"));
    }
    
    // 3. 执行蓝图
    LastResult = Instance->Execute(Context);
    
    // 4. 同步输出数据
    if (!SyncPortData(Instance, PropertyBag, false))
    {
        UE_LOG(LogTemp, Warning, TEXT("Failed to sync output data from blueprint"));
    }
    
    // 5. 清理一次性实例
    // if (Instance->bDestroyAfterExecution)
    // {
    //     Instance->MarkAsGarbage();
    //     BlueprintInstance = nullptr;
    // }
    
    return LastResult == EBlueprintActionResult::Success;
}

bool FSequenceEvent_BlueprintCall::ValidatePortBindings(const UClass* _BlueprintClass) const
{
    if (!_BlueprintClass)
    {
        return false;
    }
    
    for (const FPortBinding& Binding : PortBindings)
    {
        if (Binding.BlueprintPropertyName.IsNone())
        {
            continue;
        }
        
        // 检查蓝图类是否有这个属性
        FProperty* Property = _BlueprintClass->FindPropertyByName(Binding.BlueprintPropertyName);
        if (!Property)
        {
            UE_LOG(LogTemp, Warning, TEXT("Property %s not found in blueprint class %s"), 
                   *Binding.BlueprintPropertyName.ToString(), 
                   *_BlueprintClass->GetName());
            return false;
        }
        
        // 检查属性是否有正确的元数据标记
        // 这里可以添加更多验证
    }
    
    return true;
}

void FSequenceEvent_BlueprintCall::GetPortInfo(const UClass* _BlueprintClass, 
                                              TArray<FPortBinding>& InputPorts, 
                                              TArray<FPortBinding>& OutputPorts) const
{
    InputPorts.Empty();
    OutputPorts.Empty();
    
    if (!_BlueprintClass)
    {
        return;
    }
    
    // 收集蓝图类的所有属性
    for (TFieldIterator<FProperty> It(_BlueprintClass); It; ++It)
    {
        FProperty* Property = *It;
        FName PropertyName = Property->GetFName();
        
        // 检查属性是否有端口元数据
        FString PortDirection = Property->GetMetaData(TEXT("PortDirection"));
        
        if (!PortDirection.IsEmpty())
        {
            EPortDirection Direction = EPortDirection::Input;
            if (PortDirection.Equals(TEXT("Output"), ESearchCase::IgnoreCase))
            {
                Direction = EPortDirection::Output;
            }
            else if (PortDirection.Equals(TEXT("InOut"), ESearchCase::IgnoreCase))
            {
                Direction = EPortDirection::InOut;
            }
            
            FPortBinding Binding;
            Binding.BlueprintPropertyName = PropertyName;
            Binding.Direction = Direction;
            
            if (Direction == EPortDirection::Input || Direction == EPortDirection::InOut)
            {
                InputPorts.Add(Binding);
            }
            if (Direction == EPortDirection::Output || Direction == EPortDirection::InOut)
            {
                OutputPorts.Add(Binding);
            }
        }
    }
}