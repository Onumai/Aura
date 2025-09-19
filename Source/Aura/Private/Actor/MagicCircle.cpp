// Copyright Patrick Haubner


#include "Actor/MagicCircle.h"
#include "Components/DecalComponent.h"
#include "Components/SphereComponent.h"
#include "Interaction/EnemyInterface.h"

AMagicCircle::AMagicCircle()
{
	PrimaryActorTick.bCanEverTick = true;

	SetRootComponent(CreateDefaultSubobject<USceneComponent>("SceneRoot"));
	MagicCircleDecal = CreateDefaultSubobject<UDecalComponent>("MagicCircleDecal");
	MagicCircleDecal->SetupAttachment(GetRootComponent());

	OverlapSphere = CreateDefaultSubobject<USphereComponent>("OverlapSphere");
	OverlapSphere->SetupAttachment(GetRootComponent());
	OverlapSphere->SetCollisionResponseToChannels(ECR_Ignore);
	OverlapSphere->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
}

void AMagicCircle::BeginPlay()
{
	Super::BeginPlay();

	OverlapSphere->OnComponentBeginOverlap.AddDynamic(this, &AMagicCircle::OnSphereOverlap);
	OverlapSphere->OnComponentEndOverlap.AddDynamic(this, &AMagicCircle::OnSphereEndOverlap);
	
}

void AMagicCircle::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (IEnemyInterface* EnemyInterface = Cast<IEnemyInterface>(OtherActor))
	{
		EnemyInterface->HighlightActor();
	}
}

void AMagicCircle::OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (IEnemyInterface* EnemyInterface = Cast<IEnemyInterface>(OtherActor))
	{
		EnemyInterface->UnHighlightActor();
	}
}

void AMagicCircle::SetTargetingRadius(float Radius)
{
	OverlapSphere->SetSphereRadius(Radius);
	MagicCircleDecal->DecalSize = FVector(Radius);
}

void AMagicCircle::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

