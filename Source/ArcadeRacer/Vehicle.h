// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AsyncTickPawn.h"
#include "SuspensionComponent.h"
#include "AsyncTickFunctions.h"
#include "Components/StaticMeshComponent.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"

#include "DriveComponent.h"

#include "Vehicle.generated.h"

class UInputMappingContext;
class UInputAction; 
struct FInputActionValue;

/**
 * 
 */
UCLASS()
class ARCADERACER_API AVehicle : public AAsyncTickPawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	AVehicle();

	// Fixed Tick
	void NativeAsyncTick(float DeltaTime);

	// Event Tick
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UStaticMeshComponent* Body;

protected:
	virtual void BeginPlay() override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UDriveComponent* DriveComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class USuspensionComponent* SuspensionFR;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class USuspensionComponent* SuspensionFL;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class USuspensionComponent* SuspensionRR;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class USuspensionComponent* SuspensionRL;

	TArray<USuspensionComponent*> SuspensionArray;
	
	/** INPUT: Mapping Context */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	UInputMappingContext* DefaultMappingContext;

	/** INPUT: Throttle Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	UInputAction* ThrottleAction;

	/** INPUT: Steering Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	UInputAction* SteeringAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	UInputAction* DriftAction;

	void ApplySuspensionForce(float DeltaTime, FTransform VehicleTransform, FVector UpVector);

	/** Called for movement input */
	void ApplyThrottle(const FInputActionValue& Value);
	void ApplySteering(const FInputActionValue& Value);
	void ExecuteDrift();
	UFUNCTION(BlueprintImplementableEvent)
	void BP_StartDrift();
	UFUNCTION(BlueprintImplementableEvent)
	void BP_EndDrift();

	UFUNCTION()
	void OnDriftStateChanged(bool DriftState);
	
	void AddInputMappingContext();
};
