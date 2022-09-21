// Fill out your copyright notice in the Description page of Project Settings.


#include "TPSPawn.h"
#include "Components/StaticMeshComponent.h"
#include "TPSHealthComponent.h"

// Sets default values
ATPSPawn::ATPSPawn()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SetReplicates(true);

	HealthComp = CreateDefaultSubobject<UTPSHealthComponent>(TEXT("Health"));

	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	MeshComp->SetIsReplicated(true);
	RootComponent = MeshComp;

	bAlive = true;
}

// Called when the game starts or when spawned
void ATPSPawn::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ATPSPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ATPSPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

