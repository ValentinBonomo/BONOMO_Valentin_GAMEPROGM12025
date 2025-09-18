#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ExplosiveBarrel.generated.h"

class URadialForceComponent;
class UParticleSystem;
class USoundBase;


UCLASS()
class AExplosiveBarrel : public AActor
{
    GENERATED_BODY()

public:
    AExplosiveBarrel();

    virtual float TakeDamage(float DamageAmount,
                             struct FDamageEvent const& DamageEvent,
                             class AController* EventInstigator,
                             AActor* DamageCauser) override;

protected:
    virtual void BeginPlay() override;

    UFUNCTION(BlueprintCallable, Category="Barrel")
    void Explode();

    UFUNCTION(BlueprintCallable, Category="Barrel")
    void SilentDestroy();

private:
    UPROPERTY(VisibleAnywhere, Category="Barrel")
    UStaticMeshComponent* Mesh = nullptr;

    UPROPERTY(VisibleAnywhere, Category="Barrel")
    URadialForceComponent* RadialForce = nullptr;
    
    UPROPERTY(EditAnywhere, Category="Barrel|Gameplay", meta=(ClampMin="0"))
    float Health = 10.f;
    
    UPROPERTY(EditAnywhere, Category="Barrel|Explosion", meta=(ClampMin="0"))
    float ExplosionDamage = 80.f;

    UPROPERTY(EditAnywhere, Category="Barrel|Explosion", meta=(ClampMin="0"))
    float ExplosionRadius = 350.f;

    UPROPERTY(EditAnywhere, Category="Barrel|Explosion", meta=(ClampMin="0"))
    float ImpulseStrength = 1500.f;

    UPROPERTY(EditAnywhere, Category="Barrel|FX")
    UParticleSystem* ExplosionFX = nullptr;

    UPROPERTY(EditAnywhere, Category="Barrel|FX")
    USoundBase* ExplosionSFX = nullptr;

    UPROPERTY(EditAnywhere, Category="Barrel|Explosion")
    TSubclassOf<UDamageType> DamageTypeClass;

    bool bExplodedOrDestroyed = false;

    static FString TagsToString(const AActor* Actor);
};
