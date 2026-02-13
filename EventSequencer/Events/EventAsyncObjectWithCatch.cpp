// EventAsyncObjectWithCatch.cpp
#include "EventAsyncObjectWithCatch.h"


UEventAsyncObjectWithCatch* UEventAsyncObjectWithCatch::CreateAsyncActionWithCatch(UObject* WorldContextObject,
    const TMap<FString, FString>& Context, TSubclassOf<UEventAsyncObject> CatchAction,
    EInterruptBehavior InterruptBehavior)
{
    UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull);
    if (!World)
        return nullptr;
    
    UEventAsyncObjectWithCatch* Action = NewObject<UEventAsyncObjectWithCatch>(World);
    if (Action)
    {
        Action->SetContext(Context);
        Action->CatchActionClass = CatchAction;
        Action->InterruptBehavior = InterruptBehavior;
    }
    
    return Action;
}


void UEventAsyncObjectWithCatch::InternalComplete(EEventExecuteResult Result, const FString& Reason)
{
    if (Result == EEventExecuteResult::Failed && CatchActionClass)
    {
        // 失败且有Catch块，执行Catch
        ExecuteCatch(Reason);
    }
    else
    {
        // 直接完成
        Super::InternalComplete(Result, Reason);
    }
}


void UEventAsyncObjectWithCatch::ExecuteCatch(const FString& FailureReason)
{
    if (!CatchActionClass)
    {
        // 没有Catch块，直接失败
        Super::InternalComplete(EEventExecuteResult::Failed, FailureReason);
        return;
    }
    
    UWorld* World = GetWorld();
    if (!World)
    {
        Super::InternalComplete(EEventExecuteResult::Failed, FailureReason);
        return;
    }
    
    // 创建Catch块实例
    CatchActionInstance = NewObject<UEventAsyncObject>(World, CatchActionClass);
    if (!CatchActionInstance)
    {
        Super::InternalComplete(EEventExecuteResult::Failed, FailureReason);
        return;
    }
    
    // 传递上下文
    TMap<FString, FString> CatchContext = ContextData;
    CatchContext.Add(TEXT("FailureReason"), FailureReason);
    CatchActionInstance->SetContext(CatchContext);
    
    // 绑定Catch完成委托
    CatchActionInstance->OnCompleted.AddDynamic(this, &UEventAsyncObjectWithCatch::OnCatchActionCompleted);
    
    // 激活Catch块
    CatchActionInstance->Activate();
}

void UEventAsyncObjectWithCatch::OnCatchActionCompleted(EEventExecuteResult Result, const FString& Reason)
{
    CatchResult = Result;
    CatchReason = Reason;
    
    // 触发Catch完成委托
    OnCatchCompleted.Broadcast(Result, Reason);
    
    // 主异步完成（使用Catch块的结果）
    Super::InternalComplete(Result, Reason);
}