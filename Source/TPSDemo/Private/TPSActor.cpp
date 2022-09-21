// Fill out your copyright notice in the Description page of Project Settings.


#include "TPSActor.h"
#include "Components/StaticMeshComponent.h"
#include "TPSHealthComponent.h"

// Sets default values
ATPSActor::ATPSActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SetReplicates(true);

	HealthComp = CreateDefaultSubobject<UTPSHealthComponent>(TEXT("Health"));

	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	MeshComp->SetIsReplicated(true);
	RootComponent = MeshComp;

	bAlive = true;
}

// Called when the game starts or when spawned
void ATPSActor::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ATPSActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}