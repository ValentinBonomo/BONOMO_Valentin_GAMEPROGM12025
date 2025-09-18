#include "IcePower.h"
#include "PowerComponent.h"
#include "PowerConfig.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Components/PrimitiveComponent.h"
#include "Engine/World.h"
#include "Engine/EngineTypes.h"
#include "DrawDebugHelpers.h"
#include "NiagaraSystem.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Particles/ParticleSystem.h"
#include "Sound/SoundBase.h"
#include "DamageType_Ice.h"
#include "EnemyDummy.h"

bool UIcePower::ActivateNative()
{
	if (!Config || !OwnerComp) return false;

	ACharacter* OwnerChar = Cast<ACharacter>(OwnerComp->GetOwner());
	if (!OwnerChar) return false;

	const FVector Origin     = OwnerChar->GetActorLocation();
	const float   BaseDamage = Config->Numbers.ScalarA;

	DoShockwave(Origin, BaseDamage);
	return true;
}

void UIcePower::DoShockwave(const FVector& Origin, float BaseDamage)
{
	UNiagaraComponent* SpawnedNiagara = nullptr;

	if (UNiagaraSystem* NS = Config->LoadVfxNiagara())
	{
		float VisualScale = 1.f;
		if (bMatchVfxToRadius)
		{
			VisualScale = FMath::Max(ShockwaveRadius / FMath::Max(1.f, VfxReferenceRadius), 0.01f);
		}
		else
		{
			VisualScale = VfxManualScale;
		}

		SpawnedNiagara = UNiagaraFunctionLibrary::SpawnSystemAtLocation(
			GetWorld(), NS, Origin, FRotator::ZeroRotator, FVector(VisualScale), true, true, ENCPoolMethod::AutoRelease);

		if (SpawnedNiagara && NiagaraRadiusParamName != NAME_None)
		{
			SpawnedNiagara->SetVariableFloat(NiagaraRadiusParamName, ShockwaveRadius);
		}
	}
	else if (UParticleSystem* PS = Config->LoadVfxCascade())
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), PS, Origin, FRotator::ZeroRotator);
	}
	
	if (USoundBase* Snd = Config->LoadSfx())
	{
		UGameplayStatics::PlaySoundAtLocation(this, Snd, Origin);
	}
	
	const float ShockDamage = BaseDamage * DamageMultiplier;

	TArray<AActor*> Ignore;
	if (AActor* OwnerActor = OwnerComp ? OwnerComp->GetOwner() : nullptr)
	{
		Ignore.Add(OwnerActor);
	}

	UGameplayStatics::ApplyRadialDamage(
		this,
		ShockDamage,
		Origin,
		ShockwaveRadius,
		UDamageType_Ice::StaticClass(),
		Ignore,
		OwnerComp ? OwnerComp->GetOwner() : nullptr,
		nullptr
	);
	
	TArray<TEnumAsByte<EObjectTypeQuery>> PawnType;
	PawnType.Add(UEngineTypes::ConvertToObjectType(ECC_Pawn));

	TArray<AActor*> OverlappedActors;
	UKismetSystemLibrary::SphereOverlapActors(
		GetWorld(),
		Origin,
		ShockwaveRadius,
		PawnType,
		nullptr,
		Ignore,
		OverlappedActors
	);

	ApplyActiveSlowToActors(OverlappedActors);
	
	if (RadialImpulseStrength > 0.f)
	{
		TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
		ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_PhysicsBody));

		TArray<UPrimitiveComponent*> HitComponents;
		UKismetSystemLibrary::SphereOverlapComponents(
			GetWorld(),
			Origin,
			ShockwaveRadius,
			ObjectTypes,
			UPrimitiveComponent::StaticClass(),
			Ignore,
			HitComponents
		);

		for (UPrimitiveComponent* Prim : HitComponents)
		{
			if (Prim && Prim->IsSimulatingPhysics())
			{
				Prim->AddRadialImpulse(
					Origin,
					ShockwaveRadius,
					RadialImpulseStrength,
					ERadialImpulseFalloff::RIF_Linear,
					true
				);
			}
		}
	}
	
}

void UIcePower::ApplyActiveSlowToActors(const TArray<AActor*>& Actors)
{
	if (ActiveSlowPercent <= 0.f || ActiveSlowDuration <= 0.f) return;

	for (AActor* Target : Actors)
	{
		if (!Target) continue;
		if (OwnerComp && Target == OwnerComp->GetOwner()) continue;
		
		if (AEnemyDummy* Dummy = Cast<AEnemyDummy>(Target))
		{
			Dummy->ApplySlow(ActiveSlowPercent, ActiveSlowDuration);
		}
	}
}

void UIcePower::OnEquipped_Implementation() {}
void UIcePower::OnUnequipped_Implementation() {}
