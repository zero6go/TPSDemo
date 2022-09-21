// Fill out your copyright notice in the Description page of Project Settings.


#include "TPSTrackerBot.h"
#include "Components/StaticMeshComponent.h"
#include "TPSHealthComponent.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Character.h"
#include "NavigationPath.h"
#include "NavigationSystem.h"
#include "GameFramework/FloatingPawnMovement.h"
#include "TPSCharacter.h"
#include "Kismet/KismetMathLibrary.h"
#include "DrawDebugHelpers.h"

ATPSTrackerBot::ATPSTrackerBot()
{
	MeshComp->SetCanEverAffectNavigation(false);
	MeshComp->SetSimulatePhysics(true);

	HealthComp->SetHealthMax(30.0f);

	bUseVelocityChange = true;
	MovementForce = 6.0f;
	RequiredDistanceToTarget = 200.0f;
	TrackTimeCount = 0.0f;
}

void ATPSTrackerBot::BeginPlay()
{
	Super::BeginPlay();

	if (GetLocalRole() == ROLE_Authority)
	{
		SetRandomPlayer();
		NextPathPoint = GetNextPathPoint();
	}
}

void ATPSTrackerBot::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (GetLocalRole() == ROLE_Authority)
	{
		TrackTimeCount += DeltaTime;
		float DistanceToTarget = (NextPathPoint - GetActorLocation()).Size();
		if (TrackTimeCount <= 3.0f && DistanceToTarget > RequiredDistanceToTarget)
		{
			FVector ForceDirection = NextPathPoint - GetActorLocation();
			ForceDirection.Normalize();
			ForceDirection *= MovementForce;
			MeshComp->AddImpulse(ForceDirection, NAME_None, bUseVelocityChange);
		}
		else
		{
			NextPathPoint = GetNextPathPoint();
			TrackTimeCount = 0.0f;
		}
	}
}

FVector ATPSTrackerBot::GetNextPathPoint()
{
	if (TargetPlayer && TargetPlayer->bAlive)
	{
		UNavigationPath* NavPath = UNavigationSystemV1::FindPathToActorSynchronously(this, GetActorLocation(), TargetPlayer);
		if (NavPath->PathPoints.Num() > 1)
		{
			return NavPath->PathPoints[1];
		}
	}
	else
	{
		SetRandomPlayer();
		if (TargetPlayer && TargetPlayer->bAlive)
		{
			UNavigationPath* NavPath = UNavigationSystemV1::FindPathToActorSynchronously(this, GetActorLocation(), TargetPlayer);
			if (NavPath->PathPoints.Num() > 1)
			{
				return NavPath->PathPoints[1];
			}
		}
	}
	return GetActorLocation();
}

void ATPSTrackerBot::SetRandomPlayer()
{
	if (PlayerType)
	{
		UGameplayStatics::GetAllActorsOfClass(this, PlayerType, Players);
		if (Players.Num())
		{
			TargetPlayer = Cast<ATPSCharacter>(Players[UKismetMathLibrary::RandomInteger(Players.Num())]);
		}
	}
}
