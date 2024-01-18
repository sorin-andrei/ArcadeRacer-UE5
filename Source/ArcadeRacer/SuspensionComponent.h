// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Components/SceneComponent.h"
#include "SuspensionComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ARCADERACER_API USuspensionComponent : public USceneComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	USuspensionComponent();

	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BaseParameters")
	float Length = 100.f; //170 rear 140 front
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BaseParameters")
	float Stiffness = 980000.f; //1960000.0 
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BaseParameters")
	float Damping = 98000.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BaseParameters")
	float WheelRadius = 30.f; //60

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ImpactAssist")
	float MaxCompressionPoint = 0.5;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ImpactAssist")
	float MaxForceMultiplier = 20.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Debug")
	bool DrawDebugLines = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Debug")
	FColor LineHitColor = FColor(0, 255, 0);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Debug")
	FColor LineMissColor = FColor(255, 0, 0);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Debug")
	FColor LineCriticalColor = FColor(255, 255, 0);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Debug")
	int32 SphereSegments = 10;

	UPROPERTY(BlueprintReadOnly)
	float Compression = 0.f;

	UPROPERTY(BlueprintReadOnly)
	float PreviousCompression = 0.f;

	UPROPERTY(BlueprintReadOnly)
	float Force = 0.f;

	UPROPERTY(BlueprintReadOnly)
	FVector ImpactNormal;

	UPROPERTY(BlueprintReadOnly)
	float Dot;


	UFUNCTION()
	void ComputeSuspensionData(float AsyncDeltaTime, FVector AsyncLocation, FVector AsyncUpVector, float VehicleSpeedNormalized);

	UFUNCTION()
	bool GetHitSuccess() { return HitSuccess; };

	UPROPERTY(BlueprintReadOnly)
	float DisplayHitDistance;


protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	FVector DebugLineStart;
	FVector DebugLineEnd;
	FColor DebugColor;

private:	
	bool HitSuccess;
};
