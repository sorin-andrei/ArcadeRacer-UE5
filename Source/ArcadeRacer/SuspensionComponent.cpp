// Fill out your copyright notice in the Description page of Project Settings.


#include "SuspensionComponent.h"

// Sets default values for this component's properties
USuspensionComponent::USuspensionComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void USuspensionComponent::BeginPlay()
{
	Super::BeginPlay();
	
	// ...
	
}


// Called every frame
void USuspensionComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	if (DrawDebugLines)
	{
		FVector TraceStart = GetComponentLocation();
		FVector TraceEnd = TraceStart - (GetUpVector() * Length);

		FHitResult VisualHit;
		FCollisionQueryParams VisualCollisionParams;
		VisualCollisionParams.AddIgnoredActor(GetOwner());
		bool VisualSphereTrace = GetWorld()->SweepSingleByChannel(VisualHit, TraceStart, TraceEnd, FQuat::Identity, ECC_WorldStatic, FCollisionShape::MakeSphere(WheelRadius), VisualCollisionParams);

		if (VisualSphereTrace)
		{
			TraceEnd = VisualHit.Location;
		}

		DrawDebugSphere(GetWorld(), TraceEnd, WheelRadius,/*segments= */SphereSegments, DebugColor, /*bPersistentLines=*/false, /*lifetime=*/0.f, /*depthpriority=*/ 1, /*thickness=*/ 1.f );
		DrawDebugLine(GetWorld(), TraceStart, TraceEnd, DebugColor, false, 0.f, 100, 5.f);
		DrawDebugString(GetWorld(),GetComponentLocation(), FString::SanitizeFloat(Compression), NULL, FColor::White, 0.f);
	}

	// ...
}

void USuspensionComponent::ComputeSuspensionData(float AsyncDeltaTime, FVector AsyncLocation, FVector AsyncUpVector, float VehicleSpeedNormalized)
{
	FCollisionQueryParams CollisionParams;
	FHitResult Hit;
	CollisionParams.AddIgnoredActor(GetOwner());

	FVector TraceStart = AsyncLocation;
	FVector TraceEnd = TraceStart - (AsyncUpVector * Length);

	HitSuccess = GetWorld()->SweepSingleByChannel(Hit, TraceStart, TraceEnd, FQuat::Identity, ECC_WorldStatic, FCollisionShape::MakeSphere(WheelRadius), CollisionParams);
	ImpactNormal = Hit.ImpactNormal;
	Dot = FVector::DotProduct(ImpactNormal, GetUpVector());

	Compression = (Length - Hit.Distance) / Length;
	DisplayHitDistance = Length - Hit.Distance;

	float CurrentDamping = Damping;
	float CurrentStiffness = Stiffness;
	
	if (!HitSuccess)
	{
		DebugLineEnd = TraceEnd;
		Compression = 0;
		Force = 0;
		PreviousCompression = 0;
		DebugColor = LineMissColor;
		return;
	}

	Force = (CurrentStiffness * Compression) + CurrentDamping * (Compression - PreviousCompression) / AsyncDeltaTime;
	PreviousCompression = Compression;
	DebugColor = LineHitColor;

	if (Compression > MaxCompressionPoint)
	{
		float Multiplier = VehicleSpeedNormalized * MaxForceMultiplier;
		if (Multiplier < 1)
		{
			Multiplier = 1;
		}
		Force *= Multiplier;
	}
	if (Compression > 0.5f)
	{
		DebugColor = LineCriticalColor;
		Force *= 1.2f;
	}

	if (FVector::Orthogonal(Hit.ImpactNormal, GetUpVector()))
	{
		Force = 0;
	}
	return;
}
