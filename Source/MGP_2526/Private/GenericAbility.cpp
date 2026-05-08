// Fill out your copyright notice in the Description page of Project Settings.


#include "GenericAbility.h"

// Sets default values for this component's properties
UGenericAbility::UGenericAbility()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UGenericAbility::BeginPlay()
{
	Super::BeginPlay();
	SetupAbility();
	// ...
	
}

void UGenericAbility::SetupAbility() {

}
// Called every frame
void UGenericAbility::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UGenericAbility::TryUseAbility() {

}

void UGenericAbility :: AbilityStart() {

}

void UGenericAbility::AbilityEnd() {

}

FVector UGenericAbility::AbilityRayCast(float distance, FVector StartPos, FVector EndPos) {

	//
	return FVector::ZeroVector;
}
