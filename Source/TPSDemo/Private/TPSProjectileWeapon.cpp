// Fill out your copyright notice in the Description page of Project Settings.


#include "TPSProjectileWeapon.h"
#include "Kismet/GameplayStatics.h"

void ATPSProjectileWeapon::Fire()
{
	AActor* Character = GetOwner();
	if (Character) 
	{
		if (GetLocalRole() < ROLE_Authority)
		{
			ServerFire();
			PlayFireEffects();
			return;
		}
		FVector EyeLocation;
		FRotator EyeRotation;
		Character->GetActorEyesViewPoint(EyeLocation, EyeRotation);
		FRotator TargetRotation = EyeRotation;
		TargetRotation.Pitch += 6.0f;
		FVector TargetLocation = MeshComp->GetSocketLocation(MuzzleSocketName);

		FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
		if (ProjectileClass)
		{
			AActor* Projectile = GetWorld()->SpawnActor<AActor>(ProjectileClass, TargetLocation, TargetRotation, SpawnParams);
			Projectile->SetInstigator(Cast<APawn>(Character));
		}
		PlayFireEffects();
	}
}

void ATPSProjectileWeapon::PlayFireEffects()
{
	if (MuzzleEffect)
	{
		UGameplayStatics::SpawnEmitterAttached(MuzzleEffect, MeshComp, MuzzleSocketName);
	}
	if (FireSound)
	{
		UGameplayStatics::SpawnSoundAttached(FireSound, MeshComp, MuzzleSocketName);
	}
}
