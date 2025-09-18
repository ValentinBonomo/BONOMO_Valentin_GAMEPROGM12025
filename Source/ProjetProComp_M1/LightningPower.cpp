// LightningPower.cpp
#include "LightningPower.h"
#include "PowerComponent.h"
#include "PowerConfig.h"
#include "ElectricGenerator.h"

#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"      // <<< overlap helpers
#include "TimerManager.h"
#include "Engine/World.h"


// VFX/SFX
#include "DamageType_Lightning.h"
#include "Particles/ParticleSystem.h"
#include "Sound/SoundBase.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraSystem.h"

bool ULightningPower::ActivateNative()
{
	if (!Config || !OwnerComp) return false;

	OwnerChar = Cast<ACharacter>(OwnerComp->GetOwner());
	if (!OwnerChar.IsValid()) return false;

	OwnerMove = OwnerChar->GetCharacterMovement();
	if (!OwnerMove.IsValid()) return false;

	const float Damage = Config->Numbers.ScalarA;

	const FVector Start = OwnerChar->GetActorLocation();
	const FVector Dir   = OwnerChar->GetActorForwardVector().GetSafeNormal();

	// 1) Impact AOE au départ
	SpawnStartAoe(Start, Damage);

	// 2) Dash contrôlé (déplacement uniquement, pas de dégâts sur la trajectoire)
	SavedBrakingDecel          = OwnerMove->BrakingDecelerationWalking;
	SavedGroundFriction        = OwnerMove->GroundFriction;
	SavedBrakingFrictionFactor = OwnerMove->BrakingFrictionFactor;

	OwnerMove->BrakingDecelerationWalking = 0.f;
	OwnerMove->GroundFriction             = 0.f;
	OwnerMove->BrakingFrictionFactor      = 0.f;

	OwnerMove->StopMovementImmediately();
	OwnerMove->Velocity = Dir * DashSpeed;

	OwnerChar->GetWorldTimerManager().SetTimer(
		Timer_EndDash, this, &ULightningPower::EndDash, DashDuration, false);

	return true;
}

void ULightningPower::SpawnStartAoe(const FVector& Start, float BaseDamage)
{
	// --- VFX depuis DataAsset : d'abord Niagara, sinon Cascade ---
	if (UNiagaraSystem* NS = Config->LoadVfxNiagara())
	{
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(
			GetWorld(), NS, Start, FRotator::ZeroRotator, FVector(1.f), true);
	}
	else if (UParticleSystem* PS = Config->LoadVfxCascade())
	{
		UGameplayStatics::SpawnEmitterAtLocation(
			GetWorld(), PS, Start, FRotator::ZeroRotator);
	}

	// --- SFX ---
	if (USoundBase* AoeSFX = Config->LoadSfx())
	{
		UGameplayStatics::PlaySoundAtLocation(this, AoeSFX, Start);
	}

	// --- Dégâts ---
	const float AoeDamage = BaseDamage * StartAoeDamageMultiplier;

	TArray<AActor*> Ignore;
	if (OwnerChar.IsValid()) Ignore.Add(OwnerChar.Get());

	UE_LOG(LogTemp, Log, TEXT("[Lightning] StartAOE @%s  Radius=%.1f  Dmg=%.1f"),
		*Start.ToString(), StartAoeRadius, AoeDamage);

	// 1) Radial "classique" (avec LOS via Visibility) – utile pour tout le reste
	UGameplayStatics::ApplyRadialDamage(
		this,
		AoeDamage,
		Start,
		StartAoeRadius,
		UDamageType_Lightning::StaticClass(),
		Ignore,
		OwnerChar.Get(),
		OwnerChar.IsValid() ? OwnerChar->GetController() : nullptr,
		/*bDoFullDamage*/ false,
		/*DamagePreventionChannel*/ ECC_Visibility
	);

	// 2) Fallback No-LOS : force les dégâts UNIQUEMENT aux AElectricGenerator dans la sphère
	{
		TArray<AActor*> Overlapped;

		UKismetSystemLibrary::SphereOverlapActors(
			this,
			Start,
			StartAoeRadius,
			/*ObjectTypes*/ { UEngineTypes::ConvertToObjectType(ECC_WorldStatic),
							  UEngineTypes::ConvertToObjectType(ECC_WorldDynamic) },
			AElectricGenerator::StaticClass(),     // <<< ne retient que les générateurs
			/*ActorsToIgnore*/ Ignore,
			/*OutActors*/ Overlapped
		);

		int32 Forced = 0;
		for (AActor* A : Overlapped)
		{
			if (!A || A == OwnerChar.Get()) continue;

			UE_LOG(LogTemp, Warning, TEXT("[Lightning][NoLOS] generator hit -> %s"), *GetNameSafe(A));

			UGameplayStatics::ApplyDamage(
				A,
				AoeDamage,
				OwnerChar.IsValid() ? OwnerChar->GetController() : nullptr,
				OwnerChar.Get(),
				UDamageType_Lightning::StaticClass()
			);
			Forced++;
		}
		UE_LOG(LogTemp, Warning, TEXT("[Lightning] No-LOS fallback (Generators only) -> forced %d hits"), Forced);
	}
}


void ULightningPower::EndDash()
{
	if (!OwnerMove.IsValid()) return;

	OwnerMove->StopMovementImmediately();

	// Restaure les valeurs de friction
	OwnerMove->BrakingDecelerationWalking = SavedBrakingDecel;
	OwnerMove->GroundFriction             = SavedGroundFriction;
	OwnerMove->BrakingFrictionFactor      = SavedBrakingFrictionFactor;
}

void ULightningPower::OnEquipped_Implementation()
{
	// Passif : léger bonus de vitesse
	if (ACharacter* C = Cast<ACharacter>(OwnerComp ? OwnerComp->GetOwner() : nullptr))
		if (UCharacterMovementComponent* Move = C->GetCharacterMovement())
		{
			// ScalarB = bonus de vitesse configurable dans le DA (par défaut ~100)
			Move->MaxWalkSpeed += Config ? Config->Numbers.ScalarB : 100.f;
		}
}

void ULightningPower::OnUnequipped_Implementation()
{
	if (ACharacter* C = Cast<ACharacter>(OwnerComp ? OwnerComp->GetOwner() : nullptr))
		if (UCharacterMovementComponent* Move = C->GetCharacterMovement())
		{
			Move->MaxWalkSpeed -= Config ? Config->Numbers.ScalarB : 100.f;
		}
}
