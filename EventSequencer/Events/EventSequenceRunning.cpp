// Fill out your copyright notice in the Description page of Project Settings.


#include "EventSequenceRunning.h"
#include "../Events/SequenceEvent/CommonStructs.h"
#include "EventSequencer/EventSequenceSystem.h"
#include "Misc/DefaultValueHelper.h"
#include "SequenceEvent/_SequenceEvent_BREAK.h"
#include "SequenceEvent/_SequenceEvent_GOTO.h"
#include "SequenceEvent/_SequenceEvent_IF.h"
#include "SequenceEvent/_SequenceEvent_LOOP.h"
#include "SequenceEvent/_SequenceEvent_RETURN.h"

void UEventSequenceRunning::AddEvent(FInstancedStruct& Event)
{
	if(!Event.IsValid()) return;
    
	if (auto* DestEvent = Event.GetMutablePtr<FBaseSequenceEvent>())
	{
		EventQueue.Add(Event);
	}
}

void UEventSequenceRunning::AppendEvents(TArray<FInstancedStruct>& Events)
{
	if (Events.IsEmpty()) return;
	
	for (auto& Event : Events)
	{
		AddEvent(Event);
	}
}

void UEventSequenceRunning::GOTO(FName Label)
{
	if(int* index = Label2Index.Find(Label))
	{
		GOTO(*index);
	}
}

void UEventSequenceRunning::GOTO(int Index)
{
	if(EventQueue.IsValidIndex(Index))
	{
		CurEventIndex = Index;
		if (auto* DestEvent =  EventQueue[Index].GetMutablePtr<FBaseSequenceEvent>())
		{
			DestEvent->SetState(EEventState::Idle);
		}
	}
}


bool UEventSequenceRunning::IsCompleted() const
{
	for (auto Event : EventQueue)
	{
		if (const FBaseSequenceEvent* E = Event.GetPtr<FBaseSequenceEvent>())
		{
			if (E->GetState() != EEventState::Completed)
			{
				return false;
			}
		}
	}
	return true;
}

FInstancedStruct& UEventSequenceRunning::GetCurEvent()
{
	return EventQueue[CurEventIndex];
}

void UEventSequenceRunning::AddLabel(FName Label)
{
	// 暂不考虑Label重复问题
	Label2Index.Add(Label,EventQueue.Num());
}

void UEventSequenceRunning::Start()
{
	bPause = false;
}

void UEventSequenceRunning::Stop()
{
	bPause = true;
}

void UEventSequenceRunning::Next()
{
	FInstancedStruct& CurEvent = GetCurEvent();
	if (FBaseSequenceEvent* E = CurEvent.GetMutablePtr<FBaseSequenceEvent>())
	{
		if (E->Execute())
		{
			CurEventIndex++;
			if (CurEventIndex >= EventQueue.Num())
			{
				UE_LOG(LogTemp, Log, TEXT("Sequence Completed!"));
				CurEventIndex = INDEX_NONE;
			}
		}
	}
}

void UEventSequenceRunning::Exit()
{
	Destroy();
}

void UEventSequenceRunning::Destroy()
{
	if (UEventSequenceSystem* EventSequenceSystem = UEventSequenceSystem::GetInstance(GetWorld()))
	{
		EventSequenceSystem->RemoveEventSequence(this);
	}
}


bool UEventSequenceRunning::EvaluateCondition(const FSequenceCondition& Condition)
{
	if (Condition.PropertyName.IsNone()) return true;
    
    // 获取属性描述
    const FPropertyBagPropertyDesc* Desc = PropertyBagRuntime.FindPropertyDescByName(Condition.PropertyName);
    if (!Desc)
    {
        return false;
    }
    
    // 根据属性类型执行不同的比较,目前支持还不全面
    switch (Desc->ValueType)
    {
    case EPropertyBagPropertyType::Bool:
        {
            TValueOrError<bool, EPropertyBagResult> V = PropertyBagRuntime.GetValueBool(Condition.PropertyName);
            if (V.GetError() == EPropertyBagResult::Success)
            {
                bool ComparisonValue = false;
                if (TryParseBool(Condition.ComparisonValue, ComparisonValue))
                {
                    return CompareBool(V.GetValue(), ComparisonValue, Condition.Operator);
                }
            }
        }
        break;
        
    case EPropertyBagPropertyType::Int32:
        {
            TValueOrError<int32, EPropertyBagResult> V = PropertyBagRuntime.GetValueInt32(Condition.PropertyName);
    		if (V.GetError() == EPropertyBagResult::Success)
    		{
    			int32 ComparisonValue;
    			if (TryParseInt32(Condition.ComparisonValue, ComparisonValue))
    			{
    				return CompareInt32(V.GetValue(), ComparisonValue, Condition.Operator);
    			}
    		}
        }
        break;
        
    case EPropertyBagPropertyType::Float:
        {
    		TValueOrError<float, EPropertyBagResult> V = PropertyBagRuntime.GetValueFloat(Condition.PropertyName);
    		if (V.GetError() == EPropertyBagResult::Success)
    		{
    			float ComparisonValue;
    			if (TryParseFloat(Condition.ComparisonValue, ComparisonValue))
    			{
    				return CompareFloat(V.GetValue(), ComparisonValue, Condition.Operator);
    			}
    		}
        }
        break;
        
    case EPropertyBagPropertyType::Double:
        {
    		TValueOrError<double, EPropertyBagResult> V = PropertyBagRuntime.GetValueDouble(Condition.PropertyName);
    		if (V.GetError() == EPropertyBagResult::Success)
    		{
    			double ComparisonValue;
    			if (TryParseDouble(Condition.ComparisonValue, ComparisonValue))
    			{
    				return CompareDouble(V.GetValue(), ComparisonValue, Condition.Operator);
    			}
    		}
        }
        break;
        
    case EPropertyBagPropertyType::String:
        {
    		TValueOrError<FString, EPropertyBagResult> V = PropertyBagRuntime.GetValueString(Condition.PropertyName);
    		if (V.GetError() == EPropertyBagResult::Success)
    		{
    			FString ComparisonValue;
    			if (TryParseString(Condition.ComparisonValue, ComparisonValue))
    			{
    				return CompareString(V.GetValue(), ComparisonValue, Condition.Operator);
    			}
    		}
        }
        break;
        
    case EPropertyBagPropertyType::Name:
        {
    		TValueOrError<FName, EPropertyBagResult> V = PropertyBagRuntime.GetValueName(Condition.PropertyName);
    		if (V.GetError() == EPropertyBagResult::Success)
    		{
    			FName ComparisonValue;
    			if (TryParseName(Condition.ComparisonValue, ComparisonValue))
    			{
    				return CompareName(V.GetValue(), ComparisonValue, Condition.Operator);
    			}
    		}
        }
        break;
        
    case EPropertyBagPropertyType::Text:
        {
    		TValueOrError<FText, EPropertyBagResult> V = PropertyBagRuntime.GetValueText(Condition.PropertyName);
    		if (V.GetError() == EPropertyBagResult::Success)
    		{
    			FText ComparisonValue;
    			if (TryParseText(Condition.ComparisonValue, ComparisonValue))
    			{
    				return CompareText(V.GetValue(), ComparisonValue, Condition.Operator);
    			}
    		}
        }
        break;
    }
    
    return false;
	
}

bool UEventSequenceRunning::CompareBool(bool Left, bool Right, ESequenceConditionOperator Op)
{
	switch (Op)
	{
	case ESequenceConditionOperator::Equal: return Left == Right;
	case ESequenceConditionOperator::NotEqual: return Left != Right;
	default: return false;
	}
}

bool UEventSequenceRunning::CompareInt32(int32 Left, int32 Right, ESequenceConditionOperator Op)
{
	switch (Op)
	{
	case ESequenceConditionOperator::Equal: return Left == Right;
	case ESequenceConditionOperator::NotEqual: return Left != Right;
	case ESequenceConditionOperator::Greater: return Left > Right;
	case ESequenceConditionOperator::GreaterEqual: return Left >= Right;
	case ESequenceConditionOperator::Less: return Left < Right;
	case ESequenceConditionOperator::LessEqual: return Left <= Right;
	default: return false;
	}
}

bool UEventSequenceRunning::CompareFloat(const float& Left, const float& Right, ESequenceConditionOperator Op)
{
	// 浮点数比较需要处理精度误差
	const float Epsilon = 0.0001f;
    
	switch (Op)
	{
	case ESequenceConditionOperator::Equal:
		return FMath::IsNearlyEqual(Left, Right, Epsilon);
	case ESequenceConditionOperator::NotEqual:
		return !FMath::IsNearlyEqual(Left, Right, Epsilon);
	case ESequenceConditionOperator::Greater:
		return Left > Right;
	case ESequenceConditionOperator::GreaterEqual:
		return Left >= Right || FMath::IsNearlyEqual(Left, Right, Epsilon);
	case ESequenceConditionOperator::Less:
		return Left < Right;
	case ESequenceConditionOperator::LessEqual:
		return Left <= Right || FMath::IsNearlyEqual(Left, Right, Epsilon);
	case ESequenceConditionOperator::Contains:
	case ESequenceConditionOperator::StartsWith:
	case ESequenceConditionOperator::EndsWith:
		// 对于浮点数，这些操作符没有意义，返回 false
		return false;
	default:
		return false; // 未知操作符
	}
}

bool UEventSequenceRunning::CompareDouble(const double& Left, const double& Right, ESequenceConditionOperator Op)
{
	// 双精度浮点数比较需要处理精度误差
	const double Epsilon = 0.000001;
    
	switch (Op)
	{
	case ESequenceConditionOperator::Equal:
		return FMath::IsNearlyEqual(Left, Right, Epsilon);
	case ESequenceConditionOperator::NotEqual:
		return !FMath::IsNearlyEqual(Left, Right, Epsilon);
	case ESequenceConditionOperator::Greater:
		return Left > Right;
	case ESequenceConditionOperator::GreaterEqual:
		return Left >= Right || FMath::IsNearlyEqual(Left, Right, Epsilon);
	case ESequenceConditionOperator::Less:
		return Left < Right;
	case ESequenceConditionOperator::LessEqual:
		return Left <= Right || FMath::IsNearlyEqual(Left, Right, Epsilon);
	default:
		return false; // 不支持的操作符（Contains, StartsWith, EndsWith 对数值类型无意义）
	}
}

bool UEventSequenceRunning::CompareString(const FString& Left, const FString& Right, ESequenceConditionOperator Op)
{
	switch (Op)
	{
	case ESequenceConditionOperator::Equal:
		return Left.Equals(Right, ESearchCase::CaseSensitive);
	case ESequenceConditionOperator::NotEqual:
		return !Left.Equals(Right, ESearchCase::CaseSensitive);
	case ESequenceConditionOperator::Greater:
		return Left > Right;
	case ESequenceConditionOperator::GreaterEqual:
		return Left >= Right;
	case ESequenceConditionOperator::Less:
		return Left < Right;
	case ESequenceConditionOperator::LessEqual:
		return Left <= Right;
	case ESequenceConditionOperator::Contains:
		return Left.Contains(Right);
	case ESequenceConditionOperator::StartsWith:
		return Left.StartsWith(Right);
	case ESequenceConditionOperator::EndsWith:
		return Left.EndsWith(Right);
	default:
		return false;
	}
}

bool UEventSequenceRunning::CompareName(const FName& Left, const FName& Right, ESequenceConditionOperator Op)
{
	// FName 的比较是大小写不敏感的
	switch (Op)
	{
	case ESequenceConditionOperator::Equal:
		return Left.IsEqual(Right, ENameCase::IgnoreCase);
	case ESequenceConditionOperator::NotEqual:
		return !Left.IsEqual(Right, ENameCase::IgnoreCase);
	case ESequenceConditionOperator::Contains:
		// FName 没有 Contains 方法，转换为字符串处理
		return Left.ToString().Contains(Right.ToString(), ESearchCase::IgnoreCase);
	case ESequenceConditionOperator::StartsWith:
		return Left.ToString().StartsWith(Right.ToString(), ESearchCase::IgnoreCase);
	case ESequenceConditionOperator::EndsWith:
		return Left.ToString().EndsWith(Right.ToString(), ESearchCase::IgnoreCase);
	default:
		return false; // 不支持的操作符（如大于、小于）
	}
}

bool UEventSequenceRunning::CompareText(const FText& Left, const FText& Right, ESequenceConditionOperator Op)
{
	// 转换为字符串进行比较
	FString LeftString = Left.ToString();
	FString RightString = Right.ToString();
    
	switch (Op)
	{
	case ESequenceConditionOperator::Equal:
		return LeftString.Equals(RightString, ESearchCase::CaseSensitive);
	case ESequenceConditionOperator::NotEqual:
		return !LeftString.Equals(RightString, ESearchCase::CaseSensitive);
	case ESequenceConditionOperator::Contains:
		return LeftString.Contains(RightString);
	case ESequenceConditionOperator::StartsWith:
		return LeftString.StartsWith(RightString);
	case ESequenceConditionOperator::EndsWith:
		return LeftString.EndsWith(RightString);
	default:
		return false; // 不支持的操作符（如大于、小于）
	}
}

bool UEventSequenceRunning::TryParseBool(const FString& StringValue, bool& OutValue)
{
	// 移除首尾空格
	FString Trimmed = StringValue.TrimStartAndEnd();
    
	// 转换为小写以简化比较
	FString Lower = Trimmed.ToLower();
    
	// 尝试解析常见的布尔表示形式
	if (Lower == "true" || Lower == "1" || Lower == "yes" || Lower == "on" || Lower == "t")
	{
		OutValue = true;
		return true;
	}

	if (Lower == "false" || Lower == "0" || Lower == "no" || Lower == "off" || Lower == "f")
	{
		OutValue = false;
		return true;
	}
    
	// 尝试转换为整数
	int32 IntValue = 0;
	if (FDefaultValueHelper::ParseInt(Trimmed, IntValue))
	{
		OutValue = IntValue != 0;
		return true;
	}
    
	// 尝试转换为浮点数
	float FloatValue = 0.0f;
	if (FDefaultValueHelper::ParseFloat(Trimmed, FloatValue))
	{
		OutValue = FloatValue != 0.0f;
		return true;
	}
    
	// 无法解析
	return false;
}

bool UEventSequenceRunning::TryParseInt32(const FString& StringValue, int32& OutValue)
{
	if (StringValue.IsEmpty())
	{
		return false;
	}
    
	// 移除首尾空格
	FString Trimmed = StringValue.TrimStartAndEnd();
    
	// 检查是否为空字符串
	if (Trimmed.IsEmpty())
	{
		return false;
	}
    
	if (FDefaultValueHelper::ParseInt(Trimmed, OutValue))
	{
		return true;
	}
	
	return false;
}

bool UEventSequenceRunning::TryParseFloat(const FString& StringValue, float& OutValue)
{
	if (StringValue.IsEmpty())
	{
		return false;
	}
    
	FString Trimmed = StringValue.TrimStartAndEnd();
    
	if (FDefaultValueHelper::ParseFloat(Trimmed, OutValue))
	{
		return true;
	}

	return false;
}

bool UEventSequenceRunning::TryParseDouble(const FString& StringValue, double& OutValue)
{
	if (StringValue.IsEmpty())
	{
		return false;
	}
    
	FString Trimmed = StringValue.TrimStartAndEnd();
    
	if (FDefaultValueHelper::ParseDouble(Trimmed, OutValue))
	{
		return true;
	}
	return false;
}

bool UEventSequenceRunning::TryParseString(const FString& StringValue, FString& OutValue)
{
	if (StringValue.IsEmpty())
	{
		OutValue = TEXT("");
		return true;
	}
    
	OutValue = StringValue.TrimStartAndEnd();
	return true;
}

bool UEventSequenceRunning::TryParseName(const FString& StringValue, FName& OutValue)
{
	if (StringValue.IsEmpty())
	{
		OutValue = NAME_None;
		return true;
	}
    
	FString Trimmed = StringValue.TrimStartAndEnd();
    
	// 检查是否是特殊名称
	if (Trimmed.Equals(TEXT("none"), ESearchCase::IgnoreCase))
	{
		OutValue = NAME_None;
		return true;
	}
    
	// 处理可能的命名空间格式：Package.Group.Object
	if (Trimmed.Contains(TEXT(".")))
	{
		// 已经是有效的 FName 格式
		OutValue = FName(*Trimmed);
		return true;
	}
    
	// 处理可能的空名称
	if (Trimmed.Equals(TEXT("")) || Trimmed.Equals(TEXT(" ")))
	{
		OutValue = NAME_None;
		return true;
	}
    
	// 检查是否是布尔值转名称
	if (Trimmed.Equals(TEXT("true"), ESearchCase::IgnoreCase))
	{
		OutValue = FName(TEXT("true"));
		return true;
	}
	else if (Trimmed.Equals(TEXT("false"), ESearchCase::IgnoreCase))
	{
		OutValue = FName(TEXT("false"));
		return true;
	}
    
	// 检查是否是数字转名称
	if (Trimmed.IsNumeric())
	{
		OutValue = FName(*Trimmed);
		return true;
	}
    
	// 默认情况：创建 FName
	OutValue = FName(*Trimmed);
	return true;
}

bool UEventSequenceRunning::TryParseText(const FString& StringValue, FText& OutValue)
{
	FString Trimmed = StringValue.TrimStartAndEnd();
    
	if (Trimmed.IsEmpty() || Trimmed.Equals(TEXT("empty"), ESearchCase::IgnoreCase))
	{
		OutValue = FText::GetEmpty();
	}
	else if (Trimmed.Equals(TEXT("none"), ESearchCase::IgnoreCase))
	{
		OutValue = FText::GetEmpty();
	}
	else
	{
		OutValue = FText::FromString(Trimmed);
	}
    
	return true;
}

void UEventSequenceRunning::SetDataAsset(UEventSequenceDA* DataAsset)
{
	if (!DataAsset) return;
	
	InitDataAsset = DataAsset;
	PropertyBagRuntime = InitDataAsset->PropertyBagDefault;
	PropertyBagRuntime.MigrateToNewBagInstance(InitDataAsset->PropertyBagInput);
}

void UEventSequenceRunning::LOOP(const FEventState_LOOP& LoopState)
{
	if (LoopStateStack.IsEmpty())
	{
		LoopStateStack.Push(LoopState);
		GOTO(LoopState.LoopStartIndex);
		return;
	}
	
	FEventState_LOOP& CurLoopState = LoopStateStack.Last();
	if (CurLoopState.LoopEventSelf == LoopState.LoopEventSelf)
	{
		CurLoopState.CurLoopTimes ++;
		if (CurLoopState.MaxLoopTimes > 0 && CurLoopState.CurLoopTimes == CurLoopState.MaxLoopTimes)
		{
			GOTO(CurLoopState.LoopEndIndex);
			LoopStateStack.Pop();
		}
		// 无限循环，只能等Break或Return跳出
		else if (CurLoopState.MaxLoopTimes == 0)
		{
			GOTO(CurLoopState.LoopStartIndex);
		}
	}
	else
	{
		LoopStateStack.Push(LoopState);
		GOTO(LoopState.LoopStartIndex);
	}
}

void UEventSequenceRunning::BREAK()
{
	if (LoopStateStack.IsEmpty())
	{
		return;
	}

	FEventState_LOOP& CurLoopState = LoopStateStack.Last();
	GOTO(CurLoopState.LoopEndIndex);
	LoopStateStack.Pop();
}


void UEventSequenceRunning::Tick(float DeltaTime)
{
	if (bPause) return;
        
	FInstancedStruct& CurEventStruct = EventQueue[CurEventIndex];
	if (const F_SequenceEvent_GOTO* CurEvent_GOTO = CurEventStruct.GetPtr<F_SequenceEvent_GOTO>())
	{
		if (EvaluateCondition(CurEvent_GOTO->Condition))
		{
			GOTO(CurEvent_GOTO->TargetLabel);
		}
	}
	else if (const F_SequenceEvent_BREAK* CurEvent_BREAK = CurEventStruct.GetPtr<F_SequenceEvent_BREAK>())
	{
		// 跳出一层循环状态
		if (EvaluateCondition(CurEvent_BREAK->Condition))
		{
			BREAK();
		}
	}
	else if (const F_SequenceEvent_LOOP* CurEvent_LOOP = CurEventStruct.GetPtr<F_SequenceEvent_LOOP>())
	{
		// 循环状态
		if (EvaluateCondition(CurEvent_LOOP->Condition))
		{
			LOOP(CurEvent_LOOP->State);
		}
	}
	else if (const F_SequenceEvent_IF* CurEvent_IF = CurEventStruct.GetPtr<F_SequenceEvent_IF>())
	{
		// 进入条件分支判断
		if (EvaluateCondition(CurEvent_IF->Condition))
		{
			GOTO(CurEvent_IF->TrueEventsStartIndex);
		}
		else
		{
			GOTO(CurEvent_IF->FalseEventsStartIndex);
		}
	}
	else if (const F_SequenceEvent_RETURN* CurEvent_RETURN = CurEventStruct.GetPtr<F_SequenceEvent_RETURN>())
	{
		// 结束该序列
		if (EvaluateCondition(CurEvent_RETURN->Condition))
		{
			Exit();
		}
	}
	else if (FBaseSequenceEvent* Event = CurEventStruct.GetMutablePtr<FBaseSequenceEvent>())
	{
		switch (Event->GetState())
		{
		case EEventState::Idle:
			Event->OnPending();
			break;
			
		case EEventState::Pending:
			Event->OnEnter();
			break;
            
		case EEventState::Active:
			Event->Tick(DeltaTime);
			break;
            
		case EEventState::Completed:
			Event->OnExit();
			CurEventIndex++;
            
			if (CurEventIndex >= EventQueue.Num())
			{
				UE_LOG(LogTemp, Log, TEXT("Sequence Completed!"));
				CurEventIndex = INDEX_NONE;
			}
			break;
		}
	}
}
