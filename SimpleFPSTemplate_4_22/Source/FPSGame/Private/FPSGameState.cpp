// Fill out your copyright notice in the Description page of Project Settings.


#include "FPSGameState.h"
#include "Engine/World.h"
#include "FPSPlayerController.h"

void AFPSGameState::MulticastOnMissionCompleted_Implementation(APawn* InstigatorPawn, bool bMissionSuccess)
{
	
	//FConstPawnIterator It = GetWorld()->GetPawnIterator();
	//for ( ; It; It ++)
	//{
	//	APawn* Pawn = It->Get();
	//	if (Pawn && Pawn->IsLocallyControlled())
	//	{
	//		Pawn->DisableInput(nullptr);
	//	}
	//}

	FConstPlayerControllerIterator PIt = GetWorld()->GetPlayerControllerIterator();

	for (; PIt; PIt++)
	{
		APlayerController* PlayerController = PIt->Get();
		AFPSPlayerController* FPSPlayerController = Cast<AFPSPlayerController>(PlayerController);
		if (FPSPlayerController && FPSPlayerController->IsLocalController())
		{
			FPSPlayerController->OnMissionCompleted(InstigatorPawn, bMissionSuccess);

			APawn* Pawn = FPSPlayerController->GetPawn();
			if (Pawn /* && Pawn->IsLocallyControlled() */)
			{
				Pawn->DisableInput(nullptr);
			}
		}
	}
}
