#include "FireballProjectile.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystem.h"
#include "Particles/ParticleSystemComponent.h"
#include "Sound/SoundBase.h"
#include "DamageType_Fire.h"
#include "PowerComponent.h"

AFireballProjectile::AFireballProjectile()
{
	PrimaryActorTick.bCanEverTick = false;

	Collision = CreateDefaultSubobject<USphereComponent>(TEXT("Collision"));
	SetRootComponent(Collision);
	Collision->InitSphereRadius(10.f);
	Collision->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	Collision->SetCollisionObjectType(ECC_WorldDynamic);
	Collision->SetCollisionResponseToAllChannels(ECR_Block);
	Collision->SetNotifyRigidBodyCollision(true);
	Collision->OnComponentHit.AddDynamic(this, &AFireballProjectile::OnHit);

	Movement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("Movement"));
	Movement->InitialSpeed = 2000.f;
	Movement->MaxSpeed = 2000.f;
	Movement->ProjectileGravityScale = 0.f;
	Movement->bRotationFollowsVelocity = true;
	Movement->bShouldBounce = false;

	TrailPSC = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("TrailPSC"));
	TrailPSC->SetupAttachment(RootComponent);
	TrailPSC->bAutoActivate = true;

	InitialLifeSpan = 10.f;
	Tags.Add(FName("Fire"));
	
}

void AFireballProjectile::BeginPlay()
{
	Super::BeginPlay();
}

void AFireballProjectile::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor,
								UPrimitiveComponent* OtherComp, FVector NormalImpulse,
								const FHitResult& Hit)
{
	if (bHasExploded) return;
	bHasExploded = true;
	
	if (OtherActor && OtherActor != this && OtherActor != Shooter)
	{
		UE_LOG(LogTemp, Warning, TEXT("[BDF] %s a pris %.1f dmg"),
			*GetNameSafe(OtherActor), Damage);

		UGameplayStatics::ApplyDamage(
			OtherActor,
			Damage,
			Shooter ? Shooter->GetInstigatorController() : nullptr,
			Shooter,
			UDamageType_Fire::StaticClass()
		);
		
		if (Shooter)
		{
			if (UPowerComponent* PC = Shooter->FindComponentByClass<UPowerComponent>())
			{
				PC->NotifyPlayerHitActor(OtherActor);
			}
		}
	}
	
	Explode(Hit, OtherActor);
}



void AFireballProjectile::Explode(const FHitResult& Hit, AActor* FirstHitActor)
{
	const FVector Origin = Hit.bBlockingHit ? FVector(Hit.ImpactPoint) : GetActorLocation();
	
	if (ImpactCascade)
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ImpactCascade, Origin, FRotator::ZeroRotator);
	}
	if (ImpactSnd)
	{
		UGameplayStatics::PlaySoundAtLocation(this, ImpactSnd, Origin);
	}
	
	TArray<AActor*> Ignore;
	Ignore.Add(this);
	if (FirstHitActor) { Ignore.Add(FirstHitActor); }

	UGameplayStatics::ApplyRadialDamage(
		this,
		Damage,
		Origin,
		Radius,
		nullptr,
		Ignore,
		Shooter, 
		Shooter ? Shooter->GetInstigatorController() : nullptr,
		true
	);

	SetLifeSpan(0.01f);
}
