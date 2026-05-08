// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Logging/LogMacros.h"
#include "MyHealthComponent.h"
#include "BallProj.h"
#include "CableComponent.h"		
#include "Kismet/GameplayStatics.h"
#include "MGP_2526Character.generated.h"

class UCableComponent;
class USpringArmComponent;
class UCameraComponent;
class UInputAction;
class UMyHealthComponent;
struct FInputActionValue;


DECLARE_LOG_CATEGORY_EXTERN(LogTemplateCharacter, Log, All);

/**
 *  A simple player-controllable third person character
 *  Implements a controllable orbiting camera
 */
UCLASS(abstract)
class AMGP_2526Character : public ACharacter
{
	GENERATED_BODY()

	//UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	//UMyHealthComponent* HealthComponent;
	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components", meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components", meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FollowCamera;

	//I think I need sometning for audio

	UPROPERTY(EditAnywhere, Category = "Collision")
	TEnumAsByte<ECollisionChannel> TraceChannelProperty = ECC_Pawn;
	
	
protected:

	/** Jump Input Action */
	UPROPERTY(EditAnywhere, Category="Input")
	UInputAction* JumpAction;

	/** Move Input Action */
	UPROPERTY(EditAnywhere, Category="Input")
	UInputAction* MoveAction;

	UPROPERTY(EditAnywhere, Category = "Input")
	float SideDirection;
	/** Look Input Action */
	UPROPERTY(EditAnywhere, Category="Input")
	UInputAction* LookAction;

	/** Mouse Look Input Action */
	UPROPERTY(EditAnywhere, Category="Input")
	UInputAction* MouseLookAction;

	/** Dash Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* DashAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool bDashing = false;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	/** How far the dash should go */
	float DashDistance =  5000.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float DashForce = 15.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FVector DashTarget = FVector::ZeroVector;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float DashCooldown = 0.f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float DashTimer = 5.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Audio")
	USoundBase* DashImpactSound;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Audio")
	USoundBase* DashStartSound;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* GrappleAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UCableComponent* GrappleCable;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Audio")
	USoundBase* GrappleCastSound;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Audio")
	USoundBase* GrappleImpactSound;

	/** How far the dash should go */
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float GrappleRange = 3000.f;

	float InitialGrappleLength;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool bGrappling = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float GrappleCooldown = 0.f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float GrappleTimer = 4.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FVector CurrentGrapplePoint;

	/** Shoot Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* ShootAction;

	UPROPERTY(EditAnywhere)
	TSubclassOf<ABallProj> BP_ProjectileClass;

public:

	/** Constructor */
	AMGP_2526Character();	

	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

protected:

	/** Initialize input action bindings */
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

protected:

	/** Called for movement input */
	void Move(const FInputActionValue& Value);
	virtual void BeginPlay() override;	
	virtual void Tick(float DeltaSeconds) override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	/** Called for looking input */
	void Look(const FInputActionValue& Value);
	/** Connah Addition 2 methods to ensure we handle sprint enable and disable **/
	void StartDash(const FInputActionValue& Value);
	void StartGrapple(const FInputActionValue& Value);
	void Grapple(const FInputActionValue& Value);
	void EndGrapple(const FInputActionValue& Value);
	void ShootBall(const FInputActionValue& Value);
	void ShootBallEnd(const FInputActionValue& Value);


public:

	/** Handles move inputs from either controls or UI interfaces */
	UFUNCTION(BlueprintCallable, Category="Input")
	virtual void DoMove(float Right, float Forward);

	/** Handles look inputs from either controls or UI interfaces */
	UFUNCTION(BlueprintCallable, Category="Input")
	virtual void DoLook(float Yaw, float Pitch);

	UFUNCTION(BlueprintCallable, Category = "Input")
	virtual void DoStartDash(float dashStrength);
	UFUNCTION()
	virtual void DoEndDash();

	/** Handles jump pressed inputs from either controls or UI interfaces */
	UFUNCTION(BlueprintCallable, Category="Input")
	virtual void DoJumpStart();

	/** Handles jump pressed inputs from either controls or UI interfaces */
	UFUNCTION(BlueprintCallable, Category="Input")
	virtual void DoJumpEnd();
	// Initial check to see if the player can grapple
	UFUNCTION(BlueprintCallable, Category = "Input"	)
	virtual FVector TryRayCast(float range);


	/** Handles jump pressed inputs from either controls or UI interfaces */
	UFUNCTION(BlueprintCallable, Category = "Input")
	virtual void DoGrappleEnd();


	/** Handles jump pressed inputs from either controls or UI interfaces */
	UFUNCTION(BlueprintCallable, Category = "Input")
	virtual void DoShootBallStart();

	/** Handles jump pressed inputs from either controls or UI interfaces */
	UFUNCTION(BlueprintCallable, Category = "Input")
	virtual void DoShootBallEnd();

	UFUNCTION(BlueprintCallable, Category = "Input")
	virtual void ApplyGrappleForce(FVector AnchorPosition);

public:

	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }

	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }
};

