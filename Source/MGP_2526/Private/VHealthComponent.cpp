// Fill out your copyright notice in the Description page of Project Settings.


#include "VHealthComponent.h"

// Sets default values
AVHealthComponent::AVHealthComponent()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

}

// Called when the game starts or when spawned
void AVHealthComponent::BeginPlay()
{
	Super::BeginPlay();
	
}

int32 AVHealthComponent::GetHealth() const {
	return CurrentHealth;
}

int32 AVHealthComponent::GetMaxHealth() const {
	return MaxHealth;
}

