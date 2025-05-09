// Copyright Patrick Haubner


#include "Character/AuraCharacterBase.h"
#include <AbilitySystem/AuraAttributeSet.h>
#include <AbilitySystem/AuraAbilitySystemComponent.h>


AAuraCharacterBase::AAuraCharacterBase()
{
	PrimaryActorTick.bCanEverTick = false;

	Weapon = CreateDefaultSubobject<USkeletalMeshComponent>("Weapon");
	Weapon->SetupAttachment(GetMesh(), FName("WeaponHandSocket"));
	Weapon->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	AbilitySystemComponent = CreateDefaultSubobject<UAuraAbilitySystemComponent>("AbilitySystemComponent"); // Creates an instance of the AuraAbilitySystemComponent class, which is a subclass of UAbilitySystemComponent.
	AbilitySystemComponent->SetIsReplicated(true); // Enables replication for this property, ensuring it stays synchronized across server and clients.

	AttributeSet = CreateDefaultSubobject<UAuraAttributeSet>("AttributeSet");

}

UAbilitySystemComponent* AAuraCharacterBase::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent; // Returns the ability system component for this character
}

void AAuraCharacterBase::BeginPlay()
{
	Super::BeginPlay();
	
}


