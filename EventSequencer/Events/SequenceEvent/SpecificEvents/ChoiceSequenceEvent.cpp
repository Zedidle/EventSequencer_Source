#include "ChoiceSequenceEvent.h"
#include "EventSequencer/Interfaces/EventSequencerInterface.h"

FString FChoiceSequenceEvent::GetDisplayName() const
{
	return Super::GetDisplayName() + FString::Printf(TEXT("Choice Choices Num: %d"), Property.Choices.Num());
}

void FChoiceSequenceEvent::OnEnter()
{
	FBaseSequenceEvent::OnEnter();

	// 针对对话所用的UMG生成对话选项界面
	if (Property.ChoiceWidgetClass)
	{
		// CreateWidget<UChoiceWidget>(this, ChoiceWidgetClass);
		// 需要临时给每个选项绑定操作
	}
	
	GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Cyan, *FString::Printf(TEXT("FChoiceSequenceEvent::OnEnter")));
}

bool FChoiceSequenceEvent::Execute(int Index)
{
	GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Cyan, *FString::Printf(TEXT("FChoiceSequenceEvent::Next")));

	if (ChoiceWidget)
	{
		// ChoiceWidget->MakeChoice(Index);
		if (PlayerPawn->Implements<UEventSequencerInterface>())
		{
			// 可以在 IEventSequencerInterface 自定义接口传入任意参数影响Actor自身
			IEventSequencerInterface::Execute_OnChoice(PlayerPawn.Get(), Index);
		}
	}
		
	return true;
}

void FChoiceSequenceEvent::Tick(float DeltaTime)
{
	GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Cyan, *FString::Printf(TEXT("FChoiceSequenceEvent::Tick Waiting")));
}

void FChoiceSequenceEvent::OnExit()
{
	FBaseSequenceEvent::OnExit();
}
