// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "FPSAIGuard.generated.h"

class UPawnSensingComponent;

UENUM(BlueprintType)
enum class EGuardState : uint8
{
    Idle,
    Suspicious,
    Alerted,
};

UCLASS()
class FPSGAME_API AFPSAIGuard : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AFPSAIGuard();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, Category ="Components")
	UPawnSensingComponent* PawnSensingComp;

	UFUNCTION()
	void OnPawnSeen(APawn* SeenPawn);

	UFUNCTION()
	void OnNoiseHeard(APawn* Instigator0, const FVector& Location, float Volume);

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

protected:

	FRotator OriginalOrientation;
	FTimerHandle TimerHandle_ResetOrientation;
	void ResetOrientation();

    UPROPERTY(ReplicatedUsing=OnRep_GuardState)
    EGuardState GuardState;
    UFUNCTION()
    void OnRep_GuardState();

    void SetGuardState(EGuardState NewState);

    UFUNCTION(BlueprintImplementableEvent, Category = "AI")
    void OnGuardStateChanged(EGuardState NewState);
};
