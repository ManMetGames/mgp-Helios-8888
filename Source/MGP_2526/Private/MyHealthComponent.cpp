// Fill out your copyright notice in the Description page of Project Settings.


#include "MyHealthComponent.h"

// Sets default values for this component's properties
UMyHealthComponent::UMyHealthComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;
	MaxHealth = 100.0f;
}


// Called when the game starts
void UMyHealthComponent::BeginPlay()
{
	Super::BeginPlay();

	CurrentHealth = MaxHealth;
	UE_LOG(LogTemp, Warning, TEXT("Health Component Initialized with %f health"), CurrentHealth);
	
}



float UMyHealthComponent::GetHealth() const {
	return CurrentHealth;
}

float UMyHealthComponent::GetMaxHealth() const {
	return MaxHealth;
}


void UMyHealthComponent::UpdateHealth(float dHealth) {
	CurrentHealth += dHealth;
	if (CurrentHealth > MaxHealth) {
		CurrentHealth = MaxHealth;
	}
	else if (CurrentHealth < 0) {
		CurrentHealth = 0;

		OnHealthDepleted.Broadcast();

	}

	OnTakeDamage.Broadcast(CurrentHealth, MaxHealth);
	//UE_LOG(LogTemp, Warning, TEXT("Health Updated: %f health remaining"), CurrentHealth);
}
