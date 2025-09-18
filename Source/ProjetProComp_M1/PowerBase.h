#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "PowerTypes.h"
#include "PowerBase.generated.h"

class UPowerConfig;
class UPowerComponent;

UCLASS(Abstract, BlueprintType, Blueprintable, EditInlineNew, DefaultToInstanced)
class UPowerBase : public UObject
{
	GENERATED_BODY()

public:
	void Init(UPowerComponent* InOwnerComp, UPowerConfig* InConfig);

	UFUNCTION(BlueprintCallable, Category="Power")
	bool TryActivate();

	UFUNCTION(BlueprintPure, Category="Power")
	UPowerConfig* GetConfig() const { return Config; }

	UFUNCTION(BlueprintPure, Category="Power")
	UPowerComponent* GetOwnerComp() const { return OwnerComp; }

	UFUNCTION(BlueprintPure, Category="Power")
	EPowerElement GetElement() const { return Element; }

	UFUNCTION(BlueprintPure, Category="Power|Cooldown")
	bool IsOnCooldown() const;

	UFUNCTION(BlueprintPure, Category="Power|Cooldown")
	float GetCooldownRatio() const;

	UFUNCTION(BlueprintNativeEvent, Category="Power")
	void OnEquipped();
	virtual void OnEquipped_Implementation();

	UFUNCTION(BlueprintNativeEvent, Category="Power")
	void OnUnequipped();
	virtual void OnUnequipped_Implementation();

	void ApplyLiveCooldownScale(float Scale);

protected:
	virtual bool ActivateNative() { return false; }

	void StartCooldown(float Seconds);
	void StartCooldownFromConfig();
	void OnCooldownEnd();

protected:
	UPROPERTY(Transient)
	UPowerComponent* OwnerComp = nullptr;

	UPROPERTY(Transient)
	UPowerConfig* Config = nullptr;

	UPROPERTY(VisibleAnywhere, Category="Power")
	EPowerElement Element;

private:
	FTimerHandle CooldownTimer;
	float LastCooldownDuration = 0.f;
};
