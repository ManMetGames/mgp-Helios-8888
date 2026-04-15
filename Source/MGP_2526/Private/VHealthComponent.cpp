// Fill out your copyright notice in the Description page of Project Settings.


#include "VHealthComponent.h"

// Sets default values
AVHealthComponent::AVHealthComponent()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	MaxHealth = 100.0f;
}

// Called when the game starts or when spawned
void AVHealthComponent::BeginPlay()
{
	Super::BeginPlay();
	CurrentHealth = MaxHealth;
}

float AVHealthComponent::GetHealth() const {
	return CurrentHealth;
}

float AVHealthComponent::GetMaxHealth() const {
	return MaxHealth;
}


void AVHealthComponent::UpdateHealth(float dHealth) {
	CurrentHealth += dHealth;
	if (CurrentHealth > MaxHealth) {
		CurrentHealth = MaxHealth;
	}
	else if (CurrentHealth < 0) {
		CurrentHealth = 0;

		OnHealthDepleted.Broadcast();

	}	

	OnTakeDamage.Broadcast(CurrentHealth, MaxHealth);

}

