// Copyright Patrick Haubner


#include "Character/AuraEnemy.h"
#include <Aura/Aura.h>

AAuraEnemy::AAuraEnemy()
{
	GetMesh()->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
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
