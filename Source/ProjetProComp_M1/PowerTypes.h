// Fill out your copyright notice in the Description page of Project Settings.
#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "PowerTypes.generated.h"

UENUM(BlueprintType)
enum class EPowerElement : uint8
{
	Fire       UMETA(DisplayName="Fire"),
	Ice        UMETA(DisplayName="Ice"),
	Lightning  UMETA(DisplayName="Lightning")
};


USTRUCT(BlueprintType)
struct FPowerNumbers
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Power")
	float Cooldown = 5.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Power")
	float ResourceCost = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Power")
	float ScalarA = 1.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Power")
	float ScalarB = 0.f;
};
