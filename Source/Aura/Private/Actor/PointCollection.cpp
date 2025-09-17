// Copyright Patrick Haubner


#include "Actor/PointCollection.h"

#include "AbilitySystem/AuraAbilitySystemLibrary.h"
#include "Aura/AuraLogChannels.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"

APointCollection::APointCollection()
{
	PrimaryActorTick.bCanEverTick = false;

	Pt_0 = CreateDefaultSubobject<USceneComponent>("Pt_0");
	ImmutablePts.Add(Pt_0);
	SetRootComponent(Pt_0);

	Pt_1 = CreateDefaultSubobject<USceneComponent>("Pt_1");
	ImmutablePts.Add(Pt_1);
	Pt_1->SetupAttachment(GetRootComponent());

	Pt_2 = CreateDefaultSubobject<USceneComponent>("Pt_2");
	ImmutablePts.Add(Pt_2);
	Pt_2->SetupAttachment(GetRootComponent());

	Pt_3 = CreateDefaultSubobject<USceneComponent>("Pt_3");
	ImmutablePts.Add(Pt_3);
	Pt_3->SetupAttachment(GetRootComponent());

	Pt_4 = CreateDefaultSubobject<USceneComponent>("Pt_4");
	ImmutablePts.Add(Pt_4);
	Pt_4->SetupAttachment(GetRootComponent());

	Pt_5 = CreateDefaultSubobject<USceneComponent>("Pt_5");
	ImmutablePts.Add(Pt_5);
	Pt_5->SetupAttachment(GetRootComponent());

	Pt_6 = CreateDefaultSubobject<USceneComponent>("Pt_6");
	ImmutablePts.Add(Pt_6);
	Pt_6->SetupAttachment(GetRootComponent());

	Pt_7 = CreateDefaultSubobject<USceneComponent>("Pt_7");
	ImmutablePts.Add(Pt_7);
	Pt_7->SetupAttachment(GetRootComponent());

	Pt_8 = CreateDefaultSubobject<USceneComponent>("Pt_8");
	ImmutablePts.Add(Pt_8);
	Pt_8->SetupAttachment(GetRootComponent());

	Pt_9 = CreateDefaultSubobject<USceneComponent>("Pt_9");
	ImmutablePts.Add(Pt_9);
	Pt_9->SetupAttachment(GetRootComponent());

	Pt_10 = CreateDefaultSubobject<USceneComponent>("Pt_10");
	ImmutablePts.Add(Pt_10);
	Pt_10->SetupAttachment(GetRootComponent());
}

TArray<USceneComponent*> APointCollection::GetGroundPoints(const FVector& GroundLocation, int32 NumPoints,
	float YawOverride)
{
	//checkf(ImmutablePts.Num() >= NumPoints, TEXT("Attempted to access ImmutablePts out of bounds."));
	checkf(ImmutablePts.Num() >= NumPoints, TEXT("%hs: The number of Points must be greater than 0."), __FUNCTION__);

	if (NumPoints < 1)
	{
		UE_LOG(LogAura, Warning, TEXT("%hs: Called with invalid PointCount (%i), it must be greater than 0."), __FUNCTION__, NumPoints);
		return TArray<USceneComponent*>();
	}
 
	if (NumPoints > ImmutablePts.Num())
	{
		UE_LOG(LogAura, Warning, TEXT("%hs: Called with invalid PointCount (%i), setting PointCount to the maximum available (%i)."), __FUNCTION__, NumPoints, ImmutablePts.Num());
		NumPoints = ImmutablePts.Num();
	}

	TArray<USceneComponent*> ArrayCopy;

	for (USceneComponent* Pt : ImmutablePts)
	{
		if (ArrayCopy.Num() >= NumPoints) return ArrayCopy;

		if (Pt != Pt_0)
		{
			FVector ToPoint = Pt->GetComponentLocation() - Pt_0->GetComponentLocation();
			ToPoint = ToPoint.RotateAngleAxis(YawOverride, FVector::UpVector);
			Pt->SetWorldLocation(Pt_0->GetComponentLocation() + ToPoint);
		}

		const FVector RaisedLocation = FVector(Pt->GetComponentLocation().X, Pt->GetComponentLocation().Y, Pt->GetComponentLocation().Z + 250.f);
		const FVector LoweredLocation = FVector(Pt->GetComponentLocation().X, Pt->GetComponentLocation().Y, Pt->GetComponentLocation().Z - 250.f);

		FHitResult HitResult;
		TArray<AActor*> ActorsToIgnore;
		UAuraAbilitySystemLibrary::GetLivePlayersWithinRadius(
			this,
			ActorsToIgnore,
			TArray<AActor*>(),
			1500.f,
			GetActorLocation());

		//FCollisionQueryParams QueryParams;
		//QueryParams.AddIgnoredActors(ActorsToIgnore);
		
		UKismetSystemLibrary::LineTraceSingle(
			this,
			RaisedLocation,
			LoweredLocation,
			TraceTypeQuery1,
			false, ActorsToIgnore,
			EDrawDebugTrace::None,
			HitResult,
			true);
		
		//GetWorld()->LineTraceSingleByProfile(HitResult, RaisedLocation, LoweredLocation, FName("BlockAll"), QueryParams);

		if (HitResult.bBlockingHit)
		{
			const FVector ZAdjustedLocation = FVector(Pt->GetComponentLocation().X, Pt->GetComponentLocation().Y, HitResult.ImpactPoint.Z);
			Pt->SetWorldLocation(ZAdjustedLocation);
			Pt->SetWorldRotation(UKismetMathLibrary::MakeRotFromZ(HitResult.ImpactNormal));
		}
		
		ArrayCopy.Add(Pt);
	}
	return ArrayCopy;
}

void APointCollection::BeginPlay()
{
	Super::BeginPlay();
	
}


TArray<FTransform> APointCollection::GetPointsOnGeometry(int32 PointCount, const float YawOverride)
{
	checkf(ImmutablePts.Num() > 0, TEXT("%hs: The number of Points must be greater than 0."), __FUNCTION__);
     
	if (PointCount < 1)
	{
		UE_LOG(LogAura, Warning, TEXT("%hs: Called with invalid PointCount (%i), it must be greater than 0."), __FUNCTION__, PointCount);
		return TArray<FTransform>();
	}
     
	if (PointCount > ImmutablePts.Num())
	{
		UE_LOG(LogAura, Warning, TEXT("%hs: Called with invalid PointCount (%i), setting PointCount to the maximum available (%i)."), __FUNCTION__, PointCount, ImmutablePts.Num());
		PointCount = ImmutablePts.Num();
	}
     
	TArray<FTransform> PointsAtLocation;
	for (const TObjectPtr<USceneComponent>& Point : ImmutablePts)
	{
		FTransform PointTransform = Point->GetComponentTransform();
		FVector RootToPointVector = PointTransform.GetLocation() - GetActorLocation();
		RootToPointVector = RootToPointVector.RotateAngleAxis(YawOverride, FVector::UpVector);
		PointTransform.SetLocation(GetActorLocation() + RootToPointVector);

		FHitResult HitResult;
		const FVector RaisedLocation = FVector(PointTransform.GetLocation().X, PointTransform.GetLocation().Y, PointTransform.GetLocation().Z + 250.0f);
		const FVector LoweredLocation = FVector(PointTransform.GetLocation().X, PointTransform.GetLocation().Y, PointTransform.GetLocation().Z - 250.0f);

		TArray<AActor*> ActorsToIgnore;
		UAuraAbilitySystemLibrary::GetLivePlayersWithinRadius(
			this,
			ActorsToIgnore,
			TArray<AActor*>(),
			1500.f,
			GetActorLocation());

		UKismetSystemLibrary::LineTraceSingle(this, RaisedLocation, LoweredLocation, TraceTypeQuery1, false, ActorsToIgnore, EDrawDebugTrace::Persistent, HitResult, true);
		if (HitResult.bBlockingHit)
		{
			const FVector ZAdjustedPointLocation = FVector(PointTransform.GetLocation().X, PointTransform.GetLocation().Y, HitResult.ImpactPoint.Z);
			PointTransform.SetLocation(ZAdjustedPointLocation);
			PointTransform.SetRotation(UKismetMathLibrary::MakeRotFromZ(HitResult.ImpactNormal).Quaternion());

			PointsAtLocation.Emplace(PointTransform);
			if (PointsAtLocation.Num() == PointCount) break;
		}
	}
	return PointsAtLocation;
}       


