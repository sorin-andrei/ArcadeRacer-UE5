// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AsyncTickActorComponent.h"
#include "AsyncTickFunctions.h"
#include "InputActionValue.h"
#include "DriveComponent.generated.h"

/**
 * 
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FDrifting, bool, DriftState);
UCLASS()
class ARCADERACER_API UDriveComponent : public UAsyncTickActorComponent
{
	GENERATED_BODY()

public:
	//Sets default values for this componment's properties
	UDriveComponent();

	void NativeAsyncTick(float DeltaTime);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Handling")
	float Acceleration = 1000000.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Handling")
	float Torque = 100000000.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Handling")
	float Downforce = 3000.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Handling")
	float SpeedCap = 1200.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Handling")
	float BodyRoll = 60.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Drifting")
	float Traction = 30000.f;

	UPROPERTY(BlueprintReadOnly)
	UStaticMeshComponent* ParentVehicle;

	FVector ComputeThrottleForce();
	FVector ComputeSteeringTorque();
	FVector ComputeTractionForce();
	FVector ComputeDriftForce();
	FVector ComputeDownforce();

	UFUNCTION(BlueprintCallable)
	float GetCurrentSpeed() {  return CurrentSpeed; };
	UFUNCTION(BlueprintCallable)
	float GetCurrentSpeedNormalized() { return CurrentSpeedNormalized; };
	UFUNCTION(BlueprintCallable)
	float GetLateralSpeed() { return LateralSpeed; };
	UFUNCTION(BlueprintCallable)
	float GetLateralSpeedNormalized() { return LateralSpeedNormalized; };
	UFUNCTION(BlueprintCallable)
	bool GetDriftingState() { return bIsDrifting; };

	UPROPERTY(BlueprintReadOnly)
	float TractionMultiplier = 1.f;

	UPROPERTY(BlueprintReadOnly)
	float AccelerationMultiplier = 1.f;

	UPROPERTY(BlueprintReadWrite)
	float BoostPadMultiplier = 0;

	UPROPERTY(BlueprintReadOnly)
	float TorqueMultiplier = 1.f;

	UPROPERTY(BlueprintReadOnly)
	float ThrottleInput;

	UPROPERTY(BlueprintReadOnly)
	float SteeringInput;

	FVector GetForwardVector() { return ForwardVector; };
	FVector GetRightVector() { return RightVector; };
	FVector GetUpVector() { return UpVector; };
	
	void SetThrottleInput(float Value) { ThrottleInput = Value; };
	void SetSteeringInput(float Value);
	void SetVehicleGrounded(bool Value) { bIsGrounded = Value; };

	void InitiateDrift();
	void StopDrift();

	UPROPERTY(BlueprintAssignable, Category = "EventDispatchers")
	FDrifting DriftDetect;

protected:

	virtual void BeginPlay() override;

	void CheckIfStillDrifting();

	float CurrentSpeed;
	float CurrentSpeedNormalized;
	float LateralSpeed;
	float LateralSpeedNormalized;
	float DriftAccelerationBonus;

	FVector ForwardVector;
	FVector RightVector;
	FVector UpVector;



	FVector InitialCenterOfMass;
	bool bIsGrounded;
	bool bIsDrifting;

	FTimerHandle DriftChecker;
};
