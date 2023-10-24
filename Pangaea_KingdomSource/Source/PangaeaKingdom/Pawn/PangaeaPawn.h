// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "InputActionValue.h"
#include "UObject/ObjectPtr.h"
#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "PangaeaPawn.generated.h"

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
	void UpdateZoom();

	//BindInputActions
	UFUNCTION()
	void Move(const FInputActionValue&InputActionValue);
	void Spin(const FInputActionValue&InputActionValue);
	void Zoom(const FInputActionValue&InputActionValue);

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

	UPROPERTY(EditAnywhere)
	TObjectPtr<class UInputMappingContext>MInputMappingContext;

	UPROPERTY(EditAnywhere)
	TObjectPtr<class UInputAction>MInputActionMove;

	UPROPERTY(EditAnywhere)
	TObjectPtr<class UInputAction>MInputActionSpin;

	UPROPERTY(EditAnywhere)
	TObjectPtr<class UInputAction>MInputActionZoom;

	UPROPERTY(EditAnywhere)
	TObjectPtr<class UFloatingPawnMovement>MFloatMovementComp;
    //to know if the player is zooming in or out
	float MZoomDirection{};
   //to return figures smoothly for interpolation at diff points in time
	UPROPERTY(EditAnywhere)
	TObjectPtr<class UCurveFloat>MZoomCurveFloat;


public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
};
