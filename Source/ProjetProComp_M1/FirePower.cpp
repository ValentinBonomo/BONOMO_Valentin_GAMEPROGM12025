#include "FirePower.h"
#include "DamageType_Fire.h"
#include "PowerComponent.h"
#include "PowerConfig.h"
#include "FireballProjectile.h"
#include "GameFramework/Character.h"
#include "GameFramework/Controller.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraSystem.h"
#include "Particles/ParticleSystem.h"
#include "Sound/SoundBase.h"

bool UFirePower::ActivateNative()
{
    if (!OwnerComp || !Config) return false;

    ACharacter* OwnerChar = Cast<ACharacter>(OwnerComp->GetOwner());
    if (!OwnerChar) return false;
    
    TSubclassOf<AFireballProjectile> ClassToSpawn = ProjectileClass;
    if (!ClassToSpawn)
    {
        ClassToSpawn = AFireballProjectile::StaticClass();
    }
    
    const FVector  Forward  = OwnerChar->GetActorForwardVector();
    const FVector  SpawnLoc = OwnerChar->GetActorLocation() + Forward * 100.f + FVector(0, 0, 50.f);
    const FRotator SpawnRot = OwnerChar->GetControlRotation();

    FActorSpawnParameters Params;
    Params.Owner = OwnerChar;
    Params.Instigator = OwnerChar;
    Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

    AFireballProjectile* Proj = OwnerChar->GetWorld()->SpawnActor<AFireballProjectile>(ClassToSpawn, SpawnLoc, SpawnRot, Params);
    if (Proj)
    {
        const float Damage = Config->Numbers.ScalarA;
        const float Radius = Config->Numbers.ScalarB;

        Proj->SetShooter(OwnerChar);
        Proj->SetDamage(Damage);
        Proj->SetRadius(Radius);
        
        if (UParticleSystem* ImpactCascade = Config->LoadVfxCascade())
        {
            Proj->SetImpactVFX(ImpactCascade);
        }
        if (USoundBase* ImpactSnd = Config->LoadSfx())
        {
            Proj->SetImpactSFX(ImpactSnd);
        }
        return true;
    }

    return false;
}

void UFirePower::OnUnequipped_Implementation()
{
    for (auto& It : ActiveBurns)
    {
        if (It.Value.TimerHandle.IsValid())
        {
            if (UWorld* W = GetWorld())
            {
                W->GetTimerManager().ClearTimer(It.Value.TimerHandle);
            }
        }
    }
    ActiveBurns.Empty();
}

void UFirePower::ApplyBurnTo(AActor* Other)
{
    UE_LOG(LogTemp, Warning, TEXT("[Burn] ApplyBurnTo -> %s"), *GetNameSafe(Other));
    if (!Other) return;
    if (!IsValidBurnTarget(Other)) { UE_LOG(LogTemp, Warning, TEXT("[Burn] invalid target")); return; }
    StartOrRefreshBurn(Other);
}


bool UFirePower::IsValidBurnTarget(AActor* Victim) const
{
    if (!Victim) return false;
    const AActor* OwnerActor = OwnerComp ? OwnerComp->GetOwner() : nullptr;
    if (Victim == OwnerActor) return false;
    return true;
}

const FPassiveBurnConfig& UFirePower::BurnCfg() const
{
    static FPassiveBurnConfig DefaultCfg;
    return Config ? Config->Passive_Burn : DefaultCfg;
}

void UFirePower::StartOrRefreshBurn(AActor* Victim)
{
    if (!Victim) return;

    const FPassiveBurnConfig& Cfg = BurnCfg();
    UE_LOG(LogTemp, Warning, TEXT("[Burn] StartOrRefresh -> %s  (Dur=%.2fs, Tick=%.2fs, Dmg/Tick=%.1f)"),
        *GetNameSafe(Victim), Cfg.Duration, Cfg.TickInterval, Cfg.DamagePerTick);

    TWeakObjectPtr<AActor> Key = Victim;
    FBurnInstance* Instance = ActiveBurns.Find(Key);

    if (Instance)
    {
        UE_LOG(LogTemp, Warning, TEXT("[Burn] Refresh time for %s"), *GetNameSafe(Victim));
        if (Cfg.bRefreshOnHit)
        {
            Instance->TimeRemaining = Cfg.Duration;
        }
        return;
    }

    FBurnInstance NewInst;
    NewInst.TimeRemaining = Cfg.Duration;

    FTimerDelegate Del = FTimerDelegate::CreateUObject(this, &UFirePower::BurnTick, Key);
    if (UWorld* W = GetWorld())
    {
        W->GetTimerManager().SetTimer(NewInst.TimerHandle, Del, Cfg.TickInterval, true);
        UE_LOG(LogTemp, Warning, TEXT("[Burn] Timer set for %s (interval=%.2fs)"),
            *GetNameSafe(Victim), Cfg.TickInterval);
    }

    ActiveBurns.Add(Key, NewInst);
}


void UFirePower::StopBurn(AActor* Victim)
{
    if (!Victim) return;

    TWeakObjectPtr<AActor> Key = Victim;
    if (FBurnInstance* Instance = ActiveBurns.Find(Key))
    {
        if (Instance->TimerHandle.IsValid())
        {
            if (UWorld* W = GetWorld())
            {
                W->GetTimerManager().ClearTimer(Instance->TimerHandle);
            }
        }
        ActiveBurns.Remove(Key);
    }
}

void UFirePower::BurnTick(TWeakObjectPtr<AActor> VictimWeak)
{
    const FPassiveBurnConfig& Cfg = BurnCfg();

    if (!VictimWeak.IsValid())
    {
        ActiveBurns.Remove(VictimWeak);
        return;
    }

    AActor* Victim = VictimWeak.Get();
    FBurnInstance* Instance = ActiveBurns.Find(VictimWeak);
    if (!Instance) return;
    
    UE_LOG(LogTemp, Warning, TEXT("[Burn] %s +%.1f dmg"),
        *GetNameSafe(Victim), Cfg.DamagePerTick);

    UGameplayStatics::ApplyDamage(
        Victim,
        Cfg.DamagePerTick,
        nullptr,
        OwnerComp ? OwnerComp->GetOwner() : nullptr,
        UDamageType_Fire::StaticClass()
    );

    Instance->TimeRemaining -= Cfg.TickInterval;
    if (Instance->TimeRemaining <= 0.f)
    {
        StopBurn(Victim);
    }
}




