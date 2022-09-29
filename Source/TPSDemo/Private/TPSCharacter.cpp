// Fill out your copyright notice in the Description page of Project Settings.


#include "TPSCharacter.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/PawnMovementComponent.h"
#include "TPSWeapon.h"
#include "TPSHealthComponent.h"
#include "Components/CapsuleComponent.h"
#include "Net/UnrealNetwork.h"
#include "Kismet/KismetMathLibrary.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "TPSGameMode.h"
#include "TPSCombatWeapon.h"

// Sets default values
ATPSCharacter::ATPSCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	bAlive = true;

	GetMovementComponent()->GetNavAgentPropertiesRef().bCanCrouch = true;

	SpringArmComp = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArmComp->bUsePawnControlRotation = true;
	SpringArmComp->SetupAttachment(RootComponent);

	CameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	CameraComp->SetupAttachment(SpringArmComp);

	HealthComp = CreateDefaultSubobject<UTPSHealthComponent>(TEXT("Health"));

	SpeedDefault = 600.0f;
	SpeedCrouchedDefault = 300.0f;
	AimFOV = 50.0f;
	AimInterpSpeed = 10.0f;
	DefeatCount = 0;
	CombatSpeedDefault = 0.8f;
	CombatSpeed = CombatSpeedDefault;
	bAim = false;
	bFire = false;
	bJumping = false;
	bInSpeedUp = false;
}

// Called when the game starts or when spawned
void ATPSCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	DefaultFOV = CameraComp->FieldOfView;

	GetCharacterMovement()->MaxWalkSpeed = SpeedDefault;
	GetCharacterMovement()->MaxWalkSpeedCrouched = SpeedCrouchedDefault;

	if (GetLocalRole() == ROLE_Authority)
	{
		EquipWeapon(StartWeaponType);
		EquipCombatWeapon(StartCombatWeaponType);
	}
}

void ATPSCharacter::MoveForward(float value)
{
	if (bAlive)
	{
		AddMovementInput(GetActorForwardVector(), value);
	}
}

void ATPSCharacter::MoveRight(float value)
{
	if (bAlive)
	{
		AddMovementInput(GetActorRightVector(), value);
	}
}

void ATPSCharacter::AddControllerPitchInput(float value)
{
	Super::AddControllerPitchInput(value);
	CalculateAimOffset();
}

void ATPSCharacter::CrouchBegin()
{
	if (bAlive)
	{
		Crouch();
	}
}

void ATPSCharacter::CrouchEnd()
{
	if (bAlive)
	{
		UnCrouch();
	}
}

void ATPSCharacter::ServerStopAnim_Implementation()
{
	NetMulticastStopAnim();
}

bool ATPSCharacter::ServerStopAnim_Validate()
{
	return true;
}

void ATPSCharacter::NetMulticastStopAnim_Implementation()
{
	StopAnimMontage();
}

bool ATPSCharacter::NetMulticastStopAnim_Validate()
{
	return true;
}

void ATPSCharacter::StopAnim()
{
	if (GetLocalRole() < ROLE_Authority)
	{
		ServerStopAnim();
		return;
	}
	NetMulticastStopAnim();
}

void ATPSCharacter::EquipWeapon(TSubclassOf<ATPSWeapon> WeaponType)
{
	if (bAlive)
	{
		if (Weapon)
		{
			StopAnim();
			Weapon->Destroy();
		}

		FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

		Weapon = GetWorld()->SpawnActor<ATPSWeapon>(WeaponType, FVector::ZeroVector, FRotator::ZeroRotator, SpawnParams);
		if (Weapon)
		{
			Weapon->SetOwner(this);
			Weapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, Weapon->WeaponAttachSocket);
			FireDeltaTime = Weapon->FireDeltaTime;
			AmmoMax = Weapon->AmmoMax;
			AmmoNow = AmmoMax;
			ReloadSpeed = Weapon->ReloadSpeed;
			RecoilForce = Weapon->RecoilForce;
		}
	}
}

void ATPSCharacter::EquipCombatWeapon(TSubclassOf<ATPSCombatWeapon> WeaponType)
{
	if (bAlive)
	{
		if (CombatWeapon)
		{
			StopAnim();
			CombatWeapon->Destroy();
		}
		if (CombatWeaponCommon)
		{
			CombatWeaponCommon->Destroy();
		}

		FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

		CombatWeapon = GetWorld()->SpawnActor<ATPSCombatWeapon>(WeaponType, FVector::ZeroVector, FRotator::ZeroRotator, SpawnParams);
		CombatWeaponCommon = GetWorld()->SpawnActor<ATPSCombatWeapon>(WeaponType, FVector::ZeroVector, FRotator::ZeroRotator, SpawnParams);
		if (CombatWeapon && CombatWeaponCommon)
		{
			CombatWeapon->SetOwner(this);
			CombatWeapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, CombatWeapon->AttachSocket);
			CombatWeaponCommon->SetOwner(this);
			CombatWeaponCommon->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, CombatWeaponCommon->CommonAttachSocket);
			CombatWeapon->Weapon->SetVisibility(false);
		}
	}
}

void ATPSCharacter::ServerAimBegin_Implementation()
{
	AimBegin();
}

bool ATPSCharacter::ServerAimBegin_Validate()
{
	return true;
}

void ATPSCharacter::AimBegin()
{
	if (bAlive)
	{
		if (GetLocalRole() < ROLE_Authority)
		{
			ServerAimBegin();
		}
		bAim = true;
	}
}

void ATPSCharacter::ServerAimEnd_Implementation()
{
	AimEnd();
}

bool ATPSCharacter::ServerAimEnd_Validate()
{
	return true;
}

void ATPSCharacter::AimEnd()
{
	if (bAlive)
	{
		if (GetLocalRole() < ROLE_Authority)
		{
			ServerAimEnd();
		}
		bAim = false;
	}
}

void ATPSCharacter::ServerCalculateAimOffset_Implementation()
{
	CalculateAimOffset();
}

bool ATPSCharacter::ServerCalculateAimOffset_Validate()
{
	return true;
}

void ATPSCharacter::CalculateAimOffset()
{
	if (bAlive)
	{
		if (GetLocalRole() < ROLE_Authority)
		{
			ServerCalculateAimOffset();
		}
		AimOffset = FMath::Clamp(UKismetMathLibrary::NormalizedDeltaRotator(GetControlRotation(), GetActorRotation()).Pitch, -90.0f, 90.0f);
	}
}

void ATPSCharacter::FireBegin()
{
	if (bAlive)
	{
		if (GetCurrentMontage())
		{
			StopAnim();
			return;
		}
		bFire = true;
	}
}

void ATPSCharacter::FireEnd()
{
	if (bAlive)
	{
		bFire = false;
	}
}

void ATPSCharacter::Reload()
{
	if (Weapon)
	{
		AmmoNow = AmmoMax;
	}
}

void ATPSCharacter::ServerJumpBegin_Implementation()
{
	JumpBegin();
}

bool ATPSCharacter::ServerJumpBegin_Validate()
{
	return true;
}

void ATPSCharacter::JumpBegin()
{
	if (bAlive)
	{
		if (GetLocalRole() < ROLE_Authority)
		{
			ServerJumpBegin();
			Super::Jump();
			return;
		}
		Super::Jump();
		bJumping = true;
	}
}

void ATPSCharacter::ServerJumpEnd_Implementation()
{
	JumpEnd();
}

bool ATPSCharacter::ServerJumpEnd_Validate()
{
	return true;
}

void ATPSCharacter::JumpEnd()
{
	if (bAlive)
	{
		if (GetLocalRole() < ROLE_Authority)
		{
			ServerAimEnd();
			return;
		}
		bJumping = false;
	}
}

void ATPSCharacter::ServerSpeedUpBegin_Implementation()
{
	NetMulticastSpeedUpBegin();
}

bool ATPSCharacter::ServerSpeedUpBegin_Validate()
{
	return true;
}

void ATPSCharacter::NetMulticastSpeedUpBegin_Implementation()
{
	if (!bInSpeedUp)
	{
		bInSpeedUp = true;
		if (SpeedUpSound)
		{
			UGameplayStatics::SpawnSoundAttached(SpeedUpSound, GetMesh());
		}
		GetCharacterMovement()->MaxWalkSpeed = SpeedDefault * 2;
		GetCharacterMovement()->MaxWalkSpeedCrouched = SpeedCrouchedDefault * 2;
		CombatSpeed = CombatSpeedDefault * 2;
		FTimerHandle TimerHandle;
		GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &ATPSCharacter::SpeedUpEnd, 10.0f, false);
	}
}

bool ATPSCharacter::NetMulticastSpeedUpBegin_Validate()
{
	return true;
}

void ATPSCharacter::SpeedUpBegin()
{
	if (!bInSpeedUp)
	{
		if (GetLocalRole() < ROLE_Authority)
		{
			ServerSpeedUpBegin();
			return;
		}
		NetMulticastSpeedUpBegin();
	}
}

void ATPSCharacter::SpeedUpEnd()
{
	bInSpeedUp = false;
	GetCharacterMovement()->MaxWalkSpeed = SpeedDefault;
	GetCharacterMovement()->MaxWalkSpeedCrouched = SpeedCrouchedDefault;
	CombatSpeed = CombatSpeedDefault;
}

void ATPSCharacter::ServerSpeedDownBegin_Implementation()
{
	SpeedDownBegin();
}

bool ATPSCharacter::ServerSpeedDownBegin_Validate()
{
	return true;
}

void ATPSCharacter::SpeedDownBegin()
{
	if (GetLocalRole() < ROLE_Authority)
	{
		ServerSpeedDownBegin();
		return;
	}
	ATPSGameMode* GameMode = Cast<ATPSGameMode>(UGameplayStatics::GetGameMode(this));
	if (GameMode && !GameMode->bInSpeedDown)
	{
		GameMode->bInSpeedDown = true;
		UGameplayStatics::SetGlobalTimeDilation(this, 0.5f);
		FTimerHandle TimerHandle;
		GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &ATPSCharacter::SpeedDownEnd, 3.0f, false);
	}
}

void ATPSCharacter::SpeedDownEnd()
{
	Cast<ATPSGameMode>(UGameplayStatics::GetGameMode(this))->bInSpeedDown = false;
	UGameplayStatics::SetGlobalTimeDilation(this, 1.0f);
}

void ATPSCharacter::ServerInvisibilityBegin_Implementation()
{
	NetMulticastInvisibilityBegin();
}

bool ATPSCharacter::ServerInvisibilityBegin_Validate()
{
	return true;
}

void ATPSCharacter::NetMulticastInvisibilityBegin_Implementation()
{
	if (GetMesh()->IsVisible())
	{
		if (InvisibilitySound)
		{
			UGameplayStatics::SpawnSoundAttached(InvisibilitySound, GetMesh());
		}
		GetMesh()->SetVisibility(false);
		FTimerHandle TimerHandle;
		GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &ATPSCharacter::InvisibilityEnd, 5.0f, false);
	}
}

bool ATPSCharacter::NetMulticastInvisibilityBegin_Validate()
{
	return true;
}

void ATPSCharacter::InvisibilityBegin()
{
	if (GetMesh()->IsVisible())
	{
		if (GetLocalRole() < ROLE_Authority)
		{
			ServerInvisibilityBegin();
			return;
		}
		NetMulticastInvisibilityBegin();
	}
}

void ATPSCharacter::InvisibilityEnd()
{
	GetMesh()->SetVisibility(true);
}

// Called every frame
void ATPSCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	float TargetFOV = bAim ? AimFOV : DefaultFOV;
	float NewFOV = FMath::FInterpTo(CameraComp->FieldOfView, TargetFOV, DeltaTime, AimInterpSpeed);
	CameraComp->SetFieldOfView(NewFOV);

	LastShootTime += DeltaTime;
	if (bFire && LastShootTime >= FireDeltaTime && AmmoNow > 0)
	{
		LastShootTime = 0;
		Weapon->Fire();
		AmmoNow--;
		AddControllerPitchInput(RecoilForce * -1);
	}
}

// Called to bind functionality to input
void ATPSCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis("MoveForward", this, &ATPSCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ATPSCharacter::MoveRight);

	PlayerInputComponent->BindAxis("LookUp", this, &ATPSCharacter::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("TurnRight", this, &ATPSCharacter::AddControllerYawInput);

	PlayerInputComponent->BindAction("Crouch", IE_Pressed, this, &ATPSCharacter::CrouchBegin);
	PlayerInputComponent->BindAction("Crouch", IE_Released, this, &ATPSCharacter::CrouchEnd);
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ATPSCharacter::JumpBegin);

	PlayerInputComponent->BindAction("Aim", IE_Pressed, this, &ATPSCharacter::AimBegin);
	PlayerInputComponent->BindAction("Aim", IE_Released, this, &ATPSCharacter::AimEnd);
	PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &ATPSCharacter::FireBegin);
	PlayerInputComponent->BindAction("Fire", IE_Released, this, &ATPSCharacter::FireEnd);
}

FVector ATPSCharacter::GetPawnViewLocation() const
{
	if (CameraComp)
	{
		return CameraComp->GetComponentLocation();
	}
	return Super::GetPawnViewLocation();
}

void ATPSCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ATPSCharacter, bAlive);
	DOREPLIFETIME(ATPSCharacter, DefeatCount);
	DOREPLIFETIME(ATPSCharacter, Weapon);
	DOREPLIFETIME(ATPSCharacter, CombatWeapon);
	DOREPLIFETIME(ATPSCharacter, CombatWeaponCommon);
	DOREPLIFETIME(ATPSCharacter, bAim);
	DOREPLIFETIME(ATPSCharacter, AimOffset);
	DOREPLIFETIME(ATPSCharacter, bJumping);
	DOREPLIFETIME(ATPSCharacter, AmmoMax);
	DOREPLIFETIME(ATPSCharacter, AmmoNow);
	DOREPLIFETIME(ATPSCharacter, RecoilForce);
	DOREPLIFETIME(ATPSCharacter, ReloadSpeed);
	DOREPLIFETIME(ATPSCharacter, FireDeltaTime);
	DOREPLIFETIME(ATPSCharacter, CombatSpeed);
}
