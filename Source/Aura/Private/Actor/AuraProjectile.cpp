// Copyright Patrick Haubner


#include "Actor/AuraProjectile.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "NiagaraFunctionLibrary.h"
#include "Components/AudioComponent.h"
#include "Aura/Aura.h"
#include <AbilitySystemBlueprintLibrary.h>
#include "AbilitySystemComponent.h"

AAuraProjectile::AAuraProjectile()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true; // Enable replication for network play

	Sphere = CreateDefaultSubobject<USphereComponent>("Sphere");
	SetRootComponent(Sphere); // Set the sphere as the root component of the actor
	Sphere->SetCollisionObjectType(ECC_Projectile); // Custom ObjectType in Aura.h
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
	SetLifeSpan(LifeSpan);
	Sphere->OnComponentBeginOverlap.AddDynamic(this, &AAuraProjectile::OnSphereOverlap); // Register the overlap event
	
	LoopingSoundComponent = UGameplayStatics::SpawnSoundAttached(LoopingSound, GetRootComponent());
}

void AAuraProjectile::Destroyed()
{
	if (!bHit && !HasAuthority())
	{
		UGameplayStatics::PlaySoundAtLocation(this, ImpactSound, GetActorLocation(), FRotator::ZeroRotator);
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, ImpactEffect, GetActorLocation());
		LoopingSoundComponent->Stop();
	}
	Super::Destroyed();
}

void AAuraProjectile::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, 
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	UGameplayStatics::PlaySoundAtLocation(this, ImpactSound, GetActorLocation(), FRotator::ZeroRotator);
	UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, ImpactEffect, GetActorLocation());
	LoopingSoundComponent->Stop();
	
	if (HasAuthority())
	{
		if (UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(OtherActor))
		{
			// We get the ASC from the OtherActor and apply the damage effect to itself after overlap, if the other actor has an ASC
			TargetASC->ApplyGameplayEffectSpecToSelf(*DamageEffectSpecHandle.Data.Get()); // Get the Spec from SpecHandle
		}
		
		
		
		Destroy();
	}
	else
	{
		bHit = true;
	}
}