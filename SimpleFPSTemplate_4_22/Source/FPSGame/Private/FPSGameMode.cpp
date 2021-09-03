// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#include "FPSGameMode.h"
#include "FPSHUD.h"
#include "FPSCharacter.h"
#include "UObject/ConstructorHelpers.h"
#include "Kismet/GameplayStatics.h"
#include "FPSGameState.h"
#include "Engine/World.h"
#include "FPSPlayerController.h"

AFPSGameMode::AFPSGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/Blueprints/BP_Player"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;

	// use our custom HUD class
	HUDClass = AFPSHUD::StaticClass();

    GameStateClass = AFPSGameState::StaticClass();
}

void AFPSGameMode::CompleteMission(APawn* InstigatorPawn, bool bMissionSuccess)
{
	if(InstigatorPawn)
	{
		//InstigatorPawn->DisableInput(nullptr);

        if (SpectatingViewpointClass)
        {
            TArray<AActor*> ReturnedActors;
            UGameplayStatics::GetAllActorsOfClass(this, SpectatingViewpointClass, ReturnedActors);
            
            if (ReturnedActors.Num() > 0)
            {
                AActor* NewViewTarget = ReturnedActors[0];
                

                // all PlayerControllers
				FConstPlayerControllerIterator PIt = GetWorld()->GetPlayerControllerIterator();
                for (; PIt; PIt++)
                {
					APlayerController* PlayerController = PIt->Get();
					AFPSPlayerController* FPSPlayerController = Cast<AFPSPlayerController>(PlayerController);
                    if (FPSPlayerController)
                    {
                        FPSPlayerController->SetViewTargetWithBlend(NewViewTarget, 0.5f, EViewTargetBlendFunction::VTBlend_Cubic);
                    }
                }
            }
        }
        else
        {
            UE_LOG(LogTemp, Warning, TEXT("SpectatingViewpointClass is nullptr"));
        }
    }

    AFPSGameState* GS = GetGameState<AFPSGameState>();
    if (GS)
    {
        GS->MulticastOnMissionCompleted(InstigatorPawn, bMissionSuccess);
    }

	//OnMissionCompleted(InstigatorPawn, bMissionSuccess);
}
