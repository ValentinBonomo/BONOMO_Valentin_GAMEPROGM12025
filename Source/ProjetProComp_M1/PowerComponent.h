#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PowerTypes.h"
#include "PowerComponent.generated.h"

class UPowerBase;
class UPowerConfig;
class AActor;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMainChanged, UPowerBase*, NewMain);

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class UPowerComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UPowerComponent();

    UFUNCTION(BlueprintCallable, Category="Powers")
    UPowerBase* AddPower(TSubclassOf<UPowerBase> PowerClass, UPowerConfig* Config);

    UFUNCTION(BlueprintCallable, Category="Powers")
    void ActivateMain();

    UFUNCTION(BlueprintCallable, Category="Powers")
    void ChoosePower(EPowerElement Element);

    UFUNCTION(BlueprintCallable, Category="Powers")
    void EquipAsMain(UPowerBase* Pwr);

    UFUNCTION(BlueprintCallable, Category="Powers")
    void UnequipMain();

    UFUNCTION(BlueprintCallable, Category="Powers")
    void UnequipAll();

    UFUNCTION(BlueprintPure, Category="Powers")
    UPowerBase* FindByElement(EPowerElement Elt) const;
    
    UFUNCTION(BlueprintCallable, Category="Powers")
    void NotifyPlayerHitActor(AActor* Target);
    
    UFUNCTION(BlueprintPure, Category="Powers")
    UPowerBase* GetMain() const { return Main; }

    UFUNCTION(BlueprintPure, Category="Powers")
    const UPowerConfig* GetMainConfig() const;

    UFUNCTION(BlueprintPure, Category="Powers|Cooldown")
    float GetMainCooldownRatio() const;

    UPROPERTY(BlueprintAssignable, Category="Powers")
    FOnMainChanged OnMainChanged;

    UFUNCTION(BlueprintPure, Category="Powers")
    UPowerBase* GetFirePower() const { return FindByElement(EPowerElement::Fire); }

    UFUNCTION(BlueprintPure, Category="Powers")
    UPowerBase* GetIcePower() const { return FindByElement(EPowerElement::Ice); }

    UFUNCTION(BlueprintPure, Category="Powers")
    UPowerBase* GetLightningPower() const { return FindByElement(EPowerElement::Lightning); }

    UFUNCTION(BlueprintPure, Category="Powers|Cooldown")
    float GetFireCooldownRatio() const;

    UFUNCTION(BlueprintPure, Category="Powers|Cooldown")
    float GetIceCooldownRatio() const;

    UFUNCTION(BlueprintPure, Category="Powers|Cooldown")
    float GetLightningCooldownRatio() const;

    UFUNCTION(BlueprintPure, Category="Powers")
    bool IsFireEquipped() const;

    UFUNCTION(BlueprintPure, Category="Powers")
    bool IsIceEquipped() const;

    UFUNCTION(BlueprintPure, Category="Powers")
    bool IsLightningEquipped() const;

    UFUNCTION(BlueprintPure, Category="Powers|Cooldown")
    float GetGlobalCooldownMultiplier() const;

protected:
    virtual void BeginPlay() override;

private:
    /* Ice passive CDR */
    void StartIcePassiveTimer();
    void StopIcePassiveTimer();
    void TickIcePassive();
    void ResetGlobalCooldown();
    void ApplyIceState(bool bEnable);

    FString ElemToString(const UPowerBase* PB) const;

public:
    UPROPERTY(VisibleInstanceOnly, Category="Powers")
    UPowerBase* Main = nullptr;

    UPROPERTY(EditAnywhere, Category="Powers")
    bool bSimpleEquipMode = true;

private:
    UPROPERTY() TArray<UPowerBase*> OwnedPowers;

    UPROPERTY(VisibleInstanceOnly, Category="Powers")
    UPowerBase* Slot1 = nullptr;

    UPROPERTY(VisibleInstanceOnly, Category="Powers")
    UPowerBase* Slot2 = nullptr;

    UPROPERTY(VisibleAnywhere, Category="Powers|Cooldown")
    float GlobalCooldownMult = 1.0f;

    UPROPERTY(EditAnywhere, Category="Powers|Cooldown")
    float IceTickInterval = 2.5f;

    UPROPERTY(EditAnywhere, Category="Powers|Cooldown")
    float IceStepMultiplier = 0.70f;   // -30%

    UPROPERTY(EditAnywhere, Category="Powers|Cooldown")
    float IceMinGlobalMultiplier = 2.5f;

    FTimerHandle IcePassiveTimer;
};
