#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "FireballProjectile.generated.h"

class USphereComponent;
class UProjectileMovementComponent;
class UParticleSystem;
class USoundBase;

UCLASS()
class AFireballProjectile : public AActor
{
    GENERATED_BODY()

public:
    AFireballProjectile();
    
    void SetShooter(AActor* InShooter) { Shooter = InShooter; }
    void SetDamage(float InDamage) { Damage = InDamage; }
    void SetRadius(float InRadius) { Radius = InRadius; }
    void SetImpactVFX(UParticleSystem* InVfx) { ImpactCascade = InVfx; }
    void SetImpactSFX(USoundBase* InSfx) { ImpactSnd = InSfx; }

protected:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    USphereComponent* Collision;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    UProjectileMovementComponent* Movement;
    
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    UParticleSystemComponent* TrailPSC;
    
    UPROPERTY()
    AActor* Shooter = nullptr;

    UPROPERTY(EditAnywhere, Category="Projectile")
    float Damage = 20.f;

    UPROPERTY(EditAnywhere, Category="Projectile")
    float Radius = 200.f;
    
    UPROPERTY()
    UParticleSystem* ImpactCascade = nullptr;

    UPROPERTY()
    USoundBase* ImpactSnd = nullptr;
    
    bool bHasExploded = false;

protected:
    virtual void BeginPlay() override;

    UFUNCTION()
    void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor,
               UPrimitiveComponent* OtherComp, FVector NormalImpulse,
               const FHitResult& Hit);

    void Explode(const FHitResult& Hit, AActor* FirstHitActor);
};
