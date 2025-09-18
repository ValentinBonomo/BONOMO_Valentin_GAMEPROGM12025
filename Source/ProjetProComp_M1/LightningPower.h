#pragma once

#include "CoreMinimal.h"
#include "PowerBase.h"
#include "LightningPower.generated.h"

class ACharacter;
class UCharacterMovementComponent;

UCLASS()
class ULightningPower : public UPowerBase
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditDefaultsOnly, Category="Lightning|Dash")
	float DashSpeed = 2800.f;

	UPROPERTY(EditDefaultsOnly, Category="Lightning|Dash")
	float DashDuration = 0.20f;

	UPROPERTY(EditDefaultsOnly, Category="Lightning|StartAOE")
	float StartAoeRadius = 220.f;

	UPROPERTY(EditDefaultsOnly, Category="Lightning|StartAOE")
	float StartAoeDamageMultiplier = 1.0f;

	FTimerHandle Timer_EndDash;

	float SavedBrakingDecel = 0.f;
	float SavedGroundFriction = 0.f;
	float SavedBrakingFrictionFactor = 0.f;

	TWeakObjectPtr<ACharacter> OwnerChar;
	TWeakObjectPtr<UCharacterMovementComponent> OwnerMove;

protected:
	virtual bool ActivateNative() override;
	virtual void OnEquipped_Implementation() override;
	virtual void OnUnequipped_Implementation() override;

	void EndDash();

	void SpawnStartAoe(const FVector& Start, float BaseDamage);
};
