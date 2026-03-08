#include "SequenceEvent_BlueprintCall.h"
#include "EventSequenceBlueprintAction.h"
#include "StructUtils/PropertyBag.h"

FString FSequenceEvent_BlueprintCall::GetDisplayName() const
{
    return "BP-CALL [" + BlueprintClass->GetName() + "]";
}

float FSequenceEvent_BlueprintCall::Tick(float DeltaTime, float PreRemainTime)
{
    return FBaseSequenceEvent::Tick(DeltaTime, PreRemainTime);
}

bool FSequenceEvent_BlueprintCall::Execute(int Index)
{
    if (!BlueprintClass || !EventSequenceRunning.IsValid())
    {
        return false;
    }
    
    if (!BlueprintInstance)
    {
        BlueprintInstance = NewObject<UEventSequenceBlueprintAction>(EventSequenceRunning.Get(), BlueprintClass);
    }

    if (BlueprintInstance)
    {
	    UPropertyBagWrapper* Wrapper = EventSequenceRunning->GetPropertyBagWrapper();
        BlueprintInstance->Execute(Wrapper);
        
        for (FPortBinding& Bind : OutPropertyValues)
        {
            Wrapper->AddValueFromString(Bind.PropertyName, Bind.PropertyType, Bind.TargetValue);
        }

        EventSequenceRunning->PropertyBagRuntime = Wrapper->GetPropertyBag();
        return true;
    }
    return false;
}

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





void FSequenceEvent_BlueprintCall::SetEventSequenceRunning(UEventSequenceRunning* EventSequenceInstance)
{
    EventSequenceRunning = EventSequenceInstance;
}
