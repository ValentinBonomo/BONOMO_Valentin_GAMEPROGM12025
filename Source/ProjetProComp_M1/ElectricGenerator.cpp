#include "ElectricGenerator.h"
#include "Components/StaticMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundBase.h"
#include "Engine/DamageEvents.h"
#include "DamageType_Lightning.h"

AElectricGenerator::AElectricGenerator()
{
	PrimaryActorTick.bCanEverTick = false;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	SetRootComponent(Mesh);

	Mesh->SetCollisionProfileName(TEXT("BlockAll"));
	Mesh->SetSimulatePhysics(false);
	Mesh->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);

	SetCanBeDamaged(true);
}

void AElectricGenerator::BeginPlay()
{
	Super::BeginPlay();

	UE_LOG(LogTemp, Log, TEXT("[Generator] BeginPlay on %s  Powered=%s"),
		*GetName(), bIsPowered ? TEXT("true") : TEXT("false"));
}

float AElectricGenerator::TakeDamage(float DamageAmount,
                                     const FDamageEvent& DamageEvent,
                                     AController* EventInstigator,
                                     AActor* DamageCauser)
{
	Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	const UClass* DmgClass = DamageEvent.DamageTypeClass;
	const bool bIsLightning = DmgClass && DmgClass->IsChildOf(UDamageType_Lightning::StaticClass());

	UE_LOG(LogTemp, Log, TEXT("[Generator] TakeDamage: Causer=%s  DamageType=%s  Lightning? %s  Amount=%.1f"),
		*GetNameSafe(DamageCauser),
		DmgClass ? *DmgClass->GetName() : TEXT("None"),
		bIsLightning ? TEXT("YES") : TEXT("NO"),
		FMath::Max(0.f, DamageAmount));

	if (bIsLightning)
	{
		const bool bWasPowered = bIsPowered;
		bIsPowered = true;

		if (!bWasPowered)
		{
			UE_LOG(LogTemp, Warning, TEXT("[Generator] ACTIVATED by electricity!"));

			if (PowerOnSFX && !bSFXPlayedOnce)
			{
				UGameplayStatics::PlaySoundAtLocation(GetWorld(), PowerOnSFX, GetActorLocation());
				bSFXPlayedOnce = true;
			}

			OnPowered();
		}
	}

	return DamageAmount;
}
