#pragma once

#include "CoreMinimal.h"
#include "PowerBase.h"
#include "FirePower.generated.h"

class ACharacter;
class AFireballProjectile;

UCLASS()
class UFirePower : public UPowerBase
{
	GENERATED_BODY()

public:
	UFirePower() = default;
	
	void ApplyBurnTo(AActor* Other);

protected:
	UPROPERTY(EditDefaultsOnly, Category="Fire|Projectile")
	TSubclassOf<AFireballProjectile> ProjectileClass;
	
	virtual bool ActivateNative() override;
	virtual void OnUnequipped_Implementation() override;

private:
	struct FBurnInstance
	{
		float TimeRemaining = 0.f;
		FTimerHandle TimerHandle;
	};
	
	TMap<TWeakObjectPtr<AActor>, FBurnInstance> ActiveBurns;
	
	void StartOrRefreshBurn(AActor* Victim);
	void StopBurn(AActor* Victim);
	void BurnTick(TWeakObjectPtr<AActor> VictimWeak);
	
	bool IsValidBurnTarget(AActor* Victim) const;
	const struct FPassiveBurnConfig& BurnCfg() const;
};
