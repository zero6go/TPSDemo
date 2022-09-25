// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "TPSDemo/TPSDemoGameModeBase.h"
#include "TPSGameMode.generated.h"

/**
 * 
 */
UCLASS()
class TPSDEMO_API ATPSGameMode : public ATPSDemoGameModeBase
{
	GENERATED_BODY()
	
public:
	bool bInSpeedDown;

	UPROPERTY(BlueprintReadWrite)
	int32 TrackerBotCount;
	UPROPERTY(BlueprintReadWrite)
	int32 BarrelCount;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "TrackerBot")
	float TrackerBotSpawnTime;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "TrackerBot")
	TSubclassOf<AActor> TrackerBotStart;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "TrackerBot")
	TSubclassOf<AActor> TrackerBot;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "TrackerBot")
	TSubclassOf<AActor> Barrel;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	int32 WeaponSpawnCount;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	float WeaponSpawnTime;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	TSubclassOf<AActor> WeaponStart;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	TArray<TSubclassOf<AActor>> Weapons;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Buff")
	int32 BuffSpawnCount;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Buff")
	float BuffSpawnTime;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Buff")
	TSubclassOf<AActor> BuffStart;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Buff")
	TArray<TSubclassOf<AActor>> Buffs;

private:
	TArray<AActor*> TrackerBotStarts;
	TArray<AActor*> WeaponStarts;
	TArray<AActor*> BuffStarts;

public:
	ATPSGameMode();

	virtual void BeginPlay() override;

	void SpawnTrackerBotAndBarrel();

	void SpawnWeapon();

	void SpawnBuff();
};
