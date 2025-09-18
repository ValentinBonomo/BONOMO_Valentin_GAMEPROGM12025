#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ElectricGenerator.generated.h"

class UStaticMeshComponent;
class USoundBase;


UCLASS()
class AElectricGenerator : public AActor
{
	GENERATED_BODY()

public:
	AElectricGenerator();

	virtual float TakeDamage(float DamageAmount,
							 struct FDamageEvent const& DamageEvent,
							 class AController* EventInstigator,
							 AActor* DamageCauser) override;

protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Generator", meta=(AllowPrivateAccess="true"))
	UStaticMeshComponent* Mesh = nullptr;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Generator")
	bool bIsPowered = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Generator|FX")
	USoundBase* PowerOnSFX = nullptr;

private:
	bool bSFXPlayedOnce = false;

public:
	UFUNCTION(BlueprintImplementableEvent, Category="Generator")
	void OnPowered();
};
