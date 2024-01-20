// Fill out your copyright notice in the Description page of Project Settings.


#include "Vehicle.h"


// Sets default values (Constructor)
AVehicle::AVehicle()
{
	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//Create DriveComponent
	DriveComponent = CreateDefaultSubobject<UDriveComponent>(TEXT("DriveComponent"));

	//Setup Body
	Body = CreateDefaultSubobject<UStaticMeshComponent>("Body");
	Body->SetSimulatePhysics(true);
	Body->SetEnableGravity(true);
	SetRootComponent(Body);

	//Setup Suspensions
	SuspensionFR = CreateDefaultSubobject<USuspensionComponent>("Suspension FR");
	SuspensionFL = CreateDefaultSubobject<USuspensionComponent>("Suspension FL");
	SuspensionRR = CreateDefaultSubobject<USuspensionComponent>("Suspension RR");
	SuspensionRL = CreateDefaultSubobject<USuspensionComponent>("Suspension RL");

	SuspensionArray = { SuspensionFR, SuspensionFL, SuspensionRR, SuspensionRL };

	// Attaching to body
	SuspensionFR->SetupAttachment(Body);
	SuspensionFL->SetupAttachment(Body);
	SuspensionRR->SetupAttachment(Body);
	SuspensionRL->SetupAttachment(Body);

	DriveComponent->DriftDetect.AddDynamic(this, &AVehicle::OnDriftStateChanged);
}

void AVehicle::BeginPlay()
{
	Super::BeginPlay();
	AddInputMappingContext();

}

void AVehicle::NativeAsyncTick(float DeltaTime)
{
	Super::NativeAsyncTick(DeltaTime);
	
	//Setup async vectors
	FTransform Transform = UAsyncTickFunctions::ATP_GetTransform(Body);
	FQuat Rotation = Transform.GetRotation();

	FVector TotalForce = FVector::ZeroVector;
	ApplySuspensionForce(DeltaTime, Transform, DriveComponent->GetUpVector());
	TotalForce += DriveComponent->ComputeThrottleForce() + DriveComponent->ComputeTractionForce() + DriveComponent->ComputeDriftForce() + DriveComponent->ComputeDownforce();
	
	UAsyncTickFunctions::ATP_AddForce(Body, TotalForce, false);
	UAsyncTickFunctions::ATP_AddTorque(Body, DriveComponent->ComputeSteeringTorque(), false);
	
}

void AVehicle::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	DrawDebugSphere(GetWorld(), UAsyncTickFunctions::ATP_GetCoMPosition(Body), /*radius=*/20.f,/*segments= */3, FColor::Purple, /*bPersistentLines=*/false, /*lifetime=*/0.f, /*depthpriority=*/ 1, /*thickness=*/ 1.f);
}

void AVehicle::ApplySuspensionForce(float DeltaTime, FTransform VehicleTransform, FVector UpVector)
{
	if (SuspensionArray.IsEmpty())
	{
		return;
	}
	DriveComponent->SetVehicleGrounded(false);
	for (USuspensionComponent* Suspension : SuspensionArray)
	{
		FVector AsyncSuspensionLocation = VehicleTransform.TransformPosition(Suspension->GetRelativeLocation());
		Suspension->ComputeSuspensionData(DeltaTime, AsyncSuspensionLocation, UpVector, DriveComponent->GetCurrentSpeedNormalized());
		FVector SuspensionForce = UpVector * Suspension->Force;
		UAsyncTickFunctions::ATP_AddForceAtPosition(Body, AsyncSuspensionLocation, SuspensionForce);

		if (Suspension->GetHitSuccess())
		{
			DriveComponent->SetVehicleGrounded(true);
		}
	}
}

void AVehicle::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	// Set up action bindings
	UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent);
	if (!EnhancedInputComponent)
	{
		UE_LOG(LogTemp, Error, TEXT("'%s' Failed to find an Enhanced Input component!"));
		return;
	}
	
	EnhancedInputComponent->BindAction(ThrottleAction, ETriggerEvent::Triggered, this, &AVehicle::ApplyThrottle);	
	EnhancedInputComponent->BindAction(ThrottleAction, ETriggerEvent::Completed, this, &AVehicle::ApplyThrottle);
	
	EnhancedInputComponent->BindAction(SteeringAction, ETriggerEvent::Triggered, this, &AVehicle::ApplySteering);
	EnhancedInputComponent->BindAction(SteeringAction, ETriggerEvent::Completed, this, &AVehicle::ApplySteering);

	EnhancedInputComponent->BindAction(DriftAction, ETriggerEvent::Triggered, this, &AVehicle::ExecuteDrift);
}


void AVehicle::ApplyThrottle(const FInputActionValue& Value)
{
	if (Controller == nullptr)
	{
		return;
	}
	DriveComponent->SetThrottleInput(FMath::Clamp(Value.Get<float>(),-1,1));
}
void AVehicle::ApplySteering(const FInputActionValue& Value)
{
	if (Controller == nullptr)
	{
		return;
	}
	DriveComponent->SetSteeringInput(FMath::Clamp(Value.Get<FVector2D>().X,-1,1));
}

void AVehicle::ExecuteDrift()
{
	if (Controller == nullptr)
	{
		return;
	}
	DriveComponent->InitiateDrift();
}

void AVehicle::OnDriftStateChanged(bool DriftState)
{
	if (DriftState == true)
	{
		BP_StartDrift();
	}
	else
	{
		BP_EndDrift();
	}
}

void AVehicle::AddInputMappingContext()
{
	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}
}