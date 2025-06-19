// Copyright Patrick Haubner


#include "Character/AuraEnemy.h"
#include "Aura/Aura.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "AbilitySystem/AuraAttributeSet.h"
#include "Components/WidgetComponent.h"
#include "UI/Widget/AuraUserWidget.h"
#include "AbilitySystem/AuraAbilitySystemLibrary.h"
#include "AuraGameplayTags.h"
#include "GameFramework/CharacterMovementComponent.h"

AAuraEnemy::AAuraEnemy()
{
	GetMesh()->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);

	AbilitySystemComponent = CreateDefaultSubobject<UAuraAbilitySystemComponent>("AbilitySystemComponent"); // Creates an instance of the AuraAbilitySystemComponent class, which is a subclass of UAbilitySystemComponent.
	AbilitySystemComponent->SetIsReplicated(true); // Enables replication for this property, ensuring it stays synchronized across server and clients.
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Minimal); // Set the replication mode to mixed, which means that the server will replicate the effects to clients, but clients will also be able to apply effects to themselves without waiting for the server to do so.

	AttributeSet = CreateDefaultSubobject<UAuraAttributeSet>("AttributeSet");

	HealthBar = CreateDefaultSubobject<UWidgetComponent>("HealthBar"); // Creates a widget component for the health bar.
	HealthBar->SetupAttachment(GetRootComponent()); // Attaches the health bar widget to the root component of the actor.
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

void AAuraEnemy::Die()
{	
	SetLifeSpan(LifeSpan); // Sets a lifespan of 5 seconds for the actor before it is destroyed.
	Super::Die();

}

void AAuraEnemy::BeginPlay()
{
	Super::BeginPlay(); 
	GetCharacterMovement()->MaxWalkSpeed = BaseWalkSpeed;

	InitAbilityActorInfo(); // Initializes the ability actor info, which includes setting up the ability system component and default attributes.
	if (HasAuthority())
	{
		UAuraAbilitySystemLibrary::GiveStartupAbilites(this, AbilitySystemComponent); // Grants the startup abilities to the actor using the AuraAbilitySystemLibrary.
	}
	

	if (UAuraUserWidget* AuraUserWidget = Cast<UAuraUserWidget>(HealthBar->GetUserWidgetObject())) 
	{
		AuraUserWidget->SetWidgetController(this); 
	}

	// Cast the AttributeSet to UAuraAttributeSet to access specific attributes.
	if (const UAuraAttributeSet* AuraAS = Cast<UAuraAttributeSet>(AttributeSet))
	{
		AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AuraAS->GetHealthAttribute()).AddLambda(
				[this](const FOnAttributeChangeData& Data)
			{
				OnHealthChanged.Broadcast(Data.NewValue); // Broadcasts the OnHealthChanged event with the new health values.
			}
		);

		AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AuraAS->GetMaxHealthAttribute()).AddLambda(
			[this](const FOnAttributeChangeData& Data)
			{
				OnMaxHealthChanged.Broadcast(Data.NewValue); // Broadcasts the OnHealthChanged event with the new maxhealth values.
			}
		);

		
		AbilitySystemComponent->RegisterGameplayTagEvent(FAuraGameplayTags::Get().Effects_HitReact, EGameplayTagEventType::NewOrRemoved).AddUObject(
			this,
			&AAuraEnemy::HitReactTagChanged
		);

		OnHealthChanged.Broadcast(AuraAS->GetHealth()); // Broadcasts the OnHealthChanged event with the current health value.
		OnMaxHealthChanged.Broadcast(AuraAS->GetMaxHealth()); // Broadcasts the OnMaxHealthChanged event with the current max health value.
	}
	
	
}

void AAuraEnemy::HitReactTagChanged(const FGameplayTag CallbackTag, int32 NewCount)
{
	bHitReacting = NewCount > 0; // Check if the hit react tag is active based on the count.
	GetCharacterMovement()->MaxWalkSpeed = bHitReacting ? 0.f : BaseWalkSpeed; // If hit reacting, set the walk speed to 0, otherwise set it to the default walk speed.

}

void AAuraEnemy::InitAbilityActorInfo()
{
	AbilitySystemComponent->InitAbilityActorInfo(this, this); // Initialize the ability system component with the actor info.

	Cast<UAuraAbilitySystemComponent>(AbilitySystemComponent)->AbilityActorInfoSet();

	if (HasAuthority())
	{
		InitializeDefaultAttributes();
	}
	
}

void AAuraEnemy::InitializeDefaultAttributes() const
{
	UAuraAbilitySystemLibrary::InitializeDefaultAttributes(this, CharacterClass, Level, AbilitySystemComponent);
}
