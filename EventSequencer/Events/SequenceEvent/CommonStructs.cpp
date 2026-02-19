
#include "CommonStructs.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "../../Interfaces/EventSequencerInterface.h"

FString FMoveSequenceEvent::GetDisplayName() const
{
	return Super::GetDisplayName() + FString::Printf(TEXT("Move ApproachDistance: %f"), Property.ApproachDistance);
}

void FMoveSequenceEvent::OnEnter()
{
	FBaseSequenceEvent::OnEnter();
		
	GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Cyan, *FString::Printf(TEXT("MoveSequenceEvent::OnEnter")));
	if (NPCPawn)
	{
		StartLocation = NPCPawn->GetActorLocation();
	}
}

bool FMoveSequenceEvent::Execute(int Index)
{
	GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Cyan, *FString::Printf(TEXT("MoveSequenceEvent::Next")));
	return true;
}

void FMoveSequenceEvent::Tick(float DeltaTime)
{
	FBaseSequenceEvent::Tick(DeltaTime);
	GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Cyan, *FString::Printf(TEXT("MoveSequenceEvent::Tick")));

	if (!IsValid(NPCPawn))
	{
		State = EEventState::Completed;
		return;
	}
		
	FVector CurLocation = NPCPawn->GetActorLocation();
    
	if (FVector::Distance(CurLocation, Property.TargetLocation) < Property.ApproachDistance)
	{
		State = EEventState::Completed;
	}
	else if (!bMoving)
	{
		if (AController* Controller = NPCPawn->GetController())
		{
			UAIBlueprintHelperLibrary::SimpleMoveToLocation(Controller, Property.TargetLocation);
		}
		bMoving = true;
	}
}




FString FDialogSequenceEvent::GetDisplayName() const
{
	return Super::GetDisplayName() + FString::Printf(TEXT("Dialog Num: %d"), Property.DialogLines.Num());
}

void FDialogSequenceEvent::OnEnter()
{
	FBaseSequenceEvent::OnEnter();
	// 读表把对话内容加入到DialogItems
	// xxxxx
	GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Cyan, *FString::Printf(TEXT("FDialogSequenceEvent::OnEnter")));
	
}

bool FDialogSequenceEvent::Execute(int Index)
{
	GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Cyan, *FString::Printf(TEXT("FDialogSequenceEvent::Next")));
	return true;
}

void FDialogSequenceEvent::Tick(float DeltaTime)
{
	FBaseSequenceEvent::Tick(DeltaTime);
	GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Cyan, *FString::Printf(TEXT("FDialogSequenceEvent::Tick")));
}




FString FWaitSequenceEvent::GetDisplayName() const
{
	return Super::GetDisplayName() + FString::Printf(TEXT("Wait For: %f"), Property.Duration);
}

void FWaitSequenceEvent::OnEnter()
{
	FBaseSequenceEvent::OnEnter();
	GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Cyan, *FString::Printf(TEXT("FDialogSequenceEvent::OnEnter")));
}

bool FWaitSequenceEvent::Execute(int Index)
{
	GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Cyan, *FString::Printf(TEXT("FWaitSequenceEvent::Next")));
	OnExit();
	return true;
}

void FWaitSequenceEvent::Tick(float DeltaTime)
{
	CurTime += DeltaTime;
	if (CurTime >= Property.Duration)
	{
		Execute();
	}
	
	GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Cyan, *FString::Printf(TEXT("FWaitSequenceEvent::Tick Waiting")));
}

void FWaitSequenceEvent::OnExit()
{
	FBaseSequenceEvent::OnExit();
}





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
