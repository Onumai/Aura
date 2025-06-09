// Copyright Patrick Haubner

#pragma once

#include "CoreMinimal.h"
#include "Character/AuraCharacterBase.h"
#include "Interaction/EnemyInterface.h"
#include "UI/WidgetController/OverlayWidgetController.h"
#include "AuraEnemy.generated.h"

class UWidgetComponent;


/**
 * 
 */
UCLASS()
class AURA_API AAuraEnemy : public AAuraCharacterBase, public IEnemyInterface
{
	GENERATED_BODY()

public:
	AAuraEnemy();

	/* Enemy Interface*/
	virtual void HighlightActor() override; 
	virtual void UnHighlightActor() override;
	/* end Enemy Interface*/


	/* Combat Interface*/

	virtual int32 GetPlayerLevel() override;

	/* End Combat Interface*/

	UPROPERTY(BlueprintAssignable)
	FOnAttributeChangedSignature OnHealthChanged; // Delegate from OverlayWidgetController

	UPROPERTY(BlueprintAssignable)
	FOnAttributeChangedSignature OnMaxHealthChanged; // Delegate from OverlayWidgetController

	UPROPERTY(BlueprintReadOnly)
	bool bHighlighted = false; // Flag to check if the actor is highlighted or not.

protected:
	virtual void BeginPlay() override;

	virtual void InitAbilityActorInfo() override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character Class Defaults")
	int32 Level = 1;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UWidgetComponent> HealthBar;
	
};
