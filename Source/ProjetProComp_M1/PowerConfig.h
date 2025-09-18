#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "PowerTypes.h"
#include "PowerConfig.generated.h"

class UTexture2D;
class USoundBase;
class UParticleSystem;
class UNiagaraSystem;


USTRUCT(BlueprintType)
struct FPassiveBurnConfig
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Burn")
	float DamagePerTick = 4.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Burn", meta=(ClampMin="0.05"))
	float TickInterval = 0.5f;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Burn", meta=(ClampMin="0.1"))
	float Duration = 3.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Burn")
	bool bRefreshOnHit = true;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Burn|FX")
	TSoftObjectPtr<UNiagaraSystem> IgniteVfx_Niagara;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Burn|FX")
	TSoftObjectPtr<UParticleSystem> IgniteVfx_Cascade;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Burn|FX")
	TSoftObjectPtr<USoundBase> IgniteSfx;
};


UCLASS(BlueprintType)
class UPowerConfig : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Power")
	EPowerElement Element = EPowerElement::Fire;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="UI")
	FText DisplayName;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="UI", meta=(MultiLine=true))
	FText Description;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Numbers")
	FPowerNumbers Numbers;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="FX")
	TSoftObjectPtr<UTexture2D> Icon;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="FX")
	TSoftObjectPtr<USoundBase> Sfx_Activate;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="FX", meta=(DisplayName="Vfx Activate (Niagara)"))
	TSoftObjectPtr<UNiagaraSystem> VfxNiagara_Activate;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="FX", meta=(DisplayName="Vfx Activate (Cascade - Legacy)"))
	TSoftObjectPtr<UParticleSystem> Vfx_Activate;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Passive|Fire")
	FPassiveBurnConfig Passive_Burn;

public:
	UFUNCTION(BlueprintCallable, Category="FX") UNiagaraSystem* LoadVfxNiagara() const;
	UFUNCTION(BlueprintCallable, Category="FX") UParticleSystem* LoadVfxCascade() const;
	UFUNCTION(BlueprintCallable, Category="FX") USoundBase*   LoadSfx() const;
	UFUNCTION(BlueprintCallable, Category="Passive|Fire") UNiagaraSystem* LoadBurnIgniteNiagara() const;
	UFUNCTION(BlueprintCallable, Category="Passive|Fire") UParticleSystem* LoadBurnIgniteCascade() const;
	UFUNCTION(BlueprintCallable, Category="Passive|Fire") USoundBase*     LoadBurnIgniteSfx() const;
};
