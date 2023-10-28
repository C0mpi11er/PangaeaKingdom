// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "InputActionValue.h"
#include "UObject/ObjectPtr.h"
#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "PangaeaPawn.generated.h"

//used to store result for mouse projection to plane
USTRUCT()
struct FMouseProjectionResult
{
	GENERATED_BODY()
	FVector2d MScreenPos{};
	FVector MIntersectionPoint{};
	bool MValidProjection{false};
};


UCLASS()
class PANGAEAKINGDOM_API APangaeaPawn : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	APangaeaPawn();

	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	void CameraDepthOfField();
	//called to update the zoom
	void UpdateZoom();

    // Called every frame
 	virtual void Tick(float DeltaTime) override;
	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	//to check if controller is valid and set it
	void VerifyPangaeaController();
	//BindInputActions
	UFUNCTION()
	void Move(const FInputActionValue&InputActionValue);
	void Spin(const FInputActionValue&InputActionValue);
	void Zoom(const FInputActionValue&InputActionValue);
	void DragMove(const FInputActionValue&InputActionValue);
	//used to select village if valid or drag other wise
	void DragOrSelect();
	//to remove the drag mapping context
	void RemoveDragMappingContext();

	//called weh villager mapping context is triggered
	void HoverActor();
	//calculates the offset between the camera and spring arm if using lag
	FVector CalculateCameraAndBoomOffset();

	//use to check the current mouse and set it 
	void CheckMouseScreenPosition();

	//use to project the mouse vector on the screen towards thr ground plane 
	FMouseProjectionResult ProjectMouseToGround();
	//adds delta to the position of this actor in wold space
	void TrackMove();
	//interpolate teh cursor position from one point to another
	void  UpdateCursorPosition();

	//this function is called 0.1 sec to updated the cursor position with the mouse
	void MoveTracking();

	//fires time used to call the move tracking function
	void StartMoveTrackTimer();

	//callback for move tracking timer that called the move tracking function
	UFUNCTION()
	void FireMoveTracking();

	
private:
	//to select Actors
	UPROPERTY(EditAnywhere)
	TObjectPtr<UStaticMeshComponent> MCursor;
    //to give an isometric view 
	UPROPERTY(EditAnywhere)
	TObjectPtr<class USpringArmComponent> MCameraBoom;

	UPROPERTY(EditAnywhere)
    TObjectPtr<class UCameraComponent>MCameraComp;

	//to detect Pawn collision
	UPROPERTY(EditAnywhere)
	TObjectPtr<class USphereComponent>MCollisionSphereComp;

	/*MappingContext*/
	//mappintcontext for basic pawn movement
	UPROPERTY(EditAnywhere)
	TObjectPtr<class UInputMappingContext>MInputMappingContext;

	//this mapping context for villager mode
   UPROPERTY(EditAnywhere)
	TObjectPtr<class UInputMappingContext>MVillageInputMappingContext;

	//mapping context for drag
	UPROPERTY(EditAnywhere)
	TObjectPtr<class UInputMappingContext>MDragInputMappingContext;
    /*/MappingContext*/

	/*Input Actions*/
	UPROPERTY(EditAnywhere)
	TObjectPtr<class UInputAction>MInputActionMove;

	//pointer to input action spin
	UPROPERTY(EditAnywhere)
	TObjectPtr<class UInputAction>MInputActionSpin;

	//pointer to input action zoom
	UPROPERTY(EditAnywhere)
	TObjectPtr<class UInputAction>MInputActionZoom;

	//pointer to input action drag
	UPROPERTY(EditAnywhere)
	TObjectPtr<class UInputAction>MInputActionDrag;

	//pointer to input action villager
	UPROPERTY(EditAnywhere)
	TObjectPtr<class UInputAction>MInputActionVillager;

	/*/InputAction*/

	UPROPERTY(EditAnywhere)
	TObjectPtr<class UFloatingPawnMovement>MFloatMovementComp;

	//a player controller for the pangaea pawn
	UPROPERTY()
	TObjectPtr<class APangaeaPlayerController>MPangaeaPlayerController;

	//to add and remove context mappings
	UPROPERTY()
	TObjectPtr<class UEnhancedInputLocalPlayerSubsystem>MEnhanceInputSubsystem;
	
    //to know if the player is zooming in or out
	float MZoomDirection{};

	float MZoomValue{0.5};
   //to return figures smoothly for interpolation at diff points in time
	UPROPERTY(EditAnywhere)
	TObjectPtr<class UCurveFloat>MZoomCurveFloat;

	//the position of the mouse on the screen
     FVector MMouseTargetHandle{};

	
	//to keep track of when to begin move tracking on begin play
	FTimerHandle MoveTrackingTimerHandle;
public:
	
	//returns and cast pangaea Pawn controller to a player controller
	APlayerController*GetPangaeaPlayerController() const;
	
};
