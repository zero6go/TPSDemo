// Fill out your copyright notice in the Description page of Project Settings.


#include "TPSWeapon.h"
#include "Components/SkeletalMeshComponent.h"
#include "DrawDebugHelpers.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"
#include "Camera/CameraShakeBase.h"
#include "Animation/AnimMontage.h"
#include "TPSCharacter.h"
#include "TPSActor.h"
#include "Kismet/KismetMathLibrary.h"
#include "Net/UnrealNetwork.h"

// Sets default values
ATPSWeapon::ATPSWeapon()
{
	WeaponAttachSocket = "WeaponSocket1";
	MuzzleSocketName = "MuzzleSocket";
	BlockCheckSocketName = "BlockCheckSocket";
	FireDeltaTime = 0.1f;
	AmmoMax = 30;
	ReloadSpeed = 1.0f;
	DamageBase = 20.0f;
	ImpactForce = 1000.0f;
	RecoilForce = 0.15f;

	MeshComp = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("MeshComp"));
	RootComponent = MeshComp;

	SetReplicates(true);
}

// Called when the game starts or when spawned
void ATPSWeapon::BeginPlay()
{
	Super::BeginPlay();

}

void ATPSWeapon::ServerFire_Implementation()
{
	Fire();
}

bool ATPSWeapon::ServerFire_Validate()
{
	return true;
}

void ATPSWeapon::Fire()
{
	if (GetLocalRole() < ROLE_Authority)
	{
		ServerFire();
		return;
	}
	AActor* Character = GetOwner();
	if (Character)
	{
		FVector EyeLocation;
		FRotator EyeRotation;
		APawn* Pawn = Cast<APawn>(Character);
		Pawn->GetActorEyesViewPoint(EyeLocation, EyeRotation);

		FVector TraceStart = EyeLocation;
		FVector TraceEnd = EyeLocation + EyeRotation.Vector() * 10000;
		FHitResult Hit,HitBlocking;
		FCollisionQueryParams QueryParams;
		QueryParams.AddIgnoredActor(this);
		QueryParams.AddIgnoredActor(Character);
		QueryParams.bTraceComplex = true;

		GetWorld()->LineTraceSingleByChannel(Hit, TraceStart, TraceEnd, ECollisionChannel::ECC_GameTraceChannel4, QueryParams);

		//准星射线命中
		if (Hit.bBlockingHit)
		{
			FVector TraceBlockingStart = MeshComp->GetSocketLocation(BlockCheckSocketName);
			FVector TraceBlockingEnd = Hit.Location;
			GetWorld()->LineTraceSingleByChannel(HitBlocking, TraceBlockingStart, TraceBlockingEnd, ECollisionChannel::ECC_GameTraceChannel6, QueryParams);
			AActor* HitActor = Hit.GetActor();
			//枪口与目标间无障碍
			if (!HitBlocking.bBlockingHit)
			{
				if (Cast<APawn>(HitActor) || Cast<ATPSActor>(HitActor))
				{
					if (Hit.BoneName == "head")
					{
						UGameplayStatics::ApplyPointDamage(HitActor, DamageBase * 3, EyeRotation.Vector(), Hit, Character->GetInstigatorController(), this, DamageType);
					}
					else
					{
						UGameplayStatics::ApplyPointDamage(HitActor, DamageBase, EyeRotation.Vector(), Hit, Character->GetInstigatorController(), this, DamageType);
					}
				}
				NetMulticastPlayFireEffects(TraceEnd, Hit);
			}
			//枪口与目标间有障碍
			else
			{
				NetMulticastPlayFireEffects(TraceBlockingEnd, HitBlocking);
			}
		}
		//准星射线未命中
		else
		{
			FVector TraceBlockingStart = MeshComp->GetSocketLocation(BlockCheckSocketName);
			FVector TraceBlockingEnd = TraceEnd;
			GetWorld()->LineTraceSingleByChannel(HitBlocking, TraceBlockingStart, TraceBlockingEnd, ECollisionChannel::ECC_GameTraceChannel6, QueryParams);
			if (HitBlocking.bBlockingHit)
			{
				NetMulticastPlayFireEffects(TraceBlockingEnd, HitBlocking);
			}
			else
			{
				NetMulticastPlayFireEffects(TraceEnd, Hit);
			}
		}
		APlayerController* PC= Cast<APlayerController>(Pawn->GetController());
		if (PC)
		{
			PC->ClientStartCameraShake(FireCameraShake);
		}
	}
}

void ATPSWeapon::NetMulticastPlayFireEffects_Implementation(const FVector& TraceEnd, const FHitResult& Hit)
{
	PlayFireEffects(TraceEnd, Hit);
}

bool ATPSWeapon::NetMulticastPlayFireEffects_Validate(const FVector& TraceEnd, const FHitResult& Hit)
{
	return true;
}

void ATPSWeapon::PlayFireEffects(const FVector& TraceEnd, const FHitResult& Hit)
{
	if (Hit.bBlockingHit)
	{
		AActor* HitActor = Hit.GetActor();
		if (Cast<ATPSActor>(HitActor))
		{
			FVector Impuse = UKismetMathLibrary::Normal(Hit.ImpactPoint - MeshComp->GetSocketLocation(MuzzleSocketName)) * ImpactForce;
			Cast<ATPSActor>(HitActor)->MeshComp->AddImpulse(Impuse);
		}
		if (Cast<ATPSCharacter>(HitActor))
		{
			if (FleshImpactEffect)
			{
				UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), FleshImpactEffect, Hit.ImpactPoint, Hit.ImpactNormal.Rotation());
			}
		}
		else
		{
			if (DefaultImpactEffect)
			{
				UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), DefaultImpactEffect, Hit.ImpactPoint, Hit.ImpactNormal.Rotation());
			}
		}
	}
	if (MuzzleEffect)
	{
		UGameplayStatics::SpawnEmitterAttached(MuzzleEffect, MeshComp, MuzzleSocketName);
	}
	if (TraceEffect)
	{
		UParticleSystemComponent* TraceComp = UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), TraceEffect, MeshComp->GetSocketLocation(MuzzleSocketName));
		if (TraceComp)
		{
			if (Hit.bBlockingHit)
			{
				TraceComp->SetVectorParameter("Target", Hit.ImpactPoint);
			}
			else
			{
				TraceComp->SetVectorParameter("Target", TraceEnd);
			}
		}
	}
	if (FireSound)
	{
		UGameplayStatics::SpawnSoundAttached(FireSound, MeshComp, MuzzleSocketName);
	}
}