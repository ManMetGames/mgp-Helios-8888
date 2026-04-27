// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "VHealthComponent.generated.h"

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class MGP_2526_API AVHealthComponent : public AActor
{
	GENERATED_BODY()
	//Current health

	UPROPERTY(BlueprintGetter = GetHealth, EditAnywhere, Category = "Default")
	float CurrentHealth;

	// Max health
	UPROPERTY(BlueprintGetter = GetMaxHealth, EditAnywhere, Category = "Default")
	float MaxHealth = 100.0f;
public:
	// Sets default values for this actor's properties
	AVHealthComponent();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	UFUNCTION(BlueprintCallable)
	void UpdateHealth(float dHealth);
	

	UFUNCTION(BlueprintCallable)
	float GetHealth() const;

	UFUNCTION(BlueprintCallable)
	float GetMaxHealth() const;
};