// Fill out your copyright notice in the Description page of Project Settings.


#include "FPSAIGuard.h"
#include "Perception/PawnSensingComponent.h"
#include "DrawDebugHelpers.h"
#include "TimerManager.h"
#include "FPSCharacter.h"
#include "FPSGameMode.h"
#include "Engine/World.h"

// Sets default values
AFPSAIGuard::AFPSAIGuard()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	PawnSensingComp = CreateDefaultSubobject<UPawnSensingComponent>(TEXT("PawnSensingComp"));

    GuardState = EGuardState::Idle;
}

// Called when the game starts or when spawned
void AFPSAIGuard::BeginPlay()
{
	Super::BeginPlay();

	// this is strange:
	// as im following the tutorial: register the 2 callbacks in the constructor but it doesnt work
	// and then i spend hours check, recompile, restart ... and finally tried move these to BeginPlay() then it works...
	PawnSensingComp->OnSeePawn.AddDynamic(this, &AFPSAIGuard::OnPawnSeen);
	PawnSensingComp->OnHearNoise.AddDynamic(this, &AFPSAIGuard::OnNoiseHeard);

	OriginalOrientation = GetActorRotation();
}

void AFPSAIGuard::OnPawnSeen(APawn* SeenPawn)
{
	UE_LOG(LogTemp, Log, TEXT("seen !!"));

	if (SeenPawn == nullptr)
	{
		return;
	}

	DrawDebugSphere(GetWorld(), SeenPawn->GetActorLocation(), 32.0f, 12, FColor::Yellow, false, 10.0f);

	// mission fail
	AFPSCharacter* fpcChar = Cast<AFPSCharacter>(SeenPawn);
	if (fpcChar != nullptr)
	{
		AFPSGameMode* GM = Cast<AFPSGameMode>(GetWorld()->GetAuthGameMode());
		if (GM != nullptr)
		{
			GM->CompleteMission(SeenPawn, false);
		}
        
        SetGuardState(EGuardState::Alerted);
	}
}

void AFPSAIGuard::OnNoiseHeard(APawn* Instigator0, const FVector& Location, float Volume)
{
	UE_LOG(LogTemp, Log, TEXT("hear !!"));

	DrawDebugSphere(GetWorld(), Location, 32.0f, 12, FColor::Blue, false, 10.0f);

	// look at noise location
	FVector LookDir = Location - GetActorLocation();
	LookDir.Normalize();
	FRotator LookRot = FRotationMatrix::MakeFromX(LookDir).Rotator();
	LookRot.Pitch = 0.0f;
	LookRot.Roll = 0.0f;
	this->SetActorRotation(LookRot);

	GetWorldTimerManager().ClearTimer(TimerHandle_ResetOrientation);
	GetWorldTimerManager().SetTimer(TimerHandle_ResetOrientation, this, &AFPSAIGuard::ResetOrientation, 3.0f, false);

    SetGuardState(EGuardState::Suspicious);
}

// Called every frame
void AFPSAIGuard::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AFPSAIGuard::ResetOrientation()
{
	SetActorRotation(OriginalOrientation);
    SetGuardState(EGuardState::Idle);
}

void AFPSAIGuard::SetGuardState(EGuardState NewState)
{
    if (GuardState == NewState)
    {
        return;
    }
    
    GuardState = NewState;
    OnGuardStateChanged(GuardState);
}
