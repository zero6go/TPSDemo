// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "TPSCharacter.generated.h"

UCLASS()
class TPSDEMO_API ATPSCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ATPSCharacter();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class USpringArmComponent* SpringArmComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class UCameraComponent* CameraComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class UTPSHealthComponent* HealthComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Replicated, Category = "Score")
	int32 DefeatCount;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Replicated, Category = "Character")
	bool bAlive;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character")
	float SpeedDefault;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character")
	float SpeedCrouchedDefault;

	bool bInSpeedUp;

	UPROPERTY(EditDefaultsOnly, Category = "Character")
	float AimFOV;

	float DefaultFOV;

	float AimInterpSpeed;

	UPROPERTY(BlueprintReadOnly, Replicated)
	float AimOffset;

	UPROPERTY(BlueprintReadWrite, Replicated, Category = "Character")
	bool bAim;

	UPROPERTY(BlueprintReadWrite, Replicated)
	bool bJumping;

	UPROPERTY(BlueprintReadWrite, Category = "Character")
	bool bFire;

	UPROPERTY(Replicated)
	float FireDeltaTime;

	float LastShootTime;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Replicated, Category = "Weapon")
	class ATPSWeapon* Weapon;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
	TSubclassOf<ATPSWeapon> StartWeaponType;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Replicated, Category = "CombatWeapon")
	class ATPSCombatWeapon* CombatWeapon;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CombatWeapon")
	TSubclassOf<ATPSCombatWeapon> StartCombatWeaponType;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Replicated, Category = "Weapon")
	int32 AmmoMax;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Replicated, Category = "Weapon")
	int32 AmmoNow;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Replicated, Category = "Weapon")
	float ReloadSpeed;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Replicated, Category = "Weapon")
	float RecoilForce;

	UPROPERTY(BlueprintReadOnly, Replicated)
	float CombatSpeed;
	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	float CombatSpeedDefault;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Sound")
	USoundBase* SpeedUpSound;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Sound")
	USoundBase* InvisibilitySound;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Sound")
	USoundBase* SpeedDownSound;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	void MoveForward(float value);

	void MoveRight(float value);

	void AddControllerPitchInput(float value);

	void CrouchBegin();

	void CrouchEnd();

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerStopAnim();
	UFUNCTION(NetMulticast, Reliable, WithValidation)
	void NetMulticastStopAnim();
	UFUNCTION(BlueprintCallable)
	void StopAnim();

	UFUNCTION(BlueprintCallable)
	void EquipWeapon(TSubclassOf<ATPSWeapon> WeaponType);
	UFUNCTION(BlueprintCallable)
	void EquipCombatWeapon(TSubclassOf<ATPSCombatWeapon> WeaponType);

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerAimBegin();
	void AimBegin();

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerAimEnd();
	void AimEnd();

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerCalculateAimOffset();
	void CalculateAimOffset();

	void FireBegin();

	void FireEnd();

	UFUNCTION(BlueprintCallable)
	void Reload();
	
	UFUNCTION(Server, Reliable, WithValidation)
	void ServerJumpBegin();
	void JumpBegin();

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerJumpEnd();
	UFUNCTION(BlueprintCallable)
	void JumpEnd();
	
	UFUNCTION(Server, Reliable, WithValidation)
	void ServerSpeedUpBegin();
	UFUNCTION(NetMulticast, Reliable, WithValidation)
	void NetMulticastSpeedUpBegin();
	UFUNCTION(BlueprintCallable)
	void SpeedUpBegin();
	void SpeedUpEnd();

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerSpeedDownBegin();
	UFUNCTION(NetMulticast, Reliable, WithValidation)
	void NetMulticastSpeedDownBegin();
	UFUNCTION(BlueprintCallable)
	void SpeedDownBegin();
	void SpeedDownEnd();

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerInvisibilityBegin();
	UFUNCTION(NetMulticast, Reliable, WithValidation)
	void NetMulticastInvisibilityBegin();
	UFUNCTION(BlueprintCallable)
	void InvisibilityBegin();
	void InvisibilityEnd();

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual FVector GetPawnViewLocation() const override;

	UFUNCTION(BlueprintImplementableEvent)
	void Die();

	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};
