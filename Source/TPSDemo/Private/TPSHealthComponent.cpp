// Fill out your copyright notice in the Description page of Project Settings.


#include "TPSHealthComponent.h"
#include "TPSCharacter.h"
#include "TPSActor.h"
#include "TPSPawn.h"
#include "Net/UnrealNetwork.h"

// Sets default values for this component's properties
UTPSHealthComponent::UTPSHealthComponent()
{
	HealthMax = 100.0f;
}

void UTPSHealthComponent::SetHealthMax(float NewHealthMax)
{
	HealthMax = NewHealthMax;
}

void UTPSHealthComponent::RecoverLife(float AddHealthCount)
{
	HealthNow = FMath::Clamp(HealthNow + AddHealthCount, 0.0f, HealthMax);
}

// Called when the game starts
void UTPSHealthComponent::BeginPlay()
{
	Super::BeginPlay();

	SetIsReplicated(true);
	HealthNow = HealthMax;

	if (GetOwnerRole() == ROLE_Authority)
	{
		AActor* Owner = GetOwner();
		if (Owner)
		{
			Owner->OnTakeAnyDamage.AddDynamic(this, &UTPSHealthComponent::HandleTakeAnyDamage);
		}
	}
}

void UTPSHealthComponent::HandleTakeAnyDamage(AActor* DamagedActor, float Damage, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser)
{
	if (Damage <= 0)
	{
		return;
	}
	HealthNow = FMath::Clamp(HealthNow - Damage, 0.0f, HealthMax);
	if (Cast<ATPSCharacter>(GetOwner()))
	{
		ATPSCharacter* Character = Cast<ATPSCharacter>(GetOwner());
		if (HealthNow == 0 && Character->bAlive)
		{
			Character->bAlive = false;
			Character->Die();
			if (InstigatedBy)
			{
				ATPSCharacter* Killer = Cast<ATPSCharacter>(InstigatedBy->GetPawn());
				if (Killer)
				{
					Killer->DefeatCount++;
					UTPSHealthComponent* KillerHealth = Killer->HealthComp;
					float HealthPercent = KillerHealth->HealthNow / KillerHealth->HealthMax;
					KillerHealth->SetHealthMax(KillerHealth->HealthMax + 10.0f);
					KillerHealth->RecoverLife(KillerHealth->HealthMax * HealthPercent - KillerHealth->HealthNow);
				}
			}
		}
	}
	else if(Cast<ATPSActor>(GetOwner()))
	{
		ATPSActor* Actor = Cast<ATPSActor>(GetOwner());
		if (HealthNow == 0 && Actor->bAlive)
		{
			Actor->bAlive = false;
			Actor->Die();
		}
	}
	else if (Cast<ATPSPawn>(GetOwner()))
	{
		ATPSPawn* Pawn = Cast<ATPSPawn>(GetOwner());
		if (HealthNow == 0 && Pawn->bAlive)
		{
			Pawn->bAlive = false;
			Pawn->Die();
			if (InstigatedBy)
			{
				ATPSCharacter* Killer = Cast<ATPSCharacter>(InstigatedBy->GetPawn());
				if (Killer)
				{
					Killer->DefeatCount++;
					UTPSHealthComponent* KillerHealth = Killer->HealthComp;
					float HealthPercent = KillerHealth->HealthNow / KillerHealth->HealthMax;
					KillerHealth->SetHealthMax(KillerHealth->HealthMax + 5.0f);
					KillerHealth->RecoverLife(KillerHealth->HealthMax * HealthPercent - KillerHealth->HealthNow);
				}
			}
		}
	}
}

void UTPSHealthComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UTPSHealthComponent, HealthNow);
	DOREPLIFETIME(UTPSHealthComponent, HealthMax);
}
