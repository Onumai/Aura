// Copyright Patrick Haubner


#include "Player/AuraPlayerState.h"
#include <AbilitySystem/AuraAbilitySystemComponent.h>
#include <AbilitySystem/AuraAttributeSet.h>
#include <Net/UnrealNetwork.h>


AAuraPlayerState::AAuraPlayerState()
{
	AbilitySystemComponent = CreateDefaultSubobject<UAuraAbilitySystemComponent>("AbilitySystemComponent"); // Creates an instance of the AuraAbilitySystemComponent class, which is a subclass of UAbilitySystemComponent.
	AbilitySystemComponent->SetIsReplicated(true); // Enables replication for this property, ensuring it stays synchronized across server and clients.
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Mixed); // Set the replication mode to mixed, which means that the server will replicate the effects to clients, but clients will also be able to apply effects to themselves without waiting for the server to do so.


	AttributeSet = CreateDefaultSubobject<UAuraAttributeSet>("AttributeSet");


	//Replication settings for the player state 
	NetUpdateFrequency = 100.f; // Set the frequency of network updates to 100 times per second

}

void AAuraPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AAuraPlayerState, Level);


}

UAbilitySystemComponent* AAuraPlayerState::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

void AAuraPlayerState::OnRep_Level(int32 OldLevel)
{

}
