// Copyright Patrick Haubner


#include "Character/AuraEnemy.h"
#include <Aura/Aura.h>
#include <AbilitySystem/AuraAbilitySystemComponent.h>
#include <AbilitySystem/AuraAttributeSet.h>

AAuraEnemy::AAuraEnemy()
{
	GetMesh()->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);

	AbilitySystemComponent = CreateDefaultSubobject<UAuraAbilitySystemComponent>("AbilitySystemComponent"); // Creates an instance of the AuraAbilitySystemComponent class, which is a subclass of UAbilitySystemComponent.
	AbilitySystemComponent->SetIsReplicated(true); // Enables replication for this property, ensuring it stays synchronized across server and clients.
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Minimal); // Set the replication mode to mixed, which means that the server will replicate the effects to clients, but clients will also be able to apply effects to themselves without waiting for the server to do so.


	AttributeSet = CreateDefaultSubobject<UAuraAttributeSet>("AttributeSet");
}

void AAuraEnemy::HighlightActor()
{
	GetMesh()->SetRenderCustomDepth(true); // Enable custom depth rendering for the mesh.
	GetMesh()->SetCustomDepthStencilValue(CUSTOM_DEPTH_RED); // Set the custom depth stencil value.
	Weapon->SetRenderCustomDepth(true); // Enable custom depth rendering for the weapon.
	Weapon->SetCustomDepthStencilValue(CUSTOM_DEPTH_RED);
}

void AAuraEnemy::UnHighlightActor()
{
	GetMesh()->SetRenderCustomDepth(false);
	Weapon->SetRenderCustomDepth(false);
}

int32 AAuraEnemy::GetPlayerLevel()
{
	return Level;
}

void AAuraEnemy::BeginPlay()
{
	Super::BeginPlay();

	InitAbilityActorInfo();

}

void AAuraEnemy::InitAbilityActorInfo()
{
	AbilitySystemComponent->InitAbilityActorInfo(this, this); // Initialize the ability system component with the actor info.

	Cast<UAuraAbilitySystemComponent>(AbilitySystemComponent)->AbilityActorInfoSet();

	InitializeDefaultAttributes();
}
