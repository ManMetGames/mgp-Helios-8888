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
#include "CableComponent.h"
#include "BallProj.h"

#include "MGP_2526.h"

AMGP_2526Character::AMGP_2526Character()
{
	// Create health component
	//HealthComponent = CreateDefaultSubobject<UMyHealthComponent>(TEXT("HealthComponent"));


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
	GetCharacterMovement()->AirControlBoostVelocityThreshold = 2000.f;
	GetCharacterMovement()->AirControlBoostMultiplier = 2.5f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.0f;
	CameraBoom->bUsePawnControlRotation = true;


	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;

	GrappleCable = CreateDefaultSubobject<UCableComponent>(TEXT("GrappleCable"));
	GrappleCable->SetupAttachment(RootComponent);
	GrappleCable->SetVisibility(false);

	// Surely I don't have to modify the build file again	
	//DashAudioComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("Audio"));
	//DashAudioComponent->SetupAttachment(RootComponent);

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

		// Shooting
		EnhancedInputComponent->BindAction(GrappleAction, ETriggerEvent::Started, this, &AMGP_2526Character::StartGrapple);
		EnhancedInputComponent->BindAction(GrappleAction, ETriggerEvent::Ongoing, this, &AMGP_2526Character::Grapple);

		EnhancedInputComponent->BindAction(GrappleAction, ETriggerEvent::Completed, this, &AMGP_2526Character::EndGrapple);

	}
	else
	{
		UE_LOG(LogMGP_2526, Error, TEXT("'%s' Failed to find an Enhanced Input component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
	}
}
void AMGP_2526Character::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();
	//Add Input Mapping Context
	GetCapsuleComponent()->OnComponentHit.AddDynamic(this, &AMGP_2526Character::OnHit);
	GrappleTimer = 0.f;
	DashTimer = 0.f;
	GrappleCooldown = 4.f;
	DashCooldown = 5.f;
}

void AMGP_2526Character::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	if (bDashing) {
		if ((DashTarget - GetActorLocation()).Size() < 500.f) {
			DoEndDash();
		}
		else {
			FVector DashDirection = DashTarget - GetActorLocation();
			LaunchCharacter(DashForce * DashDirection, true, true);
		}
	}
	if (GrappleTimer > 0.f) {
		GrappleTimer -= DeltaSeconds;
		//UE_LOG(LogTemp, Warning, TEXT("Grapple Timer: %f"), GrappleTimer);
	}
	if (DashTimer > 0.f	) {
		DashTimer -= DeltaSeconds;
		//UE_LOG(LogTemp, Warning, TEXT("Dash Timer: %f"), DashTimer);
	}
}

void AMGP_2526Character::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
}

void AMGP_2526Character::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if ((OtherActor!= nullptr) && (OtherActor != this) && (OtherComp!=nullptr))
	{
		UE_LOG(LogTemp, Warning, TEXT("Player hit: %s"), *OtherActor->GetName());
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
	UE_LOG(LogTemp, Warning, TEXT("Dash Pressed"));
	if (DashTimer <= 0.f) {
		DoStartDash(DashDistance);
	}
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

void AMGP_2526Character::StartGrapple(const FInputActionValue& Value)
{
	// route the input
	UE_LOG(LogTemp, Warning, TEXT("Player Pressed Grapple"));
	FVector HitLocation = TryRayCast(GrappleRange);
	UE_LOG(LogTemp, Warning, TEXT("Grapple Timer: %f"), GrappleTimer);
	if (HitLocation != FVector::ZeroVector && GrappleTimer<=0.f) {
		bGrappling = true;
		CurrentGrapplePoint = HitLocation;
		InitialGrappleLength = (CurrentGrapplePoint - GetActorLocation()).Size();
		GrappleCable->CableLength = InitialGrappleLength;
		LaunchCharacter((CurrentGrapplePoint - GetActorLocation()).GetSafeNormal() * 1000.f, true, true);

		GrappleCable->SetVisibility(true);
		GrappleTimer = GrappleCooldown;	
	}
}

void AMGP_2526Character::Grapple(const FInputActionValue& Value)
{
	if (bGrappling) {

		ApplyGrappleForce(CurrentGrapplePoint);
		GrappleCable->EndLocation = GetActorTransform().InverseTransformPosition(CurrentGrapplePoint);
		float distance = (GetActorLocation() - CurrentGrapplePoint).Size();
		GrappleCable->CableLength = distance;
		if (distance< 500.f || distance>InitialGrappleLength){
			DoGrappleEnd();
		}
	}
}

void AMGP_2526Character::EndGrapple(const FInputActionValue& Value)
{
	// route the input
	DoGrappleEnd();
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
		SideDirection = Right;

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
}

void AMGP_2526Character::DoShootBallEnd()
{
	//
}

void AMGP_2526Character::DoStartDash(float dashDistance){
	//I've changed my mind. I'm gonna do a slam.
	// Ray cast towards the ground based on the player's camera direction. If it hits something we launch our player towards the ground in a straight line.
	FVector HitLocation = TryRayCast(DashDistance);
	UE_LOG(LogTemp, Warning, TEXT("Hit Location: %s"), *HitLocation.ToString());
	UE_LOG(LogTemp, Warning, TEXT("Player Location: %s"), *GetActorLocation().ToString());
	if (HitLocation != FVector::ZeroVector && (GetActorLocation().Z - HitLocation.Z>100.f)) {
		//Apply a force
		bDashing = true;
		DashTarget = HitLocation;
		DashTimer = DashCooldown;
	}
}

void AMGP_2526Character::DoEndDash() {
	// I want the end of dash to cancel out the player's vertical velocity and set their horizontal velocity to the speed they had at the start of the dash.
	bDashing = false;
	GetCharacterMovement()->StopActiveMovement();
	UE_LOG(LogTemp, Warning, TEXT("Player Ended Dash"));
	UE_LOG(LogTemp, Warning, TEXT("%d"), GetCharacterMovement()->MovementMode);
	//Play the audio cue
	//DashAudioComponent->Play();
	
}

FVector AMGP_2526Character::TryRayCast(float range) {

	FVector StartPos = GetFollowCamera()->GetComponentLocation();
	FVector EndPos = StartPos + GetFollowCamera()->GetForwardVector() * range;
	FHitResult Hit;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);

	GetWorld()->LineTraceSingleByChannel(Hit, StartPos, EndPos, TraceChannelProperty, Params);

	if (Hit.GetActor()) {

		UE_LOG(LogTemp, Warning, TEXT("Ray Cast Hit: %s"), *Hit.GetActor()->GetName());
		DrawDebugLine(GetWorld(), StartPos, EndPos, FColor::Yellow, false, 5.f);
		return Hit.ImpactPoint;
	}
	else {
		DrawDebugLine(GetWorld(), StartPos, EndPos, FColor::White, false, 5.f);
		return FVector::ZeroVector;
	}
}

void AMGP_2526Character::ApplyGrappleForce(FVector AnchorPosition)
{
	// Restrict the player's movement to a radius of the grapple range around the anchor point.
	FVector DirectionTowardsAnchor = AnchorPosition - GetActorLocation();
	float GrappleLength = DirectionTowardsAnchor.Size();
	DirectionTowardsAnchor.Normalize();
	// Apply a force towards the anchor.
	LaunchCharacter(DirectionTowardsAnchor * GrappleLength*0.1f, false, false);



}

void AMGP_2526Character::DoGrappleEnd()
{
	if (bGrappling) {
		bGrappling = false;
		UE_LOG(LogTemp, Warning, TEXT("Player Released Grapple"));
		GrappleCable->SetVisibility(false);
	}
}
