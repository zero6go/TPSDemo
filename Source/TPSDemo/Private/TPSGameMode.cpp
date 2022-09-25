// Fill out your copyright notice in the Description page of Project Settings.


#include "TPSGameMode.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"

ATPSGameMode::ATPSGameMode()
{
	bInSpeedDown = false;
	TrackerBotCount = 0;
	BarrelCount = 0;
	WeaponSpawnCount = 1;
	BuffSpawnCount = 1;
	TrackerBotSpawnTime = 10.0f;
	WeaponSpawnTime = 6.0f;
	BuffSpawnTime = 5.0f;
}

void ATPSGameMode::BeginPlay()
{
	if (GetLocalRole() == ROLE_Authority)
	{
		if (TrackerBotStart)
		{
			UGameplayStatics::GetAllActorsOfClass(this, TrackerBotStart, TrackerBotStarts);
			FTimerHandle TrackerBotTimerHandle;
			GetWorld()->GetTimerManager().SetTimer(TrackerBotTimerHandle, this, &ATPSGameMode::SpawnTrackerBotAndBarrel, TrackerBotSpawnTime, true);
		}
		if (WeaponStart)
		{
			UGameplayStatics::GetAllActorsOfClass(this, WeaponStart, WeaponStarts);
			FTimerHandle WeaponTimerHandle;
			GetWorld()->GetTimerManager().SetTimer(WeaponTimerHandle, this, &ATPSGameMode::SpawnWeapon, WeaponSpawnTime, true);
		}
		if (BuffStart)
		{
			UGameplayStatics::GetAllActorsOfClass(this, BuffStart, BuffStarts);
			FTimerHandle BuffTimerHandle;
			GetWorld()->GetTimerManager().SetTimer(BuffTimerHandle, this, &ATPSGameMode::SpawnBuff, BuffSpawnTime, true);
		}
		if (Barrel)
		{
			TArray<AActor*> Temp;
			UGameplayStatics::GetAllActorsOfClass(this, Barrel, Temp);
			BarrelCount = Temp.Num();
		}
	}
}

void ATPSGameMode::SpawnTrackerBotAndBarrel()
{
	if (TrackerBotCount < 3)
	{
		if (TrackerBotStarts.Num() > 0)
		{
			AActor* TargetStart = TrackerBotStarts[UKismetMathLibrary::RandomInteger(TrackerBotStarts.Num())];
			FActorSpawnParameters SpawnParams;
			SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
			GetWorld()->SpawnActor<AActor>(TrackerBot, TargetStart->GetActorTransform(), SpawnParams);
			TrackerBotCount++;
		}
	}
	if (BarrelCount < 10)
	{
		if (TrackerBotStarts.Num() > 0 && Barrel)
		{
			AActor* TargetStart = TrackerBotStarts[UKismetMathLibrary::RandomInteger(TrackerBotStarts.Num())];
			FActorSpawnParameters SpawnParams;
			SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
			GetWorld()->SpawnActor<AActor>(Barrel, TargetStart->GetActorTransform(), SpawnParams);
			BarrelCount++;
		}
	}
}

void ATPSGameMode::SpawnWeapon()
{
	for (int i = 0; i < WeaponSpawnCount; i++)
	{
		if (WeaponStarts.Num() > 0 && Weapons.Num() > 0)
		{
			AActor* TargetStart = WeaponStarts[UKismetMathLibrary::RandomInteger(WeaponStarts.Num())];
			FActorSpawnParameters SpawnParams;
			SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::DontSpawnIfColliding;
			int32 RandomNumber = UKismetMathLibrary::RandomInteger(Weapons.Num());
			GetWorld()->SpawnActor<AActor>(Weapons[RandomNumber], TargetStart->GetActorTransform(), SpawnParams);
		}
	}
}

void ATPSGameMode::SpawnBuff()
{
	for (int i = 0; i < BuffSpawnCount; i++)
	{
		if (BuffStarts.Num() > 0 && Buffs.Num() > 0)
		{
			AActor* TargetStart = BuffStarts[UKismetMathLibrary::RandomInteger(BuffStarts.Num())];
			FActorSpawnParameters SpawnParams;
			SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::DontSpawnIfColliding;
			int32 RandomNumber = UKismetMathLibrary::RandomInteger(Buffs.Num());
			GetWorld()->SpawnActor<AActor>(Buffs[RandomNumber], TargetStart->GetActorTransform(), SpawnParams);
		}
	}
}
