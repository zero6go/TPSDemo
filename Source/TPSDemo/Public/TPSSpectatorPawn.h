// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SpectatorPawn.h"
#include "TPSSpectatorPawn.generated.h"

/**
 * 
 */
UCLASS()
class TPSDEMO_API ATPSSpectatorPawn : public ASpectatorPawn
{
	GENERATED_BODY()

public:
	ATPSSpectatorPawn();

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Revive")
	float ReviveTime;

	UPROPERTY(BlueprintReadOnly)
	float ReviveTimeRemain;

	bool bAddedUI;

	UPROPERTY(BlueprintReadWrite)
	TArray<AActor*> RevivePlaces;

	virtual void Tick(float DeltaSeconds) override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<class ATPSCharacter> RevivedPlayerType;

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerRevive();

	void Revive();

	UFUNCTION(BlueprintImplementableEvent)
	void AddUI();

	virtual void SetupPlayerInputComponent(UInputComponent* InInputComponent) override;
};
