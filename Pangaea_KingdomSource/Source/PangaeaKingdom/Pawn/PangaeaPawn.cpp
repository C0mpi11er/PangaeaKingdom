// Fill out your copyright notice in the Description page of Project Settings.


#include "PangaeaPawn.h"
#include "Components/InputComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Camera/CameraComponent.h"
#include "Components/SphereComponent.h"
#include "GameFramework/FloatingPawnMovement.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/KismetMathLibrary.h"
// Sets default values
APangaeaPawn::APangaeaPawn()
{
	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
	MFloatMovementComp=CreateDefaultSubobject<UFloatingPawnMovement>(TEXT("FloatPawnMoveComp"));
	MCursor = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PawnMesh"));
	MCursor->SetupAttachment(GetRootComponent());
	//to attach to camera comp
	MCameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	MCameraBoom->SetupAttachment(GetRootComponent());
	//should use parent rotations
	MCameraBoom->bInheritPitch;
	MCameraBoom->bInheritYaw;
	MCameraBoom->bInheritRoll;
	//Distance From Pawn
	MCameraBoom->TargetArmLength = 1139.f;
	MCameraBoom->SocketOffset = FVector(-300.f, 0.f, 80.f);


	MCameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComp"));
	MCameraComp->SetupAttachment(MCameraBoom, MCameraBoom->SocketName);

	MCollisionSphereComp = CreateDefaultSubobject<USphereComponent>(TEXT("CollisionSphere"));
	MCollisionSphereComp->SetupAttachment(GetRootComponent());
}

// Called when the game starts or when spawned
void APangaeaPawn::BeginPlay()
{
	Super::BeginPlay();
	//init mapping context
	if (const APlayerController* PlayerController{(Cast<APlayerController>(GetController()))})
	{
		if (UEnhancedInputLocalPlayerSubsystem* EnhancedSubsystem{
			ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer())
		})
		{
			EnhancedSubsystem->AddMappingContext(MInputMappingContext,0);
		}
	}
	UpdateZoom();
}

void APangaeaPawn::CameraDepthOfField()
{
	FPostProcessSettings CameraDof;
	CameraDof.DepthOfFieldSensorWidth = 150.f;
	CameraDof.DepthOfFieldFocalDistance = MCameraBoom->TargetArmLength;
	CameraDof.DepthOfFieldFstop = 3.0;
	if(MCameraComp)
		MCameraComp->PostProcessSettings=CameraDof;
	
}

void APangaeaPawn::UpdateZoom()
{
	float ZoomValue{0.5f};
	MZoomDirection*=0.01;
	ZoomValue=FMath::Clamp(MZoomDirection+ZoomValue,0.f,1.f);
	if(MZoomCurveFloat)
	{
		const float InterpolatingValue = MZoomCurveFloat->GetFloatValue(ZoomValue);
		if(MCameraBoom)
		{
			
			MCameraBoom->TargetArmLength=UKismetMathLibrary::Lerp(800.f,8'000.f,InterpolatingValue);
			MCameraBoom->SetRelativeRotation(FRotator(UKismetMathLibrary::Lerp(-40,-55,InterpolatingValue),0.f,0.f));
		}
		MFloatMovementComp->MaxSpeed=UKismetMathLibrary::Lerp(1'000,6'000,InterpolatingValue);
		CameraDepthOfField();
		if(MCameraComp)
		    MCameraComp->SetFieldOfView(UKismetMathLibrary::Lerp(20.f,15.f,InterpolatingValue));
	}
}

void APangaeaPawn::Move(const FInputActionValue& InputActionValue)
{
	const FVector2d Movement=InputActionValue.Get<FVector2d>();

	if(Controller&&Movement.Size()!=0.f)
	AddMovementInput(GetActorForwardVector(),Movement.Y);
	AddMovementInput(GetActorRightVector(),Movement.X);
}

void APangaeaPawn::Spin(const FInputActionValue& InputActionValue)
{
	const auto SpinDirection{InputActionValue.Get<float>()};
	if(Controller&&SpinDirection!=0.f)
	{
		AddActorLocalRotation(FRotator(0.f,SpinDirection,0.f));
	}
}

void APangaeaPawn::Zoom(const FInputActionValue& InputActionValue)
{
	/*BUG: find out whtas wrong with the zoom function*/
	if (Controller)
	{
		MZoomDirection=InputActionValue.Get<float>();
		UpdateZoom();
		CameraDepthOfField();
	}
}

// Called every frame
void APangaeaPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// Called to bind functionality to input
void APangaeaPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	if(UEnhancedInputComponent*EnhancedInputComponent{CastChecked<UEnhancedInputComponent>(PlayerInputComponent)})
	{
      EnhancedInputComponent->BindAction(MInputActionMove,ETriggerEvent::Triggered,this,&APangaeaPawn::Move);
      EnhancedInputComponent->BindAction(MInputActionSpin,ETriggerEvent::Triggered,this,&APangaeaPawn::Spin);
      EnhancedInputComponent->BindAction(MInputActionZoom,ETriggerEvent::Triggered,this,&APangaeaPawn::Zoom);
		
		
	}
}
