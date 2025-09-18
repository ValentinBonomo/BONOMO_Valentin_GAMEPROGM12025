#pragma once

#include "CoreMinimal.h"
#include "PowerBase.h"
#include "IcePower.generated.h"

class ACharacter;

UCLASS()
class UIcePower : public UPowerBase
{
	GENERATED_BODY()

public:
	UIcePower() = default;

protected:
	UPROPERTY(EditAnywhere, Category="Ice|Shockwave")
	float ShockwaveRadius = 450.f;

	UPROPERTY(EditAnywhere, Category="Ice|Shockwave")
	float DamageMultiplier = 1.0f;

	UPROPERTY(EditAnywhere, Category="Ice|Shockwave", meta=(ClampMin="0.0"))
	float RadialImpulseStrength = 0.f;
	
	UPROPERTY(EditAnywhere, Category="Ice|Shockwave|Slow", meta=(ClampMin="0.0", ClampMax="0.95"))
	float ActiveSlowPercent = 0.40f;

	UPROPERTY(EditAnywhere, Category="Ice|Shockwave|Slow", meta=(ClampMin="0.1"))
	float ActiveSlowDuration = 10.0f;
	
	UPROPERTY(EditAnywhere, Category="Ice|Visual")
	bool bMatchVfxToRadius = true;

	UPROPERTY(EditAnywhere, Category="Ice|Visual", meta=(EditCondition="bMatchVfxToRadius", ClampMin="1.0"))
	float VfxReferenceRadius = 300.f;

	UPROPERTY(EditAnywhere, Category="Ice|Visual", meta=(EditCondition="!bMatchVfxToRadius", ClampMin="0.01"))
	float VfxManualScale = 1.0f;

	UPROPERTY(EditAnywhere, Category="Ice|Visual")
	FName NiagaraRadiusParamName = TEXT("Radius");

	UPROPERTY(EditAnywhere, Category="Ice|Debug")
	bool bDebugDraw = false;

	UPROPERTY(EditAnywhere, Category="Ice|Debug", meta=(EditCondition="bDebugDraw"))
	FColor DebugColor = FColor(135, 206, 235);

	UPROPERTY(EditAnywhere, Category="Ice|Debug", meta=(EditCondition="bDebugDraw"))
	float DebugTime = 1.0f;

protected:
	virtual bool ActivateNative() override;
	virtual void OnEquipped_Implementation() override;
	virtual void OnUnequipped_Implementation() override;

	void DoShockwave(const FVector& Origin, float BaseDamage);

	void ApplyActiveSlowToActors(const TArray<AActor*>& Actors);
};
