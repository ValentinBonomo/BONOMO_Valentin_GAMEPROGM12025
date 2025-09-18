#include "ExplosiveBarrel.h"
#include "Components/StaticMeshComponent.h"
#include "PhysicsEngine/RadialForceComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystem.h"
#include "Sound/SoundBase.h"

#include "DamageType_Fire.h"
#include "Engine/DamageEvents.h"   // <<--- AJOUT NECESSAIRE


AExplosiveBarrel::AExplosiveBarrel()
{
    PrimaryActorTick.bCanEverTick = false;

    Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
    SetRootComponent(Mesh);
    Mesh->SetSimulatePhysics(true);
    Mesh->SetCollisionProfileName(TEXT("PhysicsActor"));

    RadialForce = CreateDefaultSubobject<URadialForceComponent>(TEXT("RadialForce"));
    RadialForce->SetupAttachment(Mesh);
    RadialForce->Radius = ExplosionRadius;
    RadialForce->ImpulseStrength = ImpulseStrength;
    RadialForce->bImpulseVelChange = true;
    RadialForce->bAutoActivate = false;
    RadialForce->bIgnoreOwningActor = true;
}

void AExplosiveBarrel::BeginPlay()
{
    Super::BeginPlay();
}

FString AExplosiveBarrel::TagsToString(const AActor* Actor)
{
    if (!Actor) return TEXT("None");
    FString Out;
    for (const FName& T : Actor->Tags)
    {
        Out += T.ToString();
        Out += TEXT(" ");
    }
    return Out.IsEmpty() ? TEXT("None") : Out;
}

float AExplosiveBarrel::TakeDamage(float DamageAmount,
                                   const FDamageEvent& DamageEvent,
                                   AController* EventInstigator,
                                   AActor* DamageCauser)
{
    if (bExplodedOrDestroyed) return 0.f;

    Health -= FMath::Max(0.f, DamageAmount);

    const UClass* DmgClass = DamageEvent.DamageTypeClass;
    const bool bIsFire = DmgClass && DmgClass->IsChildOf(UDamageType_Fire::StaticClass());

    UE_LOG(LogTemp, Log, TEXT("[Barrel] TakeDamage: Causer=%s  DamageType=%s  Fire? %s  NewHealth=%.1f"),
        *GetNameSafe(DamageCauser),
        DmgClass ? *DmgClass->GetName() : TEXT("None"),
        bIsFire ? TEXT("YES") : TEXT("NO"),
        Health);

    if (!bIsFire)
    {
        SilentDestroy();
        return DamageAmount;
    }
    
    if (Health <= 0.f)
    {
        Explode();
    }

    return DamageAmount;
}

void AExplosiveBarrel::Explode()
{
    if (bExplodedOrDestroyed) return;
    bExplodedOrDestroyed = true;

    const FVector Loc = GetActorLocation();

    UE_LOG(LogTemp, Warning, TEXT("[Barrel] EXPLODE @ %s  Dmg=%.1f  Radius=%.1f"),
        *Loc.ToString(), ExplosionDamage, ExplosionRadius);
    
    if (ExplosionFX)
    {
        UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ExplosionFX, Loc, FRotator::ZeroRotator, true);
    }
    if (ExplosionSFX)
    {
        UGameplayStatics::PlaySoundAtLocation(GetWorld(), ExplosionSFX, Loc);
    }
    
    {
        TSubclassOf<UDamageType> DmgTypeClass = DamageTypeClass
            ? DamageTypeClass
            : TSubclassOf<UDamageType>(UDamageType::StaticClass());

        TArray<AActor*> Ignore;
        Ignore.Add(this);

        UGameplayStatics::ApplyRadialDamage(
            this,
            ExplosionDamage,
            Loc,
            ExplosionRadius,
            DmgTypeClass,
            Ignore,
            this,
             nullptr,
             true,
            ECC_Visibility
        );
    }

    // Impulsion physique
    if (RadialForce)
    {
        RadialForce->Radius = ExplosionRadius;
        RadialForce->ImpulseStrength = ImpulseStrength;
        RadialForce->FireImpulse();
    }

    Destroy();
}

void AExplosiveBarrel::SilentDestroy()
{
    if (bExplodedOrDestroyed) return;
    bExplodedOrDestroyed = true;

    UE_LOG(LogTemp, Warning, TEXT("[Barrel] Shatter (no explosion)"));
    Destroy();
}
