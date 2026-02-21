#include "SequenceEvent_AsyncBlueprintCall.h"
#include "EventSequenceAsyncBlueprintAction.h"
#include "EventSequencer/EventSequenceSystem.h"
#include "StructUtils/PropertyBag.h"
#include "Misc/DefaultValueHelper.h"


UEventSequenceAsyncBlueprintAction* FSequenceEvent_AsyncBlueprintCall::CreateAsyncInstance(UObject* Outer) const
{
    if (!BlueprintClass)
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
        // Instance->OnCompleted.Add(
        //     Outer, 
        //     &UEventSequenceSystem::OnAsyncActionCompleted,
        //     Instance->GetUniqueID()
        // );
        //
        // Instance->OnFailed.AddUObject(
        //     Outer,
        //     &UEventSequenceSystem::OnAsyncActionFailed,
        //     Instance->GetUniqueID()
        // );
    }
    
    return Instance;
}

void FSequenceEvent_AsyncBlueprintCall::DestroyAsyncInstance()
{
    if (AsyncInstance)
    {
        // AsyncInstance->OnCompleted.RemoveAll(this);
        // AsyncInstance->OnFailed.RemoveAll(this);
        AsyncInstance->ConditionalBeginDestroy();
        AsyncInstance = nullptr;
    }
}

bool FSequenceEvent_AsyncBlueprintCall::StartAsyncExecution(
    UEventSequenceAsyncBlueprintAction* Instance, 
    const FAsyncActionContext& Context,
    UPropertyBagWrapper* PropertyBag)
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
        UE_LOG(LogTemp, Warning, TEXT("Failed to sync input data to async blueprint"));
    }
    
    // 3. 如果从序列化状态恢复，尝试反序列化
    if (Context.bRecoveringFromInterrupt && !SerializedState.IsEmpty())
    {
        Instance->DeserializeState(SerializedState);
    }
    
    // 4. 执行异步操作
    Instance->OnExecute(Context);
    
    return true;
}

bool FSequenceEvent_AsyncBlueprintCall::SyncPortData(UEventSequenceAsyncBlueprintAction* Instance,
    UPropertyBagWrapper* PropertyBag, bool bToBlueprint) const
{

    return false;
}

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

bool FSequenceEvent_AsyncBlueprintCall::ValidatePortBindings(const UClass* _BlueprintClass) const
{
    return false;
}

FString FSequenceEvent_AsyncBlueprintCall::SerializeAsyncState() const
{
    if (AsyncInstance && AsyncInstance->SupportsStateSerialization())
    {
        return AsyncInstance->SerializeState();
    }
    return TEXT("");
}

void FSequenceEvent_AsyncBlueprintCall::DeserializeAsyncState(const FString& StateString)
{
    if (AsyncInstance && AsyncInstance->SupportsStateSerialization())
    {
        AsyncInstance->DeserializeState(StateString);
    }
}