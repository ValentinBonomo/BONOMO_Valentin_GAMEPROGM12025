#include "PowerBase.h"
#include "PowerComponent.h"
#include "PowerConfig.h"
#include "Engine/World.h"
#include "TimerManager.h"

void UPowerBase::Init(UPowerComponent* InOwnerComp, UPowerConfig* InConfig)
{
	OwnerComp = InOwnerComp;
	Config    = InConfig;

	if (Config)
	{
		Element = Config->Element;
	}
}

bool UPowerBase::TryActivate()
{
	if (IsOnCooldown())
	{
		return false;
	}

	if (!ActivateNative())
	{
		return false;
	}

	StartCooldownFromConfig();
	return true;
}

bool UPowerBase::IsOnCooldown() const
{
	if (const UWorld* W = GetWorld())
	{
		return W->GetTimerManager().IsTimerActive(CooldownTimer);
	}
	return false;
}

float UPowerBase::GetCooldownRatio() const
{
	const UWorld* W = GetWorld();
	if (!W) return 0.f;

	if (!W->GetTimerManager().IsTimerActive(CooldownTimer) || LastCooldownDuration <= KINDA_SMALL_NUMBER)
	{
		return 0.f;
	}

	const float Remaining = W->GetTimerManager().GetTimerRemaining(CooldownTimer);
	return FMath::Clamp(Remaining / LastCooldownDuration, 0.f, 1.f);
}

void UPowerBase::StartCooldown(float Seconds)
{
	LastCooldownDuration = FMath::Max(Seconds, 0.01f);

	if (UWorld* W = GetWorld())
	{
		W->GetTimerManager().SetTimer(CooldownTimer, this, &UPowerBase::OnCooldownEnd, LastCooldownDuration, false);
	}
}

void UPowerBase::StartCooldownFromConfig()
{
	const float BaseCd = (Config ? Config->Numbers.Cooldown : 5.f);
	const float Mult   = (OwnerComp ? OwnerComp->GetGlobalCooldownMultiplier() : 1.f);
	StartCooldown(BaseCd * Mult);
}

void UPowerBase::OnCooldownEnd()
{
	if (UWorld* W = GetWorld())
	{
		W->GetTimerManager().ClearTimer(CooldownTimer);
	}
	LastCooldownDuration = 0.f;
}

void UPowerBase::OnEquipped_Implementation() {}
void UPowerBase::OnUnequipped_Implementation() {}

void UPowerBase::ApplyLiveCooldownScale(float Scale)
{
	if (Scale <= 0.f || FMath::IsNearlyEqual(Scale, 1.f)) return;
	if (!IsOnCooldown()) return;

	UWorld* W = GetWorld();
	if (!W) return;

	FTimerManager& TM = W->GetTimerManager();
	const float Remaining = TM.GetTimerRemaining(CooldownTimer);
	if (Remaining <= 0.f) return;

	const float NewRemaining = FMath::Max(Remaining * Scale, 0.01f);
	LastCooldownDuration = FMath::Max(LastCooldownDuration * Scale, 0.01f);

	TM.ClearTimer(CooldownTimer);
	TM.SetTimer(CooldownTimer, this, &UPowerBase::OnCooldownEnd, NewRemaining, false);

	UE_LOG(LogTemp, Warning,
		TEXT("[PowerBase] %s → cooldown live réduit : %.2fs -> %.2fs (scale=%.2f)"),
		*UEnum::GetValueAsString(Element), Remaining, NewRemaining, Scale);
}
