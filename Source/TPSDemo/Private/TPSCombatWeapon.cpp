// Fill out your copyright notice in the Description page of Project Settings.


#include "TPSCombatWeapon.h"

// Sets default values
ATPSCombatWeapon::ATPSCombatWeapon()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SetReplicates(true);

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	RootComponent = Root;

	Weapon = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	Weapon->SetupAttachment(RootComponent);
	Weapon->SetCollisionProfileName("NoCollision");
	Weapon->SetIsReplicated(true);

	Trace = CreateDefaultSubobject<USceneComponent>(TEXT("Trace"));
	Trace->SetupAttachment(Weapon);

	CombatDamage = 50.0f;
	CombatRadius = 120.0f;
}

// Called when the game starts or when spawned
void ATPSCombatWeapon::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ATPSCombatWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

