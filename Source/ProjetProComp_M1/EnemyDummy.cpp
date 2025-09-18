#include "EnemyDummy.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "TimerManager.h"
#include "Engine/World.h"
#include "UObject/ConstructorHelpers.h"
#include "Components/SkeletalMeshComponent.h"

AEnemyDummy::AEnemyDummy()
{
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.f);

	if (UCharacterMovementComponent* Move = GetCharacterMovement())
	{
		Move->MaxWalkSpeed = 200.f;
	}

	static ConstructorHelpers::FObjectFinder<USkeletalMesh> MeshObj(
		TEXT("/Game/Characters/Mannequins/Meshes/SKM_Quinn")
	);

	if (MeshObj.Succeeded())
	{
		GetMesh()->SetSkeletalMesh(MeshObj.Object);
		GetMesh()->SetRelativeLocation(FVector(0.f, 0.f, -90.f));
		GetMesh()->SetRelativeRotation(FRotator(0.f, -90.f, 0.f));
	}
}

void AEnemyDummy::BeginPlay()
{
	Super::BeginPlay();

	if (UCharacterMovementComponent* Move = GetCharacterMovement())
	{
		BaseWalkSpeed = Move->MaxWalkSpeed;
		UE_LOG(LogTemp, Warning, TEXT("[Dummy] Vitesse de base = %.1f"), BaseWalkSpeed);
	}

	UE_LOG(LogTemp, Log, TEXT("[Dummy] HP init = %.1f"), Health);
}

float AEnemyDummy::TakeDamage(float DamageAmount, const FDamageEvent& DamageEvent,
							  AController* EventInstigator, AActor* DamageCauser)
{
	Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	const float Applied = FMath::Max(0.f, DamageAmount);
	Health = FMath::Max(0.f, Health - Applied);

	UE_LOG(LogTemp, Warning, TEXT("[Dummy] %.1f dmg reçu de %s -> HP=%.1f"),
		Applied, *GetNameSafe(DamageCauser), Health);

	if (UCharacterMovementComponent* Move = GetCharacterMovement())
	{
		UE_LOG(LogTemp, Log, TEXT("[Dummy] Vitesse actuelle = %.1f"), Move->MaxWalkSpeed);
	}

	if (Health <= 0.f)
	{
		UE_LOG(LogTemp, Warning, TEXT("[Dummy] Destroy (HP <= 0)"));
		Destroy();
	}

	return Applied;
}

void AEnemyDummy::ApplySlow(float Percent, float Duration)
{
	Percent  = FMath::Clamp(Percent, 0.f, 0.95f);
	Duration = FMath::Max(Duration, 0.01f);

	UCharacterMovementComponent* Move = GetCharacterMovement();
	if (!Move || BaseWalkSpeed <= 0.f)
	{
		UE_LOG(LogTemp, Warning, TEXT("[Dummy] ApplySlow ignoré (pas de movement ou base inconnue)"));
		return;
	}

	const float Mult = 1.f - Percent;                 // 0.8 = -20%
	Move->MaxWalkSpeed = BaseWalkSpeed * Mult;

	UE_LOG(LogTemp, Warning, TEXT("[Dummy] SLOW %.0f%% pendant %.2fs -> Vitesse actuelle = %.1f"),
		Percent * 100.f, Duration, Move->MaxWalkSpeed);
	
	if (Timer_Slow.IsValid())
	{
		GetWorldTimerManager().ClearTimer(Timer_Slow);
	}
	GetWorldTimerManager().SetTimer(Timer_Slow, this, &AEnemyDummy::ClearSlow, Duration, false);
}

void AEnemyDummy::ClearSlow()
{
	if (UCharacterMovementComponent* Move = GetCharacterMovement())
	{
		Move->MaxWalkSpeed = BaseWalkSpeed;
		UE_LOG(LogTemp, Warning, TEXT("[Dummy] SLOW terminé -> Vitesse = %.1f"), Move->MaxWalkSpeed);
	}
	if (Timer_Slow.IsValid())
	{
		GetWorldTimerManager().ClearTimer(Timer_Slow);
	}
}
