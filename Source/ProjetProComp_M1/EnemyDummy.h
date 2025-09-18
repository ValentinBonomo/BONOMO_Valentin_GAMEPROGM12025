#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "EnemyDummy.generated.h"

UCLASS()
class AEnemyDummy : public ACharacter
{
	GENERATED_BODY()

public:
	AEnemyDummy();
	
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent,
							 class AController* EventInstigator, AActor* DamageCauser) override;
	
	UFUNCTION(BlueprintCallable, Category="Dummy|Status")
	void ApplySlow(float Percent, float Duration);

	UFUNCTION(BlueprintCallable, Category="Dummy|Status")
	void ClearSlow();

protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Dummy|Stats", meta=(ClampMin="0", AllowPrivateAccess="true"))
	float Health = 50.f;
	
	UPROPERTY(VisibleAnywhere, Category="Dummy|Status")
	float BaseWalkSpeed = 0.f;
	
	FTimerHandle Timer_Slow;
};
