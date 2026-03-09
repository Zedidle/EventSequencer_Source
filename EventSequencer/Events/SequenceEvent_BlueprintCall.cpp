#include "SequenceEvent_BlueprintCall.h"
#include "EventSequenceBlueprintAction.h"
#include "StructUtils/PropertyBag.h"

FString FSequenceEvent_BlueprintCall::GetDisplayName() const
{
    if (BlueprintClass)
    {
        return "BP-CALL [" + BlueprintClass->GetName() + "]";
    }
    return "BP-CALL [None]";
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
    
    if (!ActionInstance)
    {
        ActionInstance = NewObject<UEventSequenceBlueprintAction>(EventSequenceRunning.Get(), BlueprintClass);
    }

    if (ActionInstance)
    {
	    UPropertyBagWrapper* Wrapper = EventSequenceRunning->GetPropertyBagWrapper();
        ActionInstance->Execute(Wrapper);
        
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
    if (ActionInstance && IsValid(ActionInstance))
    {
        return ActionInstance;
    }
    
    // 创建新实例
    ActionInstance = NewObject<UEventSequenceBlueprintAction>(Outer, BlueprintClass);
    if (!ActionInstance)
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to create blueprint instance for class: %s"), 
               *BlueprintClass->GetName());
    }
    
    return ActionInstance;
}





void FSequenceEvent_BlueprintCall::SetEventSequenceRunning(UEventSequenceRunning* EventSequenceInstance)
{
    EventSequenceRunning = EventSequenceInstance;
}
