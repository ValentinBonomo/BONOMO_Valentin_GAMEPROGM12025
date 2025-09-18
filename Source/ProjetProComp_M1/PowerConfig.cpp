#include "PowerConfig.h"
#include "NiagaraSystem.h"
#include "Particles/ParticleSystem.h"
#include "Sound/SoundBase.h"


UNiagaraSystem* UPowerConfig::LoadVfxNiagara() const
{
	return VfxNiagara_Activate.IsNull() ? nullptr : VfxNiagara_Activate.LoadSynchronous();
}
UParticleSystem* UPowerConfig::LoadVfxCascade() const
{
	return Vfx_Activate.IsNull() ? nullptr : Vfx_Activate.LoadSynchronous();
}
USoundBase* UPowerConfig::LoadSfx() const
{
	return Sfx_Activate.IsNull() ? nullptr : Sfx_Activate.LoadSynchronous();
}

UNiagaraSystem* UPowerConfig::LoadBurnIgniteNiagara() const
{
	return Passive_Burn.IgniteVfx_Niagara.IsNull() ? nullptr : Passive_Burn.IgniteVfx_Niagara.LoadSynchronous();
}
UParticleSystem* UPowerConfig::LoadBurnIgniteCascade() const
{
	return Passive_Burn.IgniteVfx_Cascade.IsNull() ? nullptr : Passive_Burn.IgniteVfx_Cascade.LoadSynchronous();
}
USoundBase* UPowerConfig::LoadBurnIgniteSfx() const
{
	return Passive_Burn.IgniteSfx.IsNull() ? nullptr : Passive_Burn.IgniteSfx.LoadSynchronous();
}
