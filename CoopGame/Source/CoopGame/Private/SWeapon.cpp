// Fill out your copyright notice in the Description page of Project Settings.


#include "SWeapon.h"
#include "Components/SkeletalMeshComponent.h"
#include "DrawDebugHelpers.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystem.h"
#include "Particles/ParticleSystemComponent.h"
#include "Components/SceneComponent.h"
#include "PhysicalMaterials/PhysicalMaterial.h"
#include "CoopGame.h"
#include "TimerManager.h"

static int32 DebugWeaponDrawing = 0;
FAutoConsoleVariableRef ACVRDebugWeaponDrawing(
        TEXT("Coop.DebugWeapons"),
        DebugWeaponDrawing,
        TEXT("Draw debug lines for weapon"),
        ECVF_Cheat
        );

// Sets default values
ASWeapon::ASWeapon()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	//PrimaryActorTick.bCanEverTick = true;


	MeshComp = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("MeshComp"));
	RootComponent = MeshComp;


	MuzzleSocketName = "MuzzleFlashSocket";
	TracerTargetName = "BeamEnd";

	BaseDamage = 20.0f;
	RateOfFire = 120.0f;
}

// Called when the game starts or when spawned
//void ASWeapon::BeginPlay()
//{
//	Super::BeginPlay();
//	
//}

void ASWeapon::Fire()
{
	// trace    pawn eyes -> crosshair

	AActor* MyOwner = GetOwner();
	if (MyOwner != nullptr)
	{
		FVector EyeLocation;
		FRotator EyeRotation;
		MyOwner->GetActorEyesViewPoint(EyeLocation, EyeRotation);

		FVector ShootDir = EyeRotation.Vector();
		FVector TraceEnd = EyeLocation + ShootDir * 10000;

		FCollisionQueryParams QueryParams;
		QueryParams.AddIgnoredActor(MyOwner);
		QueryParams.AddIgnoredActor(this);
		QueryParams.bTraceComplex = true; // true : every triangle   false : simplified bounding boxes
		QueryParams.bReturnPhysicalMaterial = true;

		FVector TracerEndPoint = TraceEnd;

		FHitResult Hit;
		if (GetWorld()->LineTraceSingleByChannel(Hit, EyeLocation, TraceEnd, COLLISION_WEAPON, QueryParams))
		{
			// Hit
			TracerEndPoint = Hit.Location;
			AActor* HitActor = Hit.GetActor();

			EPhysicalSurface surfaceType = UPhysicalMaterial::DetermineSurfaceType(Hit.PhysMaterial.Get());

			float ActualDamage = BaseDamage;
			if (surfaceType == SURFACE_FLESHVULNERABLE)
			{
				ActualDamage *= 4;
			}

			UGameplayStatics::ApplyPointDamage(HitActor, ActualDamage, ShootDir, Hit, GetOwner()->GetInstigatorController(), this, DamageType);

			//if (ImpactEffectDefault != nullptr)
			//{
			//	UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ImpactEffectDefault, Hit.ImpactPoint, Hit.ImpactNormal.Rotation());
			//}

			UParticleSystem* ImpactEffectSelected = nullptr;
			//
			switch (surfaceType)
			{
			case SURFACE_FLESHDEFAULT:
			case SURFACE_FLESHVULNERABLE:
				ImpactEffectSelected = ImpactEffectFlesh;
				break;
			default:
				ImpactEffectSelected = ImpactEffectDefault;
				break;
			}

			if (ImpactEffectSelected != nullptr)
			{
				UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ImpactEffectSelected, Hit.ImpactPoint, Hit.ImpactNormal.Rotation());
			}

		}

		if (DebugWeaponDrawing > 0)
        {
            DrawDebugLine(GetWorld(), EyeLocation, TraceEnd, FColor::White, false, 1.0f, 0, 1.0f);
        }

		PlayFireEffects(TracerEndPoint);

		LastFireTime = GetWorld()->TimeSeconds;
	}
}

void ASWeapon::StartFire()
{
	float TimeBetweenShots = 60.0f / RateOfFire;
	float FirstDelay = LastFireTime + TimeBetweenShots - GetWorld()->GetTimeSeconds();
	FirstDelay = FMath::Max(FirstDelay, 0.0f);
	GetWorldTimerManager().SetTimer(TimerHandle_TimeBetweenShots, this, &ASWeapon::Fire, TimeBetweenShots, true, FirstDelay);
}

void ASWeapon::StopFire()
{
	GetWorldTimerManager().ClearTimer(TimerHandle_TimeBetweenShots);
}

void ASWeapon::PlayFireEffects(FVector TracerEndPoint)
{

	if (MuzzleEffect != nullptr)
	{
		UGameplayStatics::SpawnEmitterAttached(MuzzleEffect, MeshComp, MuzzleSocketName);
	}

	FVector MuzzleLocation = MeshComp->GetSocketLocation(MuzzleSocketName);
	if (TracerEffect != nullptr)
	{
		UParticleSystemComponent* TracerComp = UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), TracerEffect, MuzzleLocation);
		TracerComp->SetVectorParameter(TracerTargetName, TracerEndPoint);
	}

	// camera shake
	AActor* MyOwner = GetOwner();
	if (MyOwner)
	{
		APawn* OwnerPawn = Cast<APawn>(MyOwner);
		if (OwnerPawn)
		{
			APlayerController* PC = Cast<APlayerController>(OwnerPawn->GetController());
			if (PC)
			{
				if (FireCameraShake != nullptr)
				{
					PC->ClientPlayCameraShake(FireCameraShake);
				}
			}
		}
	}

}

// Called every frame
//void ASWeapon::Tick(float DeltaTime)
//{
//	Super::Tick(DeltaTime);
//
//}

