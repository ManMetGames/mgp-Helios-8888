// Copyright Epic Games, Inc. All Rights Reserved.

#include "MGP_2526Character.h"
#include "Engine/LocalPlayer.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/Controller.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"

#include "MyHealthComponent.h"
#include "BallProj.h"

#include "MGP_2526.h"

AMGP_2526Character::AMGP_2526Character()
{
	// Create health component
	HealthComponent = CreateDefaultSubobject<UMyHealthComponent>(TEXT("HealthComponent"));


	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);
		
	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f);

	// Note: For faster iteration times these variables, and many more, can be tweaked in the Character Blueprint
	// instead of recompiling to adjust them
	GetCharacterMovement()->JumpZVelocity = 1500.0f;
	GetCharacterMovement()->AirControl = 0.85f; // 0 is no control. 1 is full control at max speed
	GetCharacterMovement()->MaxWalkSpeed = 1500.0f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;
	GetCharacterMovement()->BrakingDecelerationFalling = 1500.0f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.0f;
	CameraBoom->bUsePawnControlRotation = true;


	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;

	
	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named ThirdPersonCharacter (to avoid direct content references in C++)
}

void AMGP_2526Character::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent)) {
		
		// Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

		// Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AMGP_2526Character::Move);
		EnhancedInputComponent->BindAction(MouseLookAction, ETriggerEvent::Triggered, this, &AMGP_2526Character::Look);

		// Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AMGP_2526Character::Look);

		// Dashing
		EnhancedInputComponent->BindAction(DashAction, ETriggerEvent::Started, this, &AMGP_2526Character::StartDash);

		// Shooting
		EnhancedInputComponent->BindAction(ShootAction, ETriggerEvent::Started, this, &AMGP_2526Character::ShootBall);

		EnhancedInputComponent->BindAction(ShootAction, ETriggerEvent::Completed, this, &AMGP_2526Character::ShootBallEnd);

	}
	else
	{
		UE_LOG(LogMGP_2526, Error, TEXT("'%s' Failed to find an Enhanced Input component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
	}
}
void AMGP_2526Character::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (HealthComponent)
	{
		HealthComponent->OnTakeDamage.RemoveAll(this);
		HealthComponent->OnHealthDepleted.RemoveAll(this);
	}
}
void AMGP_2526Character::Move(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D MovementVector = Value.Get<FVector2D>();
	// route the input
	DoMove(MovementVector.X, MovementVector.Y);
}

void AMGP_2526Character::Look(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	// route the input
	DoLook(LookAxisVector.X, LookAxisVector.Y);
}

void AMGP_2526Character::StartDash(const FInputActionValue& Value)
{
	// route the input
	DoStartDash();
}

void AMGP_2526Character::ShootBall(const FInputActionValue& Value)
{
	// route the input
	DoShootBallStart();
}

void AMGP_2526Character::ShootBallEnd(const FInputActionValue& Value)
{
	// route the input
	DoShootBallEnd();
}



void AMGP_2526Character::DoMove(float Right, float Forward)
{
	if (GetController() != nullptr)
	{
		// find out which way is forward
		const FRotator Rotation = GetController()->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);

		// get right vector 
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		// add movement 
		AddMovementInput(ForwardDirection, Forward);
		AddMovementInput(RightDirection, Right);
	}
}

void AMGP_2526Character::DoLook(float Yaw, float Pitch)
{
	if (GetController() != nullptr)
	{
		// add yaw and pitch input to controller
		AddControllerYawInput(Yaw);
		AddControllerPitchInput(Pitch);
	}
}

void AMGP_2526Character::DoJumpStart()
{
	// signal the character to jump
	Jump();
}

void AMGP_2526Character::DoJumpEnd()
{
	// signal the character to stop jumping
	StopJumping();
}

void AMGP_2526Character::DoShootBallStart()
{
	UE_LOG(LogTemp, Warning, TEXT("Player Pressed Shoot"));
	FVector SpawnLocation = GetActorForwardVector() * 100 + GetActorLocation();
	ABallProj* Ball = GetWorld()->SpawnActor<ABallProj>(BP_ProjectileClass, SpawnLocation, GetFollowCamera()->GetComponentRotation());
	if (Ball) {
		Ball->SetActorLabel(TEXT("Ball"));	
		UE_LOG(LogTemp, Warning, TEXT("Ball Spawned"));
	}
	//HealthComponent->UpdateHealth(-10.f);
	//UE_LOG(LogTemp, Warning, TEXT("Player has %f health remaining"), HealthComponent->GetHealth());	
}

void AMGP_2526Character::DoShootBallEnd()
{
	//
}

void AMGP_2526Character::DoStartDash(){
	UE_LOG(LogTemp, Warning, TEXT("Dash Pressed"));
	//I'm thinking I turn this into a grapple retract. 
	//It might also be cool if when the grapple hits an enemy, it pulls the enemy towards the player instead of the player towards the enemy.
	UE_LOG(LogTemp, Warning, TEXT("%s"), *FString::SanitizeFloat(GetVelocity().Size()));
	//Get player's facing direction. later on will be the vector from player to grapple point
	FVector DashDirection = GetFollowCamera()->GetComponentRotation().Vector();
	//Apply a force
	LaunchCharacter(DashStrength * DashDirection, true, true);
}

void AMGP_2526Character::DoEndDash() {
	// I want the end of dash to cancel out the player's vertical velocity and set their horizontal velocity to the speed they had at the start of the dash.
	
	//Get the player's current position and velocity
	FVector CurrentPos = GetActorLocation();
	FVector CurrentVel = GetVelocity();
	float currentSpeed = CurrentVel.Size();
	CurrentVel.Normalize();
	//Set their walk speed to the stored velocity
	GetCharacterMovement()->MaxWalkSpeed = currentSpeed;
	//Set y velocity to 0?
	CurrentVel.Z = 0;
	CurrentVel *= currentSpeed;
	UE_LOG(LogTemp, Warning, TEXT("%s"), *CurrentVel.ToString());
	GetCharacterMovement()->Velocity = CurrentVel;
	
}

