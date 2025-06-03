// Copyright Patrick Haubner


#include "Actor/AuraProjectile.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"

AAuraProjectile::AAuraProjectile()
{
	PrimaryActorTick.bCanEverTick = false;

	Sphere = CreateDefaultSubobject<USphereComponent>("Sphere");
	SetRootComponent(Sphere); // Set the sphere as the root component of the actor
	Sphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly); // Enable collision for overlap events
	Sphere->SetCollisionResponseToAllChannels(ECR_Ignore); // Ignore all channels by default
	Sphere->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Overlap); // Enable overlap for dynamic world objects
	Sphere->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Overlap); // Enable overlap for static world objects
	Sphere->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap); // Enable overlap for pawns

	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>("ProjectileMovement");
	ProjectileMovement->InitialSpeed = 550.f; // Set the initial speed of the projectile
	ProjectileMovement->MaxSpeed = 550.f;
	ProjectileMovement->ProjectileGravityScale = 0.f; // Disable gravity for the projectile
}

void AAuraProjectile::BeginPlay()
{
	Super::BeginPlay();
	Sphere->OnComponentBeginOverlap.AddDynamic(this, &AAuraProjectile::OnSphereOverlap); // Register the overlap event
}

void AAuraProjectile::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, 
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	
}