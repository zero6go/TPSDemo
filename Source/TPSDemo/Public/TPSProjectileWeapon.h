// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "TPSWeapon.h"
#include "TPSProjectileWeapon.generated.h"

/**
 * 
 */
UCLASS()
class TPSDEMO_API ATPSProjectileWeapon : public ATPSWeapon
{
	GENERATED_BODY()
	
protected:
	void Fire();
	
	void PlayFireEffects();

	UPROPERTY(EditDefaultsOnly, Category="ProjetileWeapon")
	TSubclassOf<AActor> ProjectileClass;
	
};
