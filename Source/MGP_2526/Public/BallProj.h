 // Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BallProj.generated.h"

class USphereComponent;
class UProjectileMovementComponent;

UCLASS(config = Game)
class MGP_2526_API ABallProj : public AActor
{
	GENERATED_BODY()
	
	// ** Collision Component *//
	UPROPERTY(VisibleDefaultsOnly, Category = Projectile)
	USphereComponent* CollisionComp;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UProjectileMovementComponent* ProjectileMovement;
	
	UPROPERTY(VisibleDefaultsOnly, Category = Projectile)
	UStaticMeshComponent* BallMesh;
public:	
	// Sets default values for this actor's properties
	ABallProj();
	
	UFUNCTION() void OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, FVector Impulse, const FHitResult& hit);
	USphereComponent* GetCollisionComp() const { return CollisionComp; };
	UProjectileMovementComponent* GetProj() const { return ProjectileMovement; };
	virtual void SpawnBall();
	

protected:
	// Called when the game starts or when spawned
	float ProjectileSpeed;
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
