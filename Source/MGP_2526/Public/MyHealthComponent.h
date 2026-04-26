// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "MyHealthComponent.generated.h"



DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnHealthDepletedSignature);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnHealthChangedSignature, float, NewHealth, float, MaxHealth);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MGP_2526_API UMyHealthComponent : public UActorComponent
{
	GENERATED_BODY()
	
public:	
	//Current health
	UPROPERTY(BlueprintGetter = GetHealth, EditAnywhere, Category = "Default")
	float CurrentHealth;

	// Max health
	UPROPERTY(BlueprintGetter = GetMaxHealth, EditAnywhere, Category = "Default	")
	float MaxHealth = 100.0f;

	// Sets default values for this component's properties
	UMyHealthComponent();

	UPROPERTY(BlueprintAssignable)
	FOnHealthChangedSignature OnTakeDamage;

	UPROPERTY(BlueprintAssignable)
	FOnHealthDepletedSignature OnHealthDepleted;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	UFUNCTION(BlueprintCallable)
	void UpdateHealth(float dHealth);


	UFUNCTION(BlueprintCallable)
	float GetHealth() const;

	UFUNCTION(BlueprintCallable)
	float GetMaxHealth() const;
		
};
