// Fill out your copyright notice in the Description page of Project Settings.


#include "BallProj.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/SphereComponent.h"

// Sets default values
ABallProj::ABallProj()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	//PrimaryActorTick.bCanEverTick = true;

	//Collision
	CollisionComp = CreateDefaultSubobject<USphereComponent>(TEXT("SPHERE"));
	CollisionComp->InitSphereRadius(50.0f);
	CollisionComp->BodyInstance.SetCollisionProfileName("Projectile");
	CollisionComp->OnComponentHit.AddDynamic(this, &ABallProj::OnHit);

	//Player's can't walk on it
	CollisionComp->SetWalkableSlopeOverride(FWalkableSlopeOverride(WalkableSlope_Unwalkable, 0.f));
	CollisionComp->CanCharacterStepUpOn = ECB_No;

	RootComponent = CollisionComp;
	//Mesh component
	ProjectileMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ProjectileMesh"));
	ProjectileMesh->SetupAttachment(RootComponent);


	//Projectile Movement
	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("Projectile Movement"));
	ProjectileMovement->UpdatedComponent = CollisionComp;
	ProjectileMovement->InitialSpeed = 5000.0f;
	ProjectileMovement->MaxSpeed = 6000.0f;
	ProjectileMovement->bRotationFollowsVelocity = true;
	ProjectileMovement->bShouldBounce = true;

	InitialLifeSpan = 3.0f;

}

// Called when the game starts or when spawned
void ABallProj::BeginPlay()
{
	Super::BeginPlay();
	SpawnBall();
}

// Called every frame
void ABallProj::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ABallProj::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, FVector Impulse, const FHitResult& hit) {
	if ((OtherActor != nullptr) && (OtherActor != this) && (OtherComponent != nullptr)) //Null Checks
	{
		if (OtherComponent->IsSimulatingPhysics()) //Condition for whether the ball should trigger OnHit
		{
			OtherComponent->AddImpulseAtLocation(GetVelocity() * 100.f, GetActorLocation());

			Destroy();
		}
	}
	
}

void ABallProj::SpawnBall() {
	ProjectileMovement->InitialSpeed = 3000.0f;
	
}	

