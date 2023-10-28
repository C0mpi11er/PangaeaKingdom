// Fill out your copyright notice in the Description page of Project Settings.


#include "PangaeaKingdom/Public/PangaeaPawn/PangaeaPawn.h"
#include "Components/InputComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Camera/CameraComponent.h"
#include "Components/SphereComponent.h"
#include "GameFramework/FloatingPawnMovement.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "PangaeaKingdom/Public/PangaeaController/PangaeaPlayerController.h"
#include "Kismet/GameplayStatics.h"
// Sets default values
APangaeaPawn::APangaeaPawn()
{
	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
	MFloatMovementComp=CreateDefaultSubobject<UFloatingPawnMovement>(TEXT("FloatPawnMoveComp"));
	MCursor = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PawnMesh"));
	MCursor->SetupAttachment(GetRootComponent());
	MCursor->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	//to attach to camera comp
	MCameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	MCameraBoom->SetupAttachment(GetRootComponent());
	//should use parent rotations
	MCameraBoom->bInheritPitch;
	MCameraBoom->bInheritYaw;
	MCameraBoom->bInheritRoll;
	//Distance From Pawn
	MCameraBoom->TargetArmLength = 1139.f;
	MCameraBoom->bEnableCameraLag=true;
	MCameraBoom->SocketOffset = FVector(-300.f, 0.f, 80.f);


	MCameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComp"));
	MCameraComp->SetupAttachment(MCameraBoom, MCameraBoom->SocketName);
	

	MCollisionSphereComp = CreateDefaultSubobject<USphereComponent>(TEXT("CollisionSphere"));
	MCollisionSphereComp->SetupAttachment(GetRootComponent());
	MCollisionSphereComp->SetSphereRadius(180.f);
}

// Called when the game starts or when spawned
void APangaeaPawn::BeginPlay()
{
	Super::BeginPlay();
	UpdateZoom();
	StartMoveTrackTimer();
	//init player controller and input mapping context
	MPangaeaPlayerController=Cast<APangaeaPlayerController>(Controller);
   MPangaeaPlayerController=!MPangaeaPlayerController? Cast<APangaeaPlayerController>(Controller):MPangaeaPlayerController;
	if (MPangaeaPlayerController)
	{
		 MEnhanceInputSubsystem=ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(MPangaeaPlayerController->GetLocalPlayer());
		if(MEnhanceInputSubsystem)
		{
			MEnhanceInputSubsystem->AddMappingContext(MInputMappingContext,0);
			MEnhanceInputSubsystem->AddMappingContext(MVillageInputMappingContext,0);
		}
			
	}
	

	
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
	
	MZoomDirection*=0.01f;
	MZoomValue=FMath::Clamp(MZoomDirection+MZoomValue,0.f,1.f);
	if(MZoomCurveFloat)
	{
		const float InterpolatingValue = MZoomCurveFloat->GetFloatValue(MZoomValue);
		if(MCameraBoom)
		{
			
			MCameraBoom->TargetArmLength=UKismetMathLibrary::Lerp(800.f,40'000.f,InterpolatingValue);
			MCameraBoom->SetRelativeRotation(FRotator(UKismetMathLibrary::Lerp(-40,-55,InterpolatingValue),0.f,0.f));
		}
		if(MFloatMovementComp)
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
	
	if (Controller)
	{
		MZoomDirection=InputActionValue.Get<float>();
		UpdateZoom();
		CameraDepthOfField();
	}
}

void APangaeaPawn::DragMove(const FInputActionValue& InputActionValue)
{
	UE_LOG(LogTemp, Warning, TEXT("Dragmove triggered"))
	TrackMove();
}

void APangaeaPawn::DragOrSelect()
{
	//store mouse position in target handle
	CheckMouseScreenPosition();
	//check if controller is null or not
	VerifyPangaeaController();
	if(MPangaeaPlayerController)
	{
		MEnhanceInputSubsystem = !MEnhanceInputSubsystem
								 ? ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(
									 MPangaeaPlayerController->GetLocalPlayer()) : MEnhanceInputSubsystem;

		
		if(MEnhanceInputSubsystem)
		{
			UE_LOG(LogTemp,Warning,TEXT("DragIMC Added"))
			MEnhanceInputSubsystem->AddMappingContext(MDragInputMappingContext,0);
		}
			
	}
}

void APangaeaPawn::RemoveDragMappingContext()
{
	//check if controller is null or not
	VerifyPangaeaController();
	if(MPangaeaPlayerController)
	{
		MEnhanceInputSubsystem = !MEnhanceInputSubsystem
								 ? ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(
									 MPangaeaPlayerController->GetLocalPlayer()) : MEnhanceInputSubsystem;

		if(MEnhanceInputSubsystem)
		{
			FModifyContextOptions ContextOptions;
			ContextOptions.bIgnoreAllPressedKeysUntilRelease=true;
			ContextOptions.bForceImmediately=true;
			MEnhanceInputSubsystem->RemoveMappingContext(MDragInputMappingContext,ContextOptions);
			UE_LOG(LogTemp,Warning,TEXT("DragIMC removed"))
		}
			
	}
	
}

void APangaeaPawn::HoverActor()
{

	// do nothing for now
}

FVector APangaeaPawn::CalculateCameraAndBoomOffset()
{
	if(!MCameraBoom||!MCameraComp)return FVector::ZeroVector;
	//get delta of target arm and socket
	//the scale by the forward vector
	const auto DeltaTargetArmSocketX{
		UKismetMathLibrary::Subtract_DoubleDouble(MCameraBoom->TargetArmLength, MCameraBoom->SocketOffset.X)	
	};
	
	FVector ScaledArmLength{UKismetMathLibrary::Multiply_VectorFloat(GetActorForwardVector(),DeltaTargetArmSocketX)};
	ScaledArmLength=UKismetMathLibrary::Multiply_VectorFloat(ScaledArmLength,-1.f);
	const FVector ScaledSocketZUpVector{UKismetMathLibrary::Multiply_VectorFloat(MCameraBoom->GetUpVector(),MCameraBoom->SocketOffset.Z)};

	
	 FVector SpringArmVector{UKismetMathLibrary::Add_VectorVector(ScaledArmLength,ScaledSocketZUpVector)};
	SpringArmVector=UKismetMathLibrary::Add_VectorVector(SpringArmVector,MCameraBoom->GetComponentLocation());
	return UKismetMathLibrary::Subtract_VectorVector(MCameraComp->GetComponentLocation(),SpringArmVector);
	
	
}



void APangaeaPawn::CheckMouseScreenPosition()
{
	//store mouse deprojected position and store in target handle
	const FMouseProjectionResult MouseProjectionResult{ProjectMouseToGround()};
	MMouseTargetHandle=MouseProjectionResult.MIntersectionPoint;
}

FMouseProjectionResult APangaeaPawn::ProjectMouseToGround()
{
	FMouseProjectionResult MouseProjectionResult;
	FVector2d ViewportSize;
	VerifyPangaeaController();
	
	if(MPangaeaPlayerController)
	{
		int32 ViewX;
		int32 ViewY;
		MPangaeaPlayerController->GetViewportSize(ViewX,ViewY);
		ViewportSize.X=ViewX;
		ViewportSize.Y=ViewY;
	}
	const FVector2d ViewportCenter{UKismetMathLibrary::Divide_Vector2DFloat(ViewportSize,2.f)};

	VerifyPangaeaController();
	if(MPangaeaPlayerController)
	{
		
		//init the mouse position inside the vector and exit function if mouse is not valid
		FVector2d MousePosition;
		float MouseX;
		 float MouseY;
		if(MPangaeaPlayerController->GetMousePosition(MouseX,MouseY))
	   	{
			  //theres is a mouse device
          MouseProjectionResult.MValidProjection=true;
			//init a 2d vector position
			MousePosition.X=MouseX;
			MousePosition.Y=MouseY;
           // init the screen pos
			MouseProjectionResult.MScreenPos=MousePosition;
			
		}
		else
		{
			MouseProjectionResult.MValidProjection=false;   //there is no available mouse device
			MousePosition=ViewportCenter;                   //assume the mouse position is the viewport center
		}
       
		FVector WorldPosition;
		FVector WorldDirection;
		MPangaeaPlayerController->DeprojectScreenPositionToWorld(MousePosition.X,MousePosition.Y,WorldPosition,WorldDirection);
		float InterSectionFraction; // when multiplied by the line trace it give you length from start tp end
		FVector MousePlaneIntersectionPoint;   //make plane to intersect
		const FPlane InterSectionPlane {UKismetMathLibrary::MakePlaneFromPointAndNormal(FVector(0.f, 0.f, 0.f),
		                                                                                FVector(0.f, 0.f, 1.f))};
        
		const FVector LineStart{WorldPosition};   //were the intersection trace starts
		const FVector LineEnd{UKismetMathLibrary::Add_VectorVector(WorldPosition,WorldDirection*100'000.f)};
		const bool ValidIntersect = UKismetMathLibrary::LinePlaneIntersection(LineStart, LineEnd,
		                                                                      InterSectionPlane,
		                                                                      InterSectionFraction,
		                                                                      MousePlaneIntersectionPoint);
		if(ValidIntersect)
		{
			//init the mouse intersection point
			MouseProjectionResult.MIntersectionPoint=MousePlaneIntersectionPoint;
			
		}
	}
	return MouseProjectionResult;
	
}

void APangaeaPawn::TrackMove()
{
	const FVector CameraAndBoomDelta{CalculateCameraAndBoomOffset()};  //the delta between camera and camera boom
	const FMouseProjectionResult MouseProjectionResult{ProjectMouseToGround()};
	 FVector MouseTargetDelta{UKismetMathLibrary::Subtract_VectorVector(MMouseTargetHandle,MouseProjectionResult.MIntersectionPoint)};
	 MouseTargetDelta=UKismetMathLibrary::Subtract_VectorVector(MMouseTargetHandle,CameraAndBoomDelta);
	MouseTargetDelta.Z=0.f;
	if(MouseProjectionResult.MValidProjection)
	{
		//move pawn towards mouse direction
		AddActorWorldOffset(MouseTargetDelta);
	
	}
	

	
	

	
}

void APangaeaPawn::UpdateCursorPosition()
{
	if(!MCollisionSphereComp||!MCursor)return;


	const FTransform Target{MCollisionSphereComp->GetComponentTransform()};
	const FTransform TargetTransform{UKismetMathLibrary::MakeTransform(Target.GetLocation(),
	                                                                   Target.Rotator(),FVector(2.f,2.f,1.f))};
	const FTransform InterpolatedTransform{UKismetMathLibrary::TInterpTo(MCursor->GetComponentTransform(),
	                                                                     TargetTransform,
	                                                                     UGameplayStatics::GetWorldDeltaSeconds(GetWorld()),
	                                                                     12.0)};
	MCursor->SetWorldTransform(InterpolatedTransform);
	
}

void APangaeaPawn::MoveTracking()
{
	FVector NormalizedActorLoc{UKismetMathLibrary::Normal(GetActorLocation())};
	
	NormalizedActorLoc.Z=0.f;   //not needed because plan is 2d
	//scaled by -1
	NormalizedActorLoc=UKismetMathLibrary::Multiply_VectorFloat(NormalizedActorLoc,-1.f);
	
	double ActorLocVectorLength{GetActorLocation().Length()};
	ActorLocVectorLength=UKismetMathLibrary::Subtract_DoubleDouble(ActorLocVectorLength,9000.f);
	ActorLocVectorLength=UKismetMathLibrary::Divide_DoubleDouble(ActorLocVectorLength,5000.f);
	const auto ScaleValue{UKismetMathLibrary::Max(ActorLocVectorLength,0.0)};
	AddMovementInput(NormalizedActorLoc,ScaleValue);
	//sync cursor and collision position
	UpdateCursorPosition();
	if(MCollisionSphereComp)
	{
		   //position collision on ground plane.
		 FMouseProjectionResult MouseProjectionResult{ProjectMouseToGround()};
		   const FVector NewLocation{UKismetMathLibrary::Add_VectorVector(MouseProjectionResult.MIntersectionPoint,
		                                                                  FVector(0.f,0.f,10.f))};
		MCollisionSphereComp->SetWorldLocation(NewLocation);
	}
	
	
}

void APangaeaPawn::StartMoveTrackTimer()
{
	
	if(GetWorld())
		GetWorld()->GetTimerManager().SetTimer(MoveTrackingTimerHandle,
			this,&APangaeaPawn::FireMoveTracking,0.016667,true,0.f);
}

void APangaeaPawn::FireMoveTracking()
{
	MoveTracking();
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
		EnhancedInputComponent->BindAction(MInputActionMove, ETriggerEvent::Triggered, this, &APangaeaPawn::Move);
		EnhancedInputComponent->BindAction(MInputActionSpin, ETriggerEvent::Triggered, this, &APangaeaPawn::Spin);
		EnhancedInputComponent->BindAction(MInputActionZoom, ETriggerEvent::Triggered, this, &APangaeaPawn::Zoom);
		EnhancedInputComponent->BindAction(MInputActionDrag, ETriggerEvent::Triggered, this,&APangaeaPawn::DragMove);
		EnhancedInputComponent->BindAction(MInputActionVillager,ETriggerEvent::Triggered,this,&APangaeaPawn::HoverActor);
		EnhancedInputComponent->BindAction(MInputActionVillager,ETriggerEvent::Started,this,&APangaeaPawn::DragOrSelect);
		EnhancedInputComponent->BindAction(MInputActionVillager,ETriggerEvent::Completed,this,&APangaeaPawn::RemoveDragMappingContext);
		EnhancedInputComponent->BindAction(MInputActionVillager,ETriggerEvent::Canceled,this,&APangaeaPawn::RemoveDragMappingContext);
	}
}

void APangaeaPawn::VerifyPangaeaController()
{
	MPangaeaPlayerController=!MPangaeaPlayerController? Cast<APangaeaPlayerController>(Controller):MPangaeaPlayerController;
}

APlayerController* APangaeaPawn::GetPangaeaPlayerController() const
{
	return MPangaeaPlayerController;
}
