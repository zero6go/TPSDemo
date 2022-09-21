// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "TPSPawn.h"
#include "TPSTrackerBot.generated.h"

/**
 * 
 */
UCLASS()
class TPSDEMO_API ATPSTrackerBot : public ATPSPawn
{
	GENERATED_BODY()
	
public:
	ATPSTrackerBot();

	UPROPERTY(EditDefaultsOnly, Category = "TrackerBot")
	float MovementForce;

	UPROPERTY(EditDefaultsOnly, Category = "TrackerBot")
	bool bUseVelocityChange;

	UPROPERTY(EditDefaultsOnly, Category = "TrackerBot")
	float RequiredDistanceToTarget;

	UPROPERTY(EditDefaultsOnly, Category = "TrackerBot")
	TSubclassOf<class ATPSCharacter> PlayerType;

private:
	FVector NextPathPoint;

	class ATPSCharacter* TargetPlayer;
	TArray<AActor*> Players;

	float TrackTimeCount;

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;

	FVector GetNextPathPoint();

	void SetRandomPlayer();
};
