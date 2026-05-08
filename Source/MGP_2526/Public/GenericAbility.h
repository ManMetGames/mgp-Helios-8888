// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Kismet/GameplayStatics.h"
#include "GenericAbility.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MGP_2526_API UGenericAbility : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UGenericAbility();

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool bAbilityActive;


	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float AbilityRange = 5000.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FVector AbilityTarget = FVector::ZeroVector;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float AbilityCooldown = 0.f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float AbilityTimer = 5.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Audio")
	USoundBase* AbilityPressedSound;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Audio")
	USoundBase* AbilityStartSound;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Audio")
	USoundBase* AbilityEndSound;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	virtual void SetupAbility();

	virtual FVector AbilityRayCast(float distance, FVector StartPos, FVector EndPos);
	virtual void TryUseAbility();
	virtual void AbilityStart();
	virtual void AbilityEnd();
};
