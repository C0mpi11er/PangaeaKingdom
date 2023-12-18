// Fill out your copyright notice in the Description page of Project Settings.


#include "PangaeaKingdom/Public/PangaeaPawn/PangaeaPawn.h"
#include "Components/InputComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "NavigationPath.h"
#include "Camera/CameraComponent.h"
#include "Components/SphereComponent.h"
#include "GameFramework/FloatingPawnMovement.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "PangaeaKingdom/Public/PangaeaController/PangaeaPlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "NiagaraFunctionLibrary.h"
#include "NavigationSystem.h"
#include "NiagaraComponent.h"
#include "Kismet/KismetArrayLibrary.h"
#include "NiagaraDataInterfaceArrayFunctionLibrary.h"
#include "Interfaces/VillagerInterface.h"
#include "Interactable/InteractableObject.h"
#include "Components/StaticMeshComponent.h"
#include "Components/BoxComponent.h"
#include "Kismet/KismetMaterialLibrary.h"
// Sets default values
APangaeaPawn::APangaeaPawn()
{
	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	MDefaultRootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
	SetRootComponent(MDefaultRootComponent);
	bGenerateOverlapEventsDuringLevelStreaming = true;
	MFloatMovementComp = CreateDefaultSubobject<UFloatingPawnMovement>(TEXT("FloatPawnMoveComp"));
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
	MCameraBoom->bEnableCameraLag = true;
	MCameraBoom->SocketOffset = FVector(-300.f, 0.f, 80.f);
	MCameraBoom->bEnableCameraRotationLag = true;
	MCameraBoom->CameraLagSpeed = 6.f;
	MCameraBoom->CameraRotationLagSpeed = 10.f;
	MCameraBoom->CameraLagMaxDistance = 0.f;
	MCameraBoom->bUseCameraLagSubstepping = true;
	MCameraBoom->CameraLagMaxTimeStep = 0.016667f;


	MCameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComp"));
	MCameraComp->SetupAttachment(MCameraBoom, MCameraBoom->SocketName);


	MCollisionSphereComp = CreateDefaultSubobject<USphereComponent>(TEXT("CollisionSphere"));
	MCollisionSphereComp->SetupAttachment(GetRootComponent());
	MCollisionSphereComp->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	MCollisionSphereComp->SetSphereRadius(180.f);
}

void APangaeaPawn::BeginBuild(TSubclassOf<AInteractableObject> InteractableClass)
{
	//this function is called from the ui interface to build selected interactable object
	if (!InteractableClass)return; //to be sure the blue print class is valid
	IPangaeaPawnInterface::BeginBuild(InteractableClass);
	if (MinterInteractableSpawnObject)
		MinterInteractableSpawnObject->Destroy(); //clear former interactable object

	//spawn the interactable at the position of the mouse
	const FTransform CursorTransform{UKismetMathLibrary::Conv_VectorToTransform(GetActorLocation())};
	MinterInteractableSpawnObject = GetWorld()->SpawnActor<AInteractableObject>(
		InteractableClass, CursorTransform);

	//ini the spawn class
	MInteractableToSpawnClass = InteractableClass;
	CreateBuildOverlay();
	//implement placement mode from interactable class
	
}

void APangaeaPawn::SwitchBuildMode(bool CanSwitchToBuildMode)
{
	//this function add and removes build mode mapping context depending on the can switch bool
	IPangaeaPawnInterface::SwitchBuildMode(CanSwitchToBuildMode);
	VerifyPangaeaController();
	VerifyEnhancedInputSubSystem();
	if(CanSwitchToBuildMode)
	{
		
			if (MEnhanceInputSubsystem)
			{
				MEnhanceInputSubsystem->RemoveMappingContext(MVillageInputMappingContext);
				MEnhanceInputSubsystem->AddMappingContext(MBuildInputMappingContext,0);
				UE_LOG(LogTemp,Warning,TEXT("Buildcontextadded"))
			}
	}
	else // done switching to build modes
	{
		
		if(MEnhanceInputSubsystem)
		{
			MEnhanceInputSubsystem->RemoveMappingContext(MBuildInputMappingContext);
			MEnhanceInputSubsystem->AddMappingContext(MVillageInputMappingContext,0);
			UE_LOG(LogTemp,Warning,TEXT("Buildcontextremoved"))
		}
	}
	
}

void APangaeaPawn::SpawnInteractableObject()
{
	if(bCanDropInteractable)
	{
		FActorSpawnParameters SpawnParameters;
		SpawnParameters.SpawnCollisionHandlingOverride=ESpawnActorCollisionHandlingMethod::Undefined;
		GetWorld()->SpawnActor<AInteractableObject>(MInteractableToSpawnClass,MinterInteractableSpawnObject->GetTransform(),SpawnParameters);
		UpdateBuildAsset();
	}
	
}

void APangaeaPawn::RotateSpawnInteractable()
{
	   //this function rotates the spawn object to 90 degrees
	if(MinterInteractableSpawnObject)
	{
		const FRotator SpawnRotator{MinterInteractableSpawnObject->GetActorRotation()};
		const FRotator TargetRotation{UKismetMathLibrary::ComposeRotators(SpawnRotator,FRotator(0.f,0.f,90.f))};
		MinterInteractableSpawnObject->SetActorRotation(TargetRotation);
	}
}




// Called when the game starts or when spawned
void APangaeaPawn::BeginPlay()
{
	Super::BeginPlay();
	UpdateZoom();
	StartMoveTrackTimer();
	//init player controller and input mapping context
	MPangaeaPlayerController = Cast<APangaeaPlayerController>(Controller);
	MPangaeaPlayerController = !MPangaeaPlayerController
		                           ? Cast<APangaeaPlayerController>(Controller)
		                           : MPangaeaPlayerController;
	if (MPangaeaPlayerController)
	{
		MEnhanceInputSubsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(
			MPangaeaPlayerController->GetLocalPlayer());
		if (MEnhanceInputSubsystem)
		{
			MEnhanceInputSubsystem->AddMappingContext(MInputMappingContext, 0);
			MEnhanceInputSubsystem->AddMappingContext(MVillageInputMappingContext, 0);
		}
	}

	//to bind the pangaea overlapping function and set the hover actor
	OnActorBeginOverlap.AddDynamic(this, &ThisClass::APangaeaPawn::OnPangaeaPawnBeginOverlap);
	OnActorEndOverlap.AddDynamic(this, &APangaeaPawn::OnPangaeaPawnEndOverlap);
	
}



void APangaeaPawn::CameraDepthOfField()
{
	FPostProcessSettings CameraDof;
	CameraDof.DepthOfFieldSensorWidth = 150.f;
	CameraDof.DepthOfFieldFocalDistance = MCameraBoom->TargetArmLength;
	CameraDof.DepthOfFieldFstop = 3.0;
	if (MCameraComp)
		MCameraComp->PostProcessSettings = CameraDof;
}

void APangaeaPawn::UpdateZoom()
{
	MZoomDirection *= 0.01f;
	MZoomValue = FMath::Clamp(MZoomDirection + MZoomValue, 0.f, 1.f);
	if (MZoomCurveFloat)
	{
		const float InterpolatingValue = MZoomCurveFloat->GetFloatValue(MZoomValue);
		if (MCameraBoom)
		{
			MCameraBoom->TargetArmLength = UKismetMathLibrary::Lerp(800.f, 40'000.f, InterpolatingValue);
			MCameraBoom->
				SetRelativeRotation(FRotator(UKismetMathLibrary::Lerp(-40, -55, InterpolatingValue), 0.f, 0.f));
		}
		if (MFloatMovementComp)
			MFloatMovementComp->MaxSpeed = UKismetMathLibrary::Lerp(1'000, 6'000, InterpolatingValue);
		CameraDepthOfField();
		if (MCameraComp)
			MCameraComp->SetFieldOfView(UKismetMathLibrary::Lerp(20.f, 15.f, InterpolatingValue));
	}
}

void APangaeaPawn::CreateBuildOverlay()
{
	if (!MSpawnOverlay)
	{
		FTransform SpawnTransform;
		if (MinterInteractableSpawnObject)
		{
			//get Spawn overlay relative transform from interactable object bound
			FVector Origin;
			FVector BoxExtent;
			MinterInteractableSpawnObject->GetActorBounds(false, Origin, BoxExtent,false);
			FVector TransFormScale{UKismetMathLibrary::Divide_VectorFloat(BoxExtent, 50.f)};
			SpawnTransform = UKismetMathLibrary::MakeTransform(FVector(), FRotator(), TransFormScale);
		}

		//initializes the spawn overlay
		MSpawnOverlay = Cast<UStaticMeshComponent>(AddComponentByClass(UStaticMesh::StaticClass(),true,SpawnTransform,true));
		
		if (MSpawnOverlay)
		{
			FAttachmentTransformRules Rules{
				EAttachmentRule::SnapToTarget,
				EAttachmentRule::KeepWorld, EAttachmentRule::KeepWorld, true
			};

			if (MinterInteractableSpawnObject)
			{
				MSpawnOverlay->AttachToComponent(MinterInteractableSpawnObject->GetMesh(), Rules);
				UpdateBuildAsset();
			}
			
		}
	}
}

bool APangaeaPawn::CheckSpawnNavCorners()
{
/*1)perform multi line trace for each section of the box and return true if all traces are valid false otherwise
 *X and Y pos
 *X and Y neg
 *X neh Y Pos
 *X pos Y neg
 */
	if(!MinterInteractableSpawnObject||!MinterInteractableSpawnObject->GetCollisionBox())return false;
	FVector BoxOrigin;
	FVector BoxExtent;
	MinterInteractableSpawnObject->GetActorBounds(true,BoxOrigin,BoxExtent,false);
	double BoxExtentX{BoxExtent.X*1.05};
	double BoxExtentY{BoxExtent.Y*1.05};
	double BoxNegExtentX{BoxExtentX*-1.f};
	double BoxNegExtentY{BoxExtentY*-1.f};

	
	/*Vectors Positive and negative box axis*/
	FVector PosXPosY{UKismetMathLibrary::Add_VectorVector(BoxOrigin,FVector(BoxExtentX,BoxExtentY,0.f))};
	FVector PosYNegX{UKismetMathLibrary::Add_VectorVector(BoxOrigin,FVector(BoxNegExtentX,BoxExtentY,0.f))};
	FVector NegXNegY{UKismetMathLibrary::Add_VectorVector(BoxOrigin,FVector(BoxNegExtentX,BoxNegExtentY,0.f))};
	//will debug and investigate this 
	FVector NegYPosY{UKismetMathLibrary::Add_VectorVector(BoxOrigin,FVector(BoxExtentY,BoxNegExtentY,0.f))};
	TArray<AActor*>ActorsToIgnore{MinterInteractableSpawnObject};
	//hit result for the first trace
	TArray<FHitResult>FirstHitResultArray;

bool FirstTrace{
	UKismetSystemLibrary::LineTraceMulti(this, FVector(PosXPosY.X, PosXPosY.Y, 100.f),
	                                     FVector(PosXPosY.X, PosXPosY.Y, -1.f), ETraceTypeQuery::TraceTypeQuery1, false,
	                                     ActorsToIgnore, EDrawDebugTrace::Persistent, FirstHitResultArray, true)
};
	
   TArray<FHitResult>SecondHitResultArray;
	bool SecondTrace{UKismetSystemLibrary::LineTraceMulti(this, FVector(PosYNegX.X, PosYNegX.Y, 100.f),
											 FVector(PosYNegX.X, PosYNegX.Y, -1.f), ETraceTypeQuery::TraceTypeQuery1, false,
											 ActorsToIgnore, EDrawDebugTrace::Persistent, SecondHitResultArray, true)};	
	

	TArray<FHitResult>ThirdHitResultArray;
	bool ThirdTrace{UKismetSystemLibrary::LineTraceMulti(this, FVector(NegXNegY.X, NegXNegY.Y, 100.f),
											 FVector(NegXNegY.X, NegXNegY.Y, -1.f), ETraceTypeQuery::TraceTypeQuery1, false,
											 ActorsToIgnore, EDrawDebugTrace::Persistent, ThirdHitResultArray, true)};

	TArray<FHitResult>FourthHitResultArray;
	bool FourthTrace{UKismetSystemLibrary::LineTraceMulti(this, FVector(NegYPosY.X, NegYPosY.Y, 100.f),
											 FVector(NegYPosY.X, NegYPosY.Y, -1.f), ETraceTypeQuery::TraceTypeQuery1, false,
											 ActorsToIgnore, EDrawDebugTrace::Persistent, FourthHitResultArray, true)};

	return FirstTrace&&SecondTrace&&ThirdTrace&&FourthTrace;
}

FVector APangaeaPawn::ConvertToSteppedPos(FVector& LocationVector)
{
	LocationVector = UKismetMathLibrary::Divide_VectorFloat(LocationVector, 200.f);
	LocationVector.Z = 0.f;
	int32 MouseX{UKismetMathLibrary::Round(LocationVector.X)};
	int32 MouseY{UKismetMathLibrary::Round(LocationVector.Y)};
	MouseX *= 200.f;
	MouseY *= 200.f;
	return UKismetMathLibrary::MakeVector(MouseX, MouseY, 0.f);
}

void APangaeaPawn::UpdateBuildAsset()
{
	/*function interpolate spawned object to mouse position and dictates if it can be dropped or not
	   1) getting the mouse on screen position and confriming valid trace
	   2)smoothly interpolating the spawned object position towards the mouse on screen position after accurate convertion
		of mouse Pos
	   */

	
	
	FMouseProjectionResult MouseScreenTrace{ProjectMouseToGround()};
	if (MouseScreenTrace.MValidProjection)
	{
		if (MinterInteractableSpawnObject && GetWorld())
		{
			FVector NewVector{
				UKismetMathLibrary::VInterpTo(MinterInteractableSpawnObject->GetActorLocation(),
				                              ConvertToSteppedPos(MouseScreenTrace.MIntersectionPoint),

				                              GetWorld()->GetDeltaSeconds(), 10.f)
			};

			MinterInteractableSpawnObject->SetActorLocation(NewVector);
			//if not overlapping anything and asset is within bounds,interactable asset can be spawned
			TArray<AActor*>OverlappingActors;
			MinterInteractableSpawnObject->GetOverlappingActors(OverlappingActors,AInteractableObject::StaticClass());
			//can spawn only when spawn is not overlapping other actors and is within island bound
			if(OverlappingActors.IsEmpty())
	 		{   
				
				bCanDropInteractable=CheckSpawnNavCorners();
			}
			else
			{
				bCanDropInteractable=false;
			}
			if(MMaterialParameterCollection)
			{
				//colors is red is not spawnable and green when spawnable
				FLinearColor Color;
				Color.R=NewVector.X;
				Color.G=NewVector.Y;
				Color.B=NewVector.Z;
				Color.A=bCanDropInteractable;
				UKismetMaterialLibrary::SetVectorParameterValue(this,MMaterialParameterCollection,FName{"ValidSpawn"},Color);
			}
			
		}
	}
}

void APangaeaPawn::DestroySpawnInteractableObject()
{
	if (MinterInteractableSpawnObject && MSpawnOverlay)
	{
		MinterInteractableSpawnObject->Destroy();
		MSpawnOverlay->DestroyComponent();
	}
}

void APangaeaPawn::Move(const FInputActionValue& InputActionValue)
{
	const FVector2d Movement = InputActionValue.Get<FVector2d>();

	if (Controller && Movement.Size() != 0.f)
		AddMovementInput(GetActorForwardVector(), Movement.Y);
	AddMovementInput(GetActorRightVector(), Movement.X);
}

void APangaeaPawn::Spin(const FInputActionValue& InputActionValue)
{
	const auto SpinDirection{InputActionValue.Get<float>()};
	if (Controller && SpinDirection != 0.f)
	{
		AddActorLocalRotation(FRotator(0.f, SpinDirection, 0.f));
	}
}

void APangaeaPawn::Zoom(const FInputActionValue& InputActionValue)
{
	if (Controller)
	{
		MZoomDirection = InputActionValue.Get<float>();
		UpdateZoom();
		CameraDepthOfField();
	}
}

void APangaeaPawn::DragMove(const FInputActionValue& InputActionValue)
{
	TrackMove();
}

void APangaeaPawn::DragOrSelect(const FInputActionValue& InputActionValue)
{
	//store mouse position in target handle
	CheckMouseScreenPosition();
	//check if overlapping with villager pawn or mere actor
	AActor* OverlappedVillager{VillagerOverlapCheck()};
	if (!OverlappedVillager) //no overlapping villager
	{
		//drag
		//check if controller is null or not
		VerifyPangaeaController();
		if (MPangaeaPlayerController)
		{
			MEnhanceInputSubsystem = !MEnhanceInputSubsystem
				                         ? ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(
					                         MPangaeaPlayerController->GetLocalPlayer())
				                         : MEnhanceInputSubsystem;


			if (MEnhanceInputSubsystem)
			{
				MEnhanceInputSubsystem->AddMappingContext(MDragInputMappingContext, 0);
			}
		}
	}
	else
	{
		//available overlapping villager
		VillagerSelect(Cast<APawn>(OverlappedVillager));
	}
}

void APangaeaPawn::RemoveDragMappingContext(const FInputActionValue& InputActionValue)
{
	VerifyPangaeaController();
	if (MPangaeaPlayerController)
	{
		MEnhanceInputSubsystem = !MEnhanceInputSubsystem
			                         ? ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(
				                         MPangaeaPlayerController->GetLocalPlayer())
			                         : MEnhanceInputSubsystem;

		if (MEnhanceInputSubsystem)
		{
			FModifyContextOptions ContextOptions;
			ContextOptions.bIgnoreAllPressedKeysUntilRelease = true;
			ContextOptions.bForceImmediately = true;
			MEnhanceInputSubsystem->RemoveMappingContext(MDragInputMappingContext, ContextOptions);
		}
	}


	if (MSelectedVillager)
	{
		if (IVillagerInterface* VillagerInterface{Cast<IVillagerInterface>(MSelectedVillager)})
		{
			VillagerInterface->VillagerAction(MVillagerAction);
		}
		VillagerRelease();
	}
	else
	{
		//there cant be an action to preform if there is no valid villager pawn selected
		MVillagerAction = nullptr;
	}
}

void APangaeaPawn::StartBuildMode()
{
	UpdateBuildAsset();
}


void APangaeaPawn::EndBuildMode()
{
	//stop update on spawn 
	if(MinterInteractableSpawnObject)
	{
		FVector NewLocation{MinterInteractableSpawnObject->GetActorLocation()};
		NewLocation=ConvertToSteppedPos(NewLocation);
		MinterInteractableSpawnObject->SetActorLocation(NewLocation);
	}
		
}

void APangaeaPawn::InitializeVillagerAction()
{
	//so the villager knows whats action to perform
	if (MHoverActor)
		MVillagerAction = MHoverActor;
}

FVector APangaeaPawn::CalculateCameraAndBoomOffset()
{
	if (!MCameraBoom || !MCameraComp)return FVector::ZeroVector;
	//get delta of target arm and socket
	//the scale by the forward vector
	const auto DeltaTargetArmSocketX{
		UKismetMathLibrary::Subtract_DoubleDouble(MCameraBoom->TargetArmLength, MCameraBoom->SocketOffset.X)
	};

	FVector ScaledArmLength{
		UKismetMathLibrary::Multiply_VectorFloat(MCameraBoom->GetForwardVector(), DeltaTargetArmSocketX * -1.f)
	};

	const FVector ScaledSocketZUpVector{
		UKismetMathLibrary::Multiply_VectorFloat(MCameraBoom->GetUpVector(), MCameraBoom->SocketOffset.Z)
	};


	FVector SpringArmVector{UKismetMathLibrary::Add_VectorVector(ScaledArmLength, ScaledSocketZUpVector)};
	SpringArmVector = UKismetMathLibrary::Add_VectorVector(SpringArmVector, MCameraBoom->GetComponentLocation());
	return UKismetMathLibrary::Subtract_VectorVector(SpringArmVector, MCameraComp->GetComponentLocation());
}

void APangaeaPawn::CheckAndSetClosetHoverActor()
{
	if (!MCollisionSphereComp)return;
	TArray<AActor*> OverlappingActors;
	//used to set the new actor that is being collision with temporarily
	AActor* NewHover{nullptr};
	MCollisionSphereComp->GetOverlappingActors(OverlappingActors, AActor::StaticClass());
	if (OverlappingActors.IsEmpty())
	{
		GetWorldTimerManager().PauseTimer(MClosetHoverTimerHandle);
		UE_LOG(LogTemp, Warning, TEXT("Timer paused"))
	}

	else
	{
		if (!NewHover)
		{
			//find the distance of overlapping actors and find the closest one
			for (const auto& OverlappingActor : OverlappingActors)
			{
				//set new actor tot the first array index
				if (OverlappingActor == OverlappingActors[0])
					NewHover = OverlappingActor;

				//continue looping through the array to find a lesser vector than the new hover
				const FVector DistanceSphere{MCollisionSphereComp->GetComponentLocation()};
				const FVector DistanceNewHoverActor{NewHover->GetActorLocation()};
				const FVector DistanceOverlappingActor{OverlappingActor->GetActorLocation()};
				//get the lesser distance and init new hover actor if there is any
				if (UKismetMathLibrary::Vector_Distance(DistanceSphere, DistanceOverlappingActor) <
					UKismetMathLibrary::Vector_Distance(DistanceSphere, DistanceNewHoverActor))
				{
					NewHover = OverlappingActor;
				}
			}
		}
		if (NewHover != MHoverActor)SetHoverActor(std::move(NewHover));
		//if new hover is same as hover do nothing except otherwise
	}
}


void APangaeaPawn::CheckMouseScreenPosition()
{
	//store mouse deprojected position and store in target handle
	MMouseTargetHandle = ProjectMouseToGround().MIntersectionPoint;
}

FMouseProjectionResult APangaeaPawn::ProjectMouseToGround()
{
	FMouseProjectionResult MouseProjectionResult;
	FVector2d ViewportSize;
	VerifyPangaeaController();
	if (MPangaeaPlayerController)
	{
		int32 ViewX;
		int32 ViewY;
		MPangaeaPlayerController->GetViewportSize(ViewX, ViewY);
		ViewportSize.X = ViewX;
		ViewportSize.Y = ViewY;
	}
	const FVector2d ViewportCenter{UKismetMathLibrary::Divide_Vector2DFloat(ViewportSize, 2.f)};

	VerifyPangaeaController();
	if (MPangaeaPlayerController)
	{
		//init the mouse position inside the vector and exit function if mouse is not valid
		FVector2d MousePosition;
		double MouseX;
		double MouseY;
		if (MPangaeaPlayerController->GetMousePosition(MouseX, MouseY))
		{
			//theres is a mouse device
			MouseProjectionResult.MValidProjection = true;
			//init a 2d vector position
			MousePosition = UKismetMathLibrary::MakeVector2D(MouseX, MouseY);
		}
		else
		{
			MouseProjectionResult.MValidProjection = false; //there is no available mouse device
			MousePosition = ViewportCenter; //assume the mouse position is the viewport center
		}

		// init the screen pos
		MouseProjectionResult.MScreenPos = MousePosition;
		FVector WorldPosition;
		FVector WorldDirection;
		UGameplayStatics::DeprojectScreenToWorld(MPangaeaPlayerController, MousePosition, WorldPosition,
		                                         WorldDirection);
		float InterSectionFraction; // when multiplied by the line trace it give you length from start tp end
		FVector MousePlaneIntersectionPoint; //make plane to intersect
		const FPlane InterSectionPlane{
			UKismetMathLibrary::MakePlaneFromPointAndNormal(FVector(0.f, 0.f, 0.f),
			                                                FVector(0.f, 0.f, 1.f))
		};

		const FVector LineStart{WorldPosition}; //were the intersection trace starts
		const FVector LineEnd{UKismetMathLibrary::Add_VectorVector(WorldPosition, WorldDirection * 100'000.f)};
		UKismetMathLibrary::LinePlaneIntersection(LineStart, LineEnd,
		                                          InterSectionPlane,
		                                          InterSectionFraction,
		                                          MousePlaneIntersectionPoint);

		//init the mouse intersection point
		MouseProjectionResult.MIntersectionPoint = MousePlaneIntersectionPoint;
	}
	return MouseProjectionResult;
}

void APangaeaPawn::TrackMove()
{
	const FVector CameraAndBoomDelta{CalculateCameraAndBoomOffset()}; //the delta between camera and camera boom
	const FMouseProjectionResult MouseProjectionResult{ProjectMouseToGround()};

	FVector MouseTargetDelta{
		UKismetMathLibrary::Subtract_VectorVector(MMouseTargetHandle,
		                                          MouseProjectionResult.MIntersectionPoint)
	};
	MouseTargetDelta = UKismetMathLibrary::Subtract_VectorVector(MouseTargetDelta, CameraAndBoomDelta);
	MouseTargetDelta.Z = 0.f;

	if (MouseProjectionResult.MValidProjection)
	{
		//move pawn towards mouse directions
		AddActorWorldOffset(MouseTargetDelta);
	}
}

void APangaeaPawn::UpdateCursorPosition()
{
	//this function sets and interpolates the cursor towards a target location
	//if the hover actor is valid that become the location otherwise the collision sphere location
	if (!MCollisionSphereComp || !MCursor)return;

	FTransform Target;
	if(MHoverActor)
	{
		FVector HoverActorOrigin;
		FVector HoverActorBoxExtent;
		MHoverActor->GetActorBounds(true,HoverActorOrigin,HoverActorBoxExtent);
		HoverActorOrigin=FVector(HoverActorOrigin.X,HoverActorOrigin.Y,20.f);

		//make the box extent a 2d vector
		const FVector2d BoxExtent2d{UKismetMathLibrary::MakeVector2D(HoverActorBoxExtent.X,HoverActorBoxExtent.Y)};
		double AbsoluteBoxExtent{UKismetMathLibrary::GetAbsMax2D(BoxExtent2d)};
		AbsoluteBoxExtent/=50.f;

		//scale indicator up and down
		double GameTime{UKismetSystemLibrary::GetGameTimeInSeconds(this)};
		GameTime*=5.0f;
		double SinedGameTimeValue{UKismetMathLibrary::Sin(GameTime)};
		SinedGameTimeValue*=0.25;

		double HoverActorScale{AbsoluteBoxExtent+SinedGameTimeValue};
		HoverActorScale+=1.f;
		
		FTransform HoveredActorTransform{UKismetMathLibrary::MakeTransform(HoverActorOrigin,
			FRotator(),
			FVector(HoverActorScale,
				HoverActorScale,1.f))};

		Target=HoveredActorTransform;
	}
	else
	{
		const FTransform CollisionSphereTransform{MCollisionSphereComp->GetComponentTransform()};
		Target = UKismetMathLibrary::MakeTransform(CollisionSphereTransform.GetLocation(),
			                                  CollisionSphereTransform.Rotator(), FVector(2.f, 2.f, 1.f));
		  
	}
	const FTransform InterpolatedTransform{
		UKismetMathLibrary::TInterpTo(MCursor->GetComponentTransform(),
		                              Target,
		                              UGameplayStatics::GetWorldDeltaSeconds(GetWorld()),
		                              12.0)
	};
	MCursor->SetWorldTransform(InterpolatedTransform);
}

void APangaeaPawn::MoveTracking()
{
	FVector NormalizedActorLoc{UKismetMathLibrary::Normal(GetActorLocation())};
	NormalizedActorLoc.Z = 0.f; //not needed because plan is 2d
	//scaled by -1
	NormalizedActorLoc = UKismetMathLibrary::Multiply_VectorFloat(NormalizedActorLoc, -1.0f);


	double ActorLocVectorLength{GetActorLocation().Length()};
	ActorLocVectorLength = UKismetMathLibrary::Subtract_DoubleDouble(ActorLocVectorLength, 9000.f);
	ActorLocVectorLength = UKismetMathLibrary::Divide_DoubleDouble(ActorLocVectorLength, 5000.f);
	const int32 ScaleValue{UKismetMathLibrary::Max(ActorLocVectorLength, 0.0)};
	AddMovementInput(NormalizedActorLoc, ScaleValue);
	//sync cursor and collision position
	UpdateCursorPosition();
	//edge screen movement 
	const FCursorDistanceFromViewportResult CursorScreenMovement{EdgeMove()};
	AddMovementInput(CursorScreenMovement.MDirection, CursorScreenMovement.MStrength);
	if (MCollisionSphereComp)
	{
		//position collision on ground plane.
		const FMouseProjectionResult MouseProjectionResult{ProjectMouseToGround()};

		const FVector NewLocation{
			UKismetMathLibrary::Add_VectorVector(MouseProjectionResult.MIntersectionPoint,
			                                     FVector(0.f, 0.f, 10.f))
		};
		MCollisionSphereComp->SetWorldLocation(NewLocation);
	}
}

void APangaeaPawn::StartMoveTrackTimer()
{
	if (GetWorld())
		GetWorld()->GetTimerManager().SetTimer(MoveTrackingTimerHandle,
		                                       this, &APangaeaPawn::FireMoveTracking, 0.016667, true, 0.f);
}

void APangaeaPawn::FireMoveTracking()
{
	MoveTracking();
}

FCursorDistanceFromViewportResult APangaeaPawn::CursorDistanceFromViewportCentre(const FVector2d& CursorPosOffset)
{
	//Viewport half size edge detect distance

	//scale the mouse movement input by the edge move distance
	constexpr float MouseMoveInput{1.0f};
	const double ScaledEdgeMoveDist{UKismetMathLibrary::Multiply_DoubleDouble(MEdgeMoveDistance, MouseMoveInput)};

	//detect edge move distance
	VerifyPangaeaController();
	int32 SizeX{};
	int32 SizeY{};
	if (MPangaeaPlayerController)
	{
		MPangaeaPlayerController->GetViewportSize(SizeX, SizeY);
		SizeX = UKismetMathLibrary::Divide_DoubleDouble(SizeX, 2.0f);
		SizeY = UKismetMathLibrary::Divide_DoubleDouble(SizeY, 2.0f);
		//sub the x and y value by the scaled edge distance 
		SizeY = UKismetMathLibrary::Subtract_DoubleDouble(SizeY, ScaledEdgeMoveDist);
		SizeX = UKismetMathLibrary::Subtract_DoubleDouble(SizeX, ScaledEdgeMoveDist);
	}

	//offset track mouse position to create dead zone
	double AbsMouseX{UKismetMathLibrary::Abs(CursorPosOffset.X)};
	double AbsMouseY{UKismetMathLibrary::Abs(CursorPosOffset.Y)};
	AbsMouseX = UKismetMathLibrary::Subtract_DoubleDouble(AbsMouseX, SizeX);
	AbsMouseY = UKismetMathLibrary::Subtract_DoubleDouble(AbsMouseY, SizeY);

	//ensure the max value is between the abs mouse value and 0.f
	double MaxMouseX{FMath::Max(AbsMouseX, 0.0f)};
	double MaxMouseY{FMath::Max(AbsMouseY, 0.0f)};
	MaxMouseX = UKismetMathLibrary::Divide_DoubleDouble(MaxMouseX, MEdgeMoveDistance);
	MaxMouseY = UKismetMathLibrary::Divide_DoubleDouble(MaxMouseY, MEdgeMoveDistance);


	double SignedCursorX{FMath::Sign(CursorPosOffset.X)};
	double SignedCursorY{FMath::Sign(CursorPosOffset.Y)};
	SignedCursorX = UKismetMathLibrary::Multiply_DoubleDouble(SignedCursorX, MaxMouseX);
	SignedCursorY = UKismetMathLibrary::Multiply_DoubleDouble(SignedCursorY, MaxMouseY * -1.0f);


	const FVector Direction{UKismetMathLibrary::MakeVector(SignedCursorY, SignedCursorX, 0.0f)};

	const FCursorDistanceFromViewportResult Result{Direction, 1.0f};
	return Result;
}

FCursorDistanceFromViewportResult APangaeaPawn::EdgeMove()
{
	FVector2d ScreenPos{ProjectMouseToGround().MScreenPos};
	FCursorDistanceFromViewportResult Result;
	//get viewport half height
	VerifyPangaeaController();
	if (MPangaeaPlayerController)
	{
		int32 SizeX;
		int32 SizeY;
		MPangaeaPlayerController->GetViewportSize(SizeX, SizeY);
		FVector2d ViewportCenter = UKismetMathLibrary::MakeVector2D(SizeX, SizeY);
		ViewportCenter.Y = UKismetMathLibrary::Divide_DoubleDouble(ViewportCenter.Y, 2.0f);
		ViewportCenter.X = UKismetMathLibrary::Divide_DoubleDouble(ViewportCenter.X, 2.0f);
		//get delta of mouse  and viewport half height
		ScreenPos = UKismetMathLibrary::Subtract_Vector2DVector2D(ScreenPos, ViewportCenter);
		const FCursorDistanceFromViewportResult CursorResult{CursorDistanceFromViewportCentre(ScreenPos)};
		const FVector Direction{UKismetMathLibrary::TransformDirection(GetActorTransform(), CursorResult.MDirection)};
		Result.MDirection = Direction;
		Result.MStrength = CursorResult.MStrength;
	}
	return Result;
}

void APangaeaPawn::OnPangaeaPawnBeginOverlap(AActor* OverlappedActor, AActor* OtherActor)
{
	if (!MHoverActor)MHoverActor = OtherActor;
	//check for the closest hovering actor to player collision sphere
	StartClosestHoverCheckTimer();
}

void APangaeaPawn::OnPangaeaPawnEndOverlap(AActor* OverlappedActor, AActor* OtherActor)
{
	//check for overlapping actors and set hover actor to null if non was found.
	TArray<AActor*> OverlappingActors;
	GetOverlappingActors(OverlappingActors, AActor::StaticClass());
	if (OverlappingActors.IsEmpty())
		MHoverActor = nullptr;
}

void APangaeaPawn::StartClosestHoverCheckTimer()
{
	UE_LOG(LogTemp, Warning, TEXT("timer started"))
	GetWorldTimerManager().SetTimer(MClosetHoverTimerHandle, this, &APangaeaPawn::CheckAndSetClosetHoverActor,
	                                MCloseHoverLoopTime, true);
}

void APangaeaPawn::UpdatePathway()
{
	//init the nav mesh bound actor in world
	UNavigationSystemV1* NavMesh{FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld())};
	if (NavMesh && MCollisionSphereComp && MSelectedVillager)
	{
		//get the pathway from the start to end
		FVector StartPath{MCollisionSphereComp->GetComponentLocation()};
		FVector EndPath{MSelectedVillager->GetActorLocation()};
		UNavigationPath* NavigationPath{NavMesh->FindPathToLocationSynchronously(this, StartPath, EndPath)};
		if (NavigationPath && NavigationPath->PathPoints.IsValidIndex(1)) //to be sure there is a start and end point
		{
			//init the array of vectors in the path
			MPathPoints = NavigationPath->PathPoints;
			//init the first and end array index with the start and end vectors
			MPathPoints[0] = MCollisionSphereComp->GetComponentLocation();
			MPathPoints.Last() = MSelectedVillager->GetActorLocation();
			if (MNiagaraComponentPath)
				UNiagaraDataInterfaceArrayFunctionLibrary::SetNiagaraArrayVector(
					MNiagaraComponentPath, FName{"TargetPath"}, MPathPoints);
		}
	}
}

AActor* APangaeaPawn::VillagerOverlapCheck()
{
	//to confirm if actor is really overlapping with a pawn
	TArray<AActor*> OverlappingActors;
	GetOverlappingActors(OverlappingActors, APawn::StaticClass());
	//to be sure the the array of overlap actors are not empty
	if (!OverlappingActors.IsEmpty())
	{
		for (auto& Actor : OverlappingActors)
		{
			if (Actor && Actor == OverlappingActors[0])
				return Actor;
		}
	}
	return nullptr;
}

void APangaeaPawn::VillagerSelect(APawn* PangaeaPawn)
{
	if (!PangaeaPawn)return;
	//init selected villager
	MSelectedVillager = std::move(PangaeaPawn);
	if (MNiagaraTargetSystem)
	{
		MNiagaraComponentPath = UNiagaraFunctionLibrary::SpawnSystemAttached(MNiagaraTargetSystem,
		                                                                     GetRootComponent(), FName{""},
		                                                                     FVector(),
		                                                                     FRotator(),
		                                                                     EAttachLocation::SnapToTarget, false);
		StartUpdatePathTimer();
	}
}

void APangaeaPawn::VillagerRelease()
{
	StopUpdatePathTimer();
}


void APangaeaPawn::StartUpdatePathTimer()
{
	GetWorldTimerManager().SetTimer(MUpdatePathTimerHandle,
	                                this, &APangaeaPawn::UpdatePathway,
	                                MUpdatePathResetTime, true);
}

void APangaeaPawn::StopUpdatePathTimer()
{
	GetWorldTimerManager().PauseTimer(MUpdatePathTimerHandle);
	if (MNiagaraComponentPath)
		MNiagaraComponentPath->DestroyComponent();
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
	if (UEnhancedInputComponent* EnhancedInputComponent{CastChecked<UEnhancedInputComponent>(PlayerInputComponent)})
	{
		EnhancedInputComponent->BindAction(MInputActionMove, ETriggerEvent::Triggered, this, &APangaeaPawn::Move);
		EnhancedInputComponent->BindAction(MInputActionSpin, ETriggerEvent::Triggered, this, &APangaeaPawn::Spin);
		EnhancedInputComponent->BindAction(MInputActionZoom, ETriggerEvent::Triggered, this, &APangaeaPawn::Zoom);
		EnhancedInputComponent->BindAction(MInputActionDrag, ETriggerEvent::Triggered, this, &APangaeaPawn::DragMove);
		EnhancedInputComponent->BindAction(MInputActionVillager, ETriggerEvent::Triggered, this,
		                                   &APangaeaPawn::InitializeVillagerAction);
		EnhancedInputComponent->BindAction(MInputActionVillager, ETriggerEvent::Started, this,
		                                   &APangaeaPawn::DragOrSelect);
		EnhancedInputComponent->BindAction(MInputActionVillager, ETriggerEvent::Completed, this,
		                                   &APangaeaPawn::RemoveDragMappingContext);
		EnhancedInputComponent->BindAction(MInputActionVillager, ETriggerEvent::Canceled, this,
		                                   &APangaeaPawn::RemoveDragMappingContext);
		EnhancedInputComponent->BindAction(MInputActionBuild, ETriggerEvent::Triggered, this,
		                                   &APangaeaPawn::StartBuildMode);

		EnhancedInputComponent->BindAction(MInputActionBuild, ETriggerEvent::Completed, this,
		                                   &APangaeaPawn::EndBuildMode);
	}
}

void APangaeaPawn::VerifyPangaeaController()
{
	MPangaeaPlayerController = !MPangaeaPlayerController
		                           ? Cast<APangaeaPlayerController>(Controller)
		                           : MPangaeaPlayerController;
}

void APangaeaPawn::VerifyEnhancedInputSubSystem()
{
	if (!MPangaeaPlayerController)return;
	MEnhanceInputSubsystem = !MEnhanceInputSubsystem
		                         ? ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(
			                         MPangaeaPlayerController->GetLocalPlayer())
		                         : MEnhanceInputSubsystem;
	
}

APlayerController* APangaeaPawn::GetPangaeaPlayerController() const
{
	return MPangaeaPlayerController;
}
