// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GenericAbility.h"
#include "DashAbility.generated.h"

/**
 * 
 */
UCLASS()
class MGP_2526_API UDashAbility : public UGenericAbility
{
	GENERATED_BODY()
	
public:

	UDashAbility();
	float DashForce = 15.f;
};
