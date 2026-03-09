#include "SequenceEvent_AsyncBlueprintCall.h"
#include "EventSequenceAsyncBlueprintAction.h"
#include "EventSequencer/EventSequenceSystem.h"
#include "StructUtils/PropertyBag.h"
#include "Misc/DefaultValueHelper.h"


FString FSequenceEvent_AsyncBlueprintCall::GetDisplayName() const
{
    if (BlueprintClass)
    {
        return "ABP-CALL [" + BlueprintClass->GetName() + "]";
    }
    return "ABP-CALL [None]";
}

void FSequenceEvent_AsyncBlueprintCall::SetEventSequenceRunning(UEventSequenceRunning* EventSequenceInstance)
{
    EventSequenceRunning = EventSequenceInstance;
}

bool FSequenceEvent_AsyncBlueprintCall::Execute(int Index)
{
    if (!BlueprintClass || !EventSequenceRunning.IsValid())
    {
        return false;
    }

    if (!ActionInstance)
    {
        ActionInstance = CreateAsyncInstance(EventSequenceRunning.Get());
    }

    // 需要等待策划 调用 EventSequenceRunning->ExecuteAsyncBlueprintCallEvent()
    if (ActionInstance)
    {
        // 当执行时，就可以对 Property 进行修改
        UPropertyBagWrapper* Wrapper = EventSequenceRunning->GetPropertyBagWrapper();
        ActionInstance->OnStart(Wrapper);

        for (FPortBinding& Bind : OutPropertyValues)
        {
            Wrapper->AddValueFromString(Bind.PropertyName, Bind.PropertyType, Bind.TargetValue);
        }
        
        return true;
    }
    return false;
}

UEventSequenceAsyncBlueprintAction* FSequenceEvent_AsyncBlueprintCall::CreateAsyncInstance(UEventSequenceRunning* Outer) const
{
    if (!BlueprintClass )
    {
        UE_LOG(LogTemp, Error, TEXT("Async blueprint class is not set"));
        return nullptr;
    }
    
    // 创建异步实例
    UEventSequenceAsyncBlueprintAction* Instance = NewObject<UEventSequenceAsyncBlueprintAction>(
        Outer, 
        BlueprintClass
    );
    
    if (Instance)
    {
        // 设置中断语义
        Instance->SetInterruptBehavior(InterruptBehavior);
        
        // 绑定委托
        Instance->OnResolved.AddDynamic(
            Outer, 
            &UEventSequenceRunning::OnAsyncActionResolved
        );
        
        Instance->OnRejected.AddDynamic(
            Outer,
            &UEventSequenceRunning::OnAsyncActionRejected
        );
    }
    
    return Instance;
}

void FSequenceEvent_AsyncBlueprintCall::DestroyAsyncInstance()
{
    if (ActionInstance)
    {
        // AsyncInstance->OnCompleted.RemoveAll(this);
        // AsyncInstance->OnFailed.RemoveAll(this);
        ActionInstance->ConditionalBeginDestroy();
        ActionInstance = nullptr;
    }
}


// 端口数据的绑定不好处理，放一放先
// bool FSequenceEvent_AsyncBlueprintCall::SyncPortData(UEventSequenceAsyncBlueprintAction* Instance,
//     UPropertyBagWrapper* PropertyBag, bool bToBlueprint) const
// {
//     return false;
// }

void FSequenceEvent_AsyncBlueprintCall::HandleAsyncComplete(EAsyncActionResult Result, const FString& Reason)
{
    AsyncResult = Result;
    
    if (Result == EAsyncActionResult::Failed)
    {
        FailureReason = Reason;
    }
    
    // 清理序列化状态
    if (Result != EAsyncActionResult::Pending)
    {
        SerializedState.Empty();
    }
}

// bool FSequenceEvent_AsyncBlueprintCall::ValidatePortBindings(const UClass* _BlueprintClass) const
// {
//     return false;
// }


bool FSequenceEvent_AsyncBlueprintCall::OnExecute()
{
    if (!ActionInstance)
    {
        return false;
    }

    ActionInstance->OnExecute();
    return true;
}

