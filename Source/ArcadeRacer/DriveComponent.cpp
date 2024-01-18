// Fill out your copyright notice in the Description page of Project Settings.


#include "DriveComponent.h"

UDriveComponent::UDriveComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UDriveComponent::BeginPlay()
{
	Super::BeginPlay();
	ParentVehicle = Cast<UStaticMeshComponent>(GetOwner()->GetRootComponent());
	if (!ParentVehicle)
	{
		UE_LOG(LogTemp, Error, TEXT("DriveComponent must be attached to AVehicle class"));
	}

	InitialCenterOfMass = UAsyncTickFunctions::ATP_GetCoMPosition(ParentVehicle);
}

void UDriveComponent::NativeAsyncTick(float DeltaTime)
{
	Super::NativeAsyncTick(DeltaTime);

	if (!ParentVehicle)
	{
		return;
	}

	FTransform VehicleTransform = UAsyncTickFunctions::ATP_GetTransform(ParentVehicle);
	FQuat VehicleRotation = VehicleTransform.GetRotation();

	ForwardVector = VehicleRotation.GetForwardVector();
	RightVector = VehicleRotation.GetRightVector();
	UpVector = VehicleRotation.GetUpVector();
	
	FVector LinearVelocity = UAsyncTickFunctions::ATP_GetLinearVelocity(ParentVehicle);
	CurrentSpeed = FVector::DotProduct(LinearVelocity, ForwardVector) * 0.036;  //Convert to km/h
	LateralSpeed = FVector::DotProduct(LinearVelocity, RightVector) * 0.036;

	CurrentSpeedNormalized = CurrentSpeed / 1200.f;
	LateralSpeedNormalized = LateralSpeed / 300.f;

	if (bIsDrifting && bIsGrounded)
	{
		TractionMultiplier = FMath::FInterpTo(TractionMultiplier, 0.05f, DeltaTime, 12.f);
		TorqueMultiplier = FMath::FInterpTo(TorqueMultiplier, 1.4f, DeltaTime, 12.f);
	}
	else
	{
		TractionMultiplier = FMath::FInterpTo(TractionMultiplier, 1, DeltaTime, 6.f);
		TorqueMultiplier = FMath::FInterpTo(TorqueMultiplier, 1.f, DeltaTime, 6.f);
	}

	AccelerationMultiplier = 1 + BoostPadMultiplier;
}

void UDriveComponent::InitiateDrift()
{
	if (bIsDrifting)
	{
		return;
	}
	bIsDrifting = true;
	DriftDetect.Broadcast(true);
	GetWorld()->GetTimerManager().SetTimer(DriftChecker, this, &UDriveComponent::CheckIfStillDrifting, 1.f, true);
}

void UDriveComponent::StopDrift()
{
	bIsDrifting = false;
	DriftDetect.Broadcast(false);
}

FVector UDriveComponent::ComputeThrottleForce()
{
	if (CurrentSpeed >= SpeedCap)
	{
		return FVector::ZeroVector;
	}
	return ForwardVector * Acceleration * AccelerationMultiplier * ThrottleInput;
}

FVector UDriveComponent::ComputeSteeringTorque()
{
	return UpVector * Torque * TorqueMultiplier * SteeringInput;
}

FVector UDriveComponent::ComputeTractionForce()
{
	if (!bIsGrounded)
	{
		return FVector::ZeroVector;
	}
	return RightVector * (-1.f * (LateralSpeed / 0.036) * Traction * TractionMultiplier);
}

FVector UDriveComponent::ComputeDriftForce()
{
	if (!bIsDrifting || CurrentSpeed >= SpeedCap || !bIsGrounded)
	{
		return FVector::ZeroVector;
	}
	FVector LateralForce = RightVector * LateralSpeedNormalized * 1000000.f * -1.f;
	FVector ForwardForce = ForwardVector * FMath::Abs(LateralSpeedNormalized) * 8000000.f;
	
	return ForwardForce + LateralForce;
}

void UDriveComponent::SetSteeringInput(float Value)
{
	 SteeringInput = Value;
	 if (Value != 0)
	 {
		 FVector SteeringCoM = FVector(InitialCenterOfMass.X, -1.f * Value * SteeringRoll * (1 - GetCurrentSpeedNormalized()), InitialCenterOfMass.Z);
		 ParentVehicle->SetCenterOfMass(SteeringCoM);
	 }
	 else
	 {
		 ParentVehicle->SetCenterOfMass(InitialCenterOfMass);
	 }
}

void UDriveComponent::CheckIfStillDrifting()
{
	if (FMath::Abs(LateralSpeed) <= 100.f)
	{
		StopDrift();
	}
}
