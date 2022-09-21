// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "TPSHealthComponent.generated.h"


UCLASS( ClassGroup=(TPS), meta=(BlueprintSpawnableComponent) )
class TPSDEMO_API UTPSHealthComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UTPSHealthComponent();

	UFUNCTION(BlueprintCallable)
	void SetHealthMax(float NewHealthMax);

	UFUNCTION(BlueprintCallable)
	void RecoverLife(float AddHealthCount);

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Replicated, Category = "Health")
	float HealthNow;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Replicated, Category = "Health")
	float HealthMax;

	UFUNCTION()
	void HandleTakeAnyDamage(AActor* DamagedActor, float Damage, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser);
	
	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const;
};
