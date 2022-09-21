// Fill out your copyright notice in the Description page of Project Settings.


#include "TPSSpectatorPawn.h"
#include "GameFramework/GameModeBase.h"
#include "TPSCharacter.h"
#include "Kismet/GameplayStatics.h"

ATPSSpectatorPawn::ATPSSpectatorPawn()
{
	PrimaryActorTick.bCanEverTick = true;

	ReviveTime = 5.0f;
	ReviveTimeRemain = ReviveTime;
	bAddedUI = false;
}

void ATPSSpectatorPawn::Tick(float DeltaSeconds)
{
	ReviveTimeRemain = FMath::Clamp(ReviveTimeRemain - DeltaSeconds, 0.0f, ReviveTime);
	if (!bAddedUI && IsLocallyControlled())
	{
		bAddedUI = true;
		AddUI();
	}
}

void ATPSSpectatorPawn::ServerRevive_Implementation()
{
	Revive();
}

bool ATPSSpectatorPawn::ServerRevive_Validate()
{
	return true;
}

void ATPSSpectatorPawn::Revive()
{
	if (ReviveTimeRemain <= 0.0f)
	{
		if (GetLocalRole() < ROLE_Authority)
		{
			ServerRevive();
			return;
		}
		if (UWorld* World = GetWorld())
		{
			if (RevivePlaces.Num() > 0)
			{
				FActorSpawnParameters SpawnParams;
				SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

				int32 RandNum = FMath::Rand() % RevivePlaces.Num();
				AActor* RevivePlace = RevivePlaces[RandNum];
				ATPSCharacter* RevivedPlayer = GetWorld()->SpawnActor<ATPSCharacter>(RevivedPlayerType, RevivePlace->GetActorTransform(), SpawnParams);
				AController* PlayerController = GetController();
				PlayerController->Possess(RevivedPlayer);
				Destroy();
			}
		}
	}
}

void ATPSSpectatorPawn::SetupPlayerInputComponent(UInputComponent* InInputComponent)
{
	Super::SetupPlayerInputComponent(InInputComponent);

	InInputComponent->BindAction("Revive", IE_Pressed, this, &ATPSSpectatorPawn::Revive);
}
