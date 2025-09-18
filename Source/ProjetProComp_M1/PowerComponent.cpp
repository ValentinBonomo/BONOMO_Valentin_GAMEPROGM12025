#include "PowerComponent.h"
#include "PowerBase.h"
#include "PowerConfig.h"
#include "Engine/World.h"
#include "TimerManager.h"
#include "GameFramework/Actor.h"

UPowerComponent::UPowerComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
}

void UPowerComponent::BeginPlay()
{
    Super::BeginPlay();
    ResetGlobalCooldown();
}

/* Inventory */

UPowerBase* UPowerComponent::AddPower(TSubclassOf<UPowerBase> PowerClass, UPowerConfig* Config)
{
    if (!PowerClass || !Config) return nullptr;

    UPowerBase* NewObj = NewObject<UPowerBase>(this, PowerClass);
    if (!NewObj) return nullptr;

    NewObj->Init(this, Config);
    OwnedPowers.Add(NewObj);

    UE_LOG(LogTemp, Log, TEXT("[Powers] AddPower → %s"),
        *UEnum::GetValueAsString(Config->Element));
    return NewObj;
}

FString UPowerComponent::ElemToString(const UPowerBase* PB) const
{
    return PB ? UEnum::GetValueAsString(PB->GetElement()) : TEXT("None");
}

/* Equip */

void UPowerComponent::EquipAsMain(UPowerBase* Pwr)
{
    if (!Pwr) return;

    if (Main)
    {
        UE_LOG(LogTemp, Log, TEXT("[Powers] OnUnequipped → %s"), *ElemToString(Main));
        Main->OnUnequipped();
    }

    Main = Pwr;

    if (Main)
    {
        UE_LOG(LogTemp, Log, TEXT("[Powers] OnEquipped → %s"), *ElemToString(Main));
        Main->OnEquipped();
    }

    ApplyIceState(Main && Main->GetElement() == EPowerElement::Ice);
    OnMainChanged.Broadcast(Main);
}

void UPowerComponent::UnequipMain()
{
    if (!Main) return;

    UE_LOG(LogTemp, Log, TEXT("[Powers] UnequipMain → %s"), *ElemToString(Main));
    Main->OnUnequipped();
    Main = nullptr;

    ApplyIceState(false);
    OnMainChanged.Broadcast(nullptr);
}

void UPowerComponent::UnequipAll()
{
    UnequipMain();
    Slot1 = nullptr;
    Slot2 = nullptr;
}

/* Activation */

void UPowerComponent::ActivateMain()
{
    if (!Main)
    {
        UE_LOG(LogTemp, Warning, TEXT("[Powers] ActivateMain: aucun Main équipé"));
        return;
    }

    const bool bOK = Main->TryActivate();
    UE_LOG(LogTemp, Log, TEXT("[Powers] ActivateMain: %s → %s"),
        *UEnum::GetValueAsString(Main->GetElement()),
        bOK ? TEXT("OK") : TEXT("REFUS"));
}

/* ChoosePower */

void UPowerComponent::ChoosePower(EPowerElement Element)
{
    UPowerBase* Pwr = FindByElement(Element);
    if (!Pwr)
    {
        UE_LOG(LogTemp, Warning, TEXT("[Powers] ChoosePower(%s) → non possédé"),
            *UEnum::GetValueAsString(Element));
        return;
    }

    if (bSimpleEquipMode)
    {
        EquipAsMain(Pwr);
        return;
    }

    if (!Slot1) Slot1 = Pwr;
    else if (!Slot2) Slot2 = Pwr;
    EquipAsMain(Pwr);
}

/* Queries */

UPowerBase* UPowerComponent::FindByElement(EPowerElement Elt) const
{
    for (UPowerBase* P : OwnedPowers)
    {
        if (P && P->GetElement() == Elt)
            return P;
    }
    return nullptr;
}

const UPowerConfig* UPowerComponent::GetMainConfig() const
{
    return Main ? Main->GetConfig() : nullptr;
}

float UPowerComponent::GetMainCooldownRatio() const
{
    return Main ? Main->GetCooldownRatio() : 0.f;
}

float UPowerComponent::GetFireCooldownRatio() const
{
    if (const UPowerBase* P = FindByElement(EPowerElement::Fire))
        return P->GetCooldownRatio();
    return 0.f;
}

float UPowerComponent::GetIceCooldownRatio() const
{
    if (const UPowerBase* P = FindByElement(EPowerElement::Ice))
        return P->GetCooldownRatio();
    return 0.f;
}

float UPowerComponent::GetLightningCooldownRatio() const
{
    if (const UPowerBase* P = FindByElement(EPowerElement::Lightning))
        return P->GetCooldownRatio();
    return 0.f;
}

bool UPowerComponent::IsFireEquipped() const
{
    return Main && Main->GetElement() == EPowerElement::Fire;
}

bool UPowerComponent::IsIceEquipped() const
{
    return Main && Main->GetElement() == EPowerElement::Ice;
}

bool UPowerComponent::IsLightningEquipped() const
{
    return Main && Main->GetElement() == EPowerElement::Lightning;
}

float UPowerComponent::GetGlobalCooldownMultiplier() const
{
    return GlobalCooldownMult;
}

/* Passive notifications */

void UPowerComponent::NotifyPlayerHitActor(AActor* Target)
{
    UE_LOG(LogTemp, Verbose, TEXT("[Powers] NotifyPlayerHitActor → %s, Main=%s"),
        *GetNameSafe(Target),
        Main ? *UEnum::GetValueAsString(Main->GetElement()) : TEXT("None"));
}

/* Ice Passive CDR */

void UPowerComponent::ResetGlobalCooldown()
{
    GlobalCooldownMult = 1.0f;
}

void UPowerComponent::StartIcePassiveTimer()
{
    if (UWorld* W = GetWorld())
    {
        UE_LOG(LogTemp, Warning, TEXT("[IcePassive] Timer lancé : %.2fs"), IceTickInterval);
        W->GetTimerManager().SetTimer(IcePassiveTimer, this, &UPowerComponent::TickIcePassive, IceTickInterval, true);
    }
}

void UPowerComponent::StopIcePassiveTimer()
{
    if (UWorld* W = GetWorld())
    {
        W->GetTimerManager().ClearTimer(IcePassiveTimer);
    }
}

void UPowerComponent::ApplyIceState(bool bEnable)
{
    if (bEnable)
    {
        UE_LOG(LogTemp, Warning, TEXT("[IcePassive] Equipé → activation"));
        StartIcePassiveTimer();
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("[IcePassive] Déséquipé → reset CDR"));
        StopIcePassiveTimer();
        ResetGlobalCooldown();
    }
}

void UPowerComponent::TickIcePassive()
{
    const float Prev = GlobalCooldownMult;
    const float NewMult = FMath::Max(GlobalCooldownMult * IceStepMultiplier, IceMinGlobalMultiplier);
    GlobalCooldownMult = NewMult;

    UE_LOG(LogTemp, Warning,
        TEXT("[IcePassive] Tick: %.3f -> %.3f (step=%.2f, min=%.2f)"),
        Prev, NewMult, IceStepMultiplier, IceMinGlobalMultiplier);

    for (UPowerBase* P : OwnedPowers)
    {
        if (P && P->IsOnCooldown())
        {
            UE_LOG(LogTemp, Warning, TEXT("[IcePassive] Réduction live sur %s"),
                *UEnum::GetValueAsString(P->GetElement()));
            P->ApplyLiveCooldownScale(IceStepMultiplier);
        }
    }
}
