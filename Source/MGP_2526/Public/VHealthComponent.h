// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "VHealthComponent.generated.h"

UCLASS()
class MGP_2526_API AVHealthComponent : public AActor
{
	GENERATED_BODY()
	//Current health
	UPROPERTY(BlueprintGetter = GetHealth, Category = "Default")
	int32 CurrentHealth;

	// Max health
	UPROPERTY(BlueprintGetter = GetMaxHealth, EditAnywhere, Category = "Default")
	int32 MaxHealth;
public:
	// Sets default values for this actor's properties
	AVHealthComponent();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	UFUNCTION(BlueprintGetter)
	int32 GetHealth() const;

	UFUNCTION(BlueprintGetter)
	int32 GetMaxHealth() const;
};