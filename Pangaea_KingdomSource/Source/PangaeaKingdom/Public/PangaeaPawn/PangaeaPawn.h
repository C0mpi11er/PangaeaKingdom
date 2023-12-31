// Fill out your copyright notice in the Description page of Project Settings.
/*this class is the representation of the player in the game
 * this class informs villagers on task to perform by clicking and dragging towards respective interact-ables in world
 */
#pragma once

#include "PangaeaPawn/Interfaces/PangaeaPawnInterface.h"
#include "InputActionValue.h"
#include "UObject/ObjectPtr.h"
#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "PangaeaKingdom/PangaeaKingdom.h"
#include "PangaeaPawn.generated.h"

enum class EGameInputType : uint8;
//used to store result for mouse projection to plane
USTRUCT()
struct FMouseProjectionResult
{
	GENERATED_BODY()
	FVector2d MScreenPos{};
	FVector MIntersectionPoint{};
	bool MValidProjection{false};
};

USTRUCT()
struct FCursorDistanceFromViewportResult
{
	GENERATED_BODY()
	// the direction of the mouse cursor 
	FVector MDirection{};

	float MStrength{};
};


UCLASS()
class PANGAEAKINGDOM_API APangaeaPawn : public APawn, public IPangaeaPawnInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	APangaeaPawn();
	/*interfaces*/
	virtual void BeginBuild(TSubclassOf<AInteractableObject> InteractableClass) override;
	virtual void SwitchBuildMode(bool CanSwitchToBuildMode) override;

	//to destroy the interactable object 
	void DestroySpawnInteractableObject();
	//this function spawn the build target selected from the build ui
	void SpawnInteractableObject();
    //rotate spawn interactable object object
	void RotateSpawnInteractable();
    
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void PossessedBy(AController* NewController) override;
	//called to switch the input type of game
	UFUNCTION()
	void InputSwitch(EGameInputType GameInput);
	void CameraDepthOfField();
	//called to update the zoom
	void UpdateZoom();
	//initialize the spawn overlay
	void CreateBuildOverlay();
	//to do a boolean check if spawn interactable is on island 
	bool CheckSpawnNavCorners();
	//converts mouse on screen movement to accurate x and y axis fvector locations
	 FVector ConvertToSteppedPos(FVector&LocationVector);
	//updates the spawn intractable location in ref to mouse on screen pos
	void UpdateBuildAsset();
	

	// Called every frame
	virtual void Tick(float DeltaTime) override;
	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	//to check if controller is valid and set it
	void VerifyPangaeaController();
	//to check if enhanced input subsystem is valid and set it 
	void VerifyEnhancedInputSubSystem();
	//BindInputActions
	UFUNCTION()
	void Move(const FInputActionValue& InputActionValue);
	void Spin(const FInputActionValue& InputActionValue);
	void Zoom(const FInputActionValue& InputActionValue);
	void DragMove(const FInputActionValue& InputActionValue);
	//used to select village if valid or drag other wise
	void DragOrSelect(const FInputActionValue& InputActionValue);
	//to remove the drag mapping context
	void RemoveDragMappingContext(const FInputActionValue& InputActionValue);
	//used to triggered the build mode input action,also updates the build asset
	void StartBuildMode();

	//this function is called when the build mode IA is complete is also stops the build asset update
	void EndBuildMode();

	//called weh villager mapping context is triggered
	void InitializeVillagerAction();
	//calculates the offset between the camera and spring arm if using lag
	FVector CalculateCameraAndBoomOffset();
	//this function check for the closest actor this class is hovering over and sets it to the hover actor
	void CheckAndSetClosetHoverActor();

	//use to check the current mouse and set it 
	void CheckMouseScreenPosition();

	//use to project the mouse vector on the screen towards thr ground plane 
	FMouseProjectionResult ProjectMouseToGround();
	//adds delta to the position of this actor in wold space
	void TrackMove();
	//interpolate teh cursor position from one point to another
	void UpdateCursorPosition();

	//this function is called 0.1 sec to updated the cursor position with the mouse
	void MoveTracking();

	//fires time used to call the move tracking function
	void StartMoveTrackTimer();

	//callback for move tracking timer that called the move tracking function
	UFUNCTION()
	void FireMoveTracking();

	FCursorDistanceFromViewportResult CursorDistanceFromViewportCentre(const FVector2d& CursorPosOffset);

	//edge screen movement
	FCursorDistanceFromViewportResult EdgeMove();


	//this function is triggered when this pawn overlaps with another actor
	UFUNCTION()
	void OnPangaeaPawnBeginOverlap(AActor* OverlappedActor, AActor* OtherActor);

	UFUNCTION()
	void OnPangaeaPawnEndOverlap(AActor* OverlappedActor, AActor* OtherActor);

	//triggers the hover closest loop
	UFUNCTION()
	void StartClosestHoverCheckTimer();

	//to update and spawn niagara pathway effect from selected villager to task
	//update the path for villager to take to perform a given task
	UFUNCTION()
	void UpdatePathway();
	//this function check if the hover actor is on a villager or not
	AActor* VillagerOverlapCheck();

	//this functions spawns the niagara path system and calls the update path function
	void VillagerSelect(APawn* PangaeaPawn);

	//to release the selected villager to perform an action if there is one
	void VillagerRelease();


	//starts update part timer and loops till selected villager is released
	void StartUpdatePathTimer();

	//stops the update pathway function from looping
	UFUNCTION()
	void StopUpdatePathTimer();

private:
    //use to init if the interactable object can be dropped or not in begin build
	bool bCanDropInteractable{false};
	//to select Actors
	UPROPERTY(EditAnywhere)
	TObjectPtr<UStaticMeshComponent> MCursor;
	//to give an isometric view 
	UPROPERTY(EditAnywhere)
	TObjectPtr<class USpringArmComponent> MCameraBoom;

	EGameInputType M_GameInputType{EGameInputType::EGT_Gamepad};
	 
	UPROPERTY(EditAnywhere)
	TObjectPtr<class UCameraComponent> MCameraComp;

	//to detect Pawn collision
	UPROPERTY(EditAnywhere)
	TObjectPtr<class USphereComponent> MCollisionSphereComp;

	/*MappingContext*/
	//mappintcontext for basic pawn movement
	UPROPERTY(EditAnywhere)
	TObjectPtr<class UInputMappingContext> MInputMappingContext;

	//this mapping context for villager mode
	UPROPERTY(EditAnywhere)
	TObjectPtr<class UInputMappingContext> MVillageInputMappingContext;

	//mapping context for drag
	UPROPERTY(EditAnywhere)
	TObjectPtr<class UInputMappingContext> MDragInputMappingContext;

	//mapping context for building mode
	UPROPERTY(EditAnywhere)
	TObjectPtr<UInputMappingContext> MBuildInputMappingContext;
	/*/MappingContext*/

	/*Input Actions*/
	UPROPERTY(EditAnywhere)
	TObjectPtr<class UInputAction> MInputActionMove;

	//pointer to input action spin
	UPROPERTY(EditAnywhere)
	TObjectPtr<class UInputAction> MInputActionSpin;

	//pointer to input action zoom
	UPROPERTY(EditAnywhere)
	TObjectPtr<class UInputAction> MInputActionZoom;

	//pointer to input action drag
	UPROPERTY(EditAnywhere)
	TObjectPtr<class UInputAction> MInputActionDrag;

	//pointer to input action villager
	UPROPERTY(EditAnywhere)
	TObjectPtr<class UInputAction> MInputActionVillager;

	//input action for build mode
	UPROPERTY(EditAnywhere)
	TObjectPtr<UInputAction> MInputActionBuild;
	/*/InputAction*/

	UPROPERTY(EditAnywhere)
	TObjectPtr<class UFloatingPawnMovement> MFloatMovementComp;

	//a player controller for the pangaea pawn
	UPROPERTY()
	TObjectPtr<class APangaeaPlayerController> MPangaeaPlayerController;

	//to add and remove context mappings
	UPROPERTY()
	TObjectPtr<class UEnhancedInputLocalPlayerSubsystem> MEnhanceInputSubsystem;

	//to know if the player is zooming in or out
	float MZoomDirection{};

	float MZoomValue{0.5};
	//to return figures smoothly for interpolation at diff points in time
	UPROPERTY(EditAnywhere)
	TObjectPtr<class UCurveFloat> MZoomCurveFloat;

	//the position of the mouse on the screen
	FVector MMouseTargetHandle{};


	//to keep track of when to begin move tracking on begin play
	FTimerHandle MoveTrackingTimerHandle;

	//to be scaled with mouse move input value during edge move distance
	float MEdgeMoveDistance{50.0};

	//the actor the Pangaea_pawn is overing over
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<AActor> MHoverActor;

	//the timer that starts and pauses the looping closets actor check function
	FTimerHandle MClosetHoverTimerHandle;

	//the timer reset on every loop time completion
	float MCloseHoverLoopTime{0.01};

	//the niagara system that spawns when a villager is selected
	UPROPERTY(EditAnywhere)
	TObjectPtr<class UNiagaraSystem> MNiagaraTargetSystem;

	//to identify the villager path and target,pointer to the niagara target system
	UPROPERTY(EditAnywhere)
	TObjectPtr<class UNiagaraComponent> MNiagaraComponentPath;

	//pointer to the currently selected villager
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<class AActor> MSelectedVillager;

	UPROPERTY(EditAnywhere)
	TObjectPtr<USceneComponent> MDefaultRootComponent;

	//paths vector array on nav mesh  from Cursor to Selected villager
	UPROPERTY()
	TArray<FVector> MPathPoints;

	//handles the timer for the update pathway function
	FTimerHandle MUpdatePathTimerHandle;
	//after this time the functions triggers again
	float MUpdatePathResetTime{0.01f};
	//informs the villager on the job type to perform
	UPROPERTY()
	TObjectPtr<AActor> MVillagerAction;

	//this is the pointer to the spawned interactable class
	UPROPERTY()
	TObjectPtr<class AInteractableObject> MinterInteractableSpawnObject;

	//the bp class of the interactable to spawn,init on begin build
	UPROPERTY()
	TSubclassOf<class AInteractableObject>MInteractableToSpawnClass;
	//to be parented by the interactable obj just before spawn
	UPROPERTY()
	TObjectPtr<class UStaticMeshComponent> MSpawnOverlay;

	//to change color of placeable material color on valid spawn position or not
	UPROPERTY(EditAnywhere)
    TObjectPtr<class UMaterialParameterCollection>MMaterialParameterCollection;
public:
	//returns and cast pangaea Pawn controller to a player controller
	APlayerController* GetPangaeaPlayerController() const;

	FORCEINLINE void SetHoverActor(AActor* T_HoverActor) { MHoverActor = T_HoverActor; }
};
