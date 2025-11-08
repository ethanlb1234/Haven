#include "core/commands/Command.hpp"
#include "core/commands/BoolCommand.hpp"
#include "core/commands/ListCommand.hpp"
#include "core/commands/LoopedCommand.hpp"
#include "game/backend/FiberPool.hpp"
#include "game/backend/ScriptMgr.hpp"
#include "game/backend/Self.hpp"
#include "game/rdr/Natives.hpp"
#include "core/frontend/Notifications.hpp"

namespace YimMenu::Features
{
	// Particle Effects - Spawn cool visual effects!

	enum class ParticleEffect
	{
		FIRE,
		SMOKE,
		SPARKS,
		EXPLOSION,
		BLOOD_MIST,
		DUST,
		LIGHTNING,
		FIREWORKS_RED,
		FIREWORKS_BLUE,
		FIREWORKS_GREEN,
		WATER_SPLASH,
		SNOW,
		LEAVES,
		TORCH_FIRE
	};

	static const std::vector<std::pair<int, const char*>> g_ParticleEffects = {
		{(int)ParticleEffect::FIRE, "Fire"},
		{(int)ParticleEffect::SMOKE, "Smoke"},
		{(int)ParticleEffect::SPARKS, "Sparks"},
		{(int)ParticleEffect::EXPLOSION, "Explosion"},
		{(int)ParticleEffect::BLOOD_MIST, "Blood Mist"},
		{(int)ParticleEffect::DUST, "Dust Cloud"},
		{(int)ParticleEffect::LIGHTNING, "Lightning"},
		{(int)ParticleEffect::FIREWORKS_RED, "Fireworks (Red)"},
		{(int)ParticleEffect::FIREWORKS_BLUE, "Fireworks (Blue)"},
		{(int)ParticleEffect::FIREWORKS_GREEN, "Fireworks (Green)"},
		{(int)ParticleEffect::WATER_SPLASH, "Water Splash"},
		{(int)ParticleEffect::SNOW, "Snow"},
		{(int)ParticleEffect::LEAVES, "Leaves"},
		{(int)ParticleEffect::TORCH_FIRE, "Torch Fire"}
	};

	static ListCommand _ParticleEffect{"particleeffect", "Effect Type", "Choose particle effect", g_ParticleEffects, 0};
	static BoolCommand _ContinuousEffect{"continuouseffect", "Continuous", "Keep spawning effect"};

	struct ParticleInfo
	{
		const char* dict;
		const char* name;
		float scale;
	};

	ParticleInfo GetParticleInfo(ParticleEffect effect)
	{
		switch (effect)
		{
		case ParticleEffect::FIRE:
			return {"core", "ent_amb_torch_fire", 1.0f};
		case ParticleEffect::SMOKE:
			return {"core", "ent_amb_smoke_general", 1.0f};
		case ParticleEffect::SPARKS:
			return {"core", "ent_sht_sparks", 1.0f};
		case ParticleEffect::EXPLOSION:
			return {"core", "exp_air_molotov", 1.5f};
		case ParticleEffect::BLOOD_MIST:
			return {"core", "blood_mist", 1.0f};
		case ParticleEffect::DUST:
			return {"core", "ent_amb_dust_settle_sml", 2.0f};
		case ParticleEffect::LIGHTNING:
			return {"core", "ent_amb_elec_crackle_sp", 1.0f};
		case ParticleEffect::FIREWORKS_RED:
			return {"core", "exp_firework_red", 1.0f};
		case ParticleEffect::FIREWORKS_BLUE:
			return {"core", "exp_firework_blue", 1.0f};
		case ParticleEffect::FIREWORKS_GREEN:
			return {"core", "exp_firework_green", 1.0f};
		case ParticleEffect::WATER_SPLASH:
			return {"core", "ent_amb_water_splash", 1.0f};
		case ParticleEffect::SNOW:
			return {"core", "ent_amb_snow", 1.0f};
		case ParticleEffect::LEAVES:
			return {"core", "ent_amb_leaves", 1.0f};
		case ParticleEffect::TORCH_FIRE:
			return {"core", "ent_amb_torch_fire", 1.5f};
		default:
			return {"core", "ent_amb_smoke_general", 1.0f};
		}
	}

	class SpawnParticleEffect : public Command
	{
		using Command::Command;

		virtual void OnCall() override
		{
			FiberPool::Push([] {
				auto ped = Self::GetPed();
				if (!ped.IsValid())
					return;

				ParticleEffect effectType = (ParticleEffect)_ParticleEffect.GetState();
				ParticleInfo info = GetParticleInfo(effectType);

				auto pos = ped.GetPosition();

				// Request particle dictionary
				STREAMING::REQUEST_NAMED_PTFX_ASSET(Joaat(info.dict));
				int attempts = 0;
				while (!STREAMING::HAS_NAMED_PTFX_ASSET_LOADED(Joaat(info.dict)) && attempts < 50)
				{
					ScriptMgr::Yield();
					attempts++;
				}

				if (!STREAMING::HAS_NAMED_PTFX_ASSET_LOADED(Joaat(info.dict)))
				{
					Notifications::Show("Particle Effects", "Failed to load particle effect!", NotificationType::Error);
					return;
				}

				// Use particle effect
				GRAPHICS::USE_PARTICLE_FX_ASSET(info.dict);

				// Spawn particle
				GRAPHICS::START_NETWORKED_PARTICLE_FX_NON_LOOPED_AT_COORD(
					info.name,
					pos.x, pos.y, pos.z + 1.0f,
					0.0f, 0.0f, 0.0f,
					info.scale,
					false, false, false
				);

				Notifications::Show("Particle Effects", std::format("Spawned {}!", g_ParticleEffects[(int)effectType].second), NotificationType::Success);

				STREAMING::REMOVE_NAMED_PTFX_ASSET(Joaat(info.dict));
			});
		}
	};

	static SpawnParticleEffect _SpawnParticleEffect{"spawnparticleeffect", "Spawn Effect", "Spawn selected particle effect"};

	class ContinuousParticles : public LoopedCommand
	{
		using LoopedCommand::LoopedCommand;
		static inline int tickCount = 0;

		virtual void OnTick() override
		{
			if (!_ContinuousEffect.GetState())
				return;

			tickCount++;
			if (tickCount % 60 != 0) // Spawn every 60 ticks (~1 second)
				return;

			auto ped = Self::GetPed();
			if (!ped.IsValid())
				return;

			ParticleEffect effectType = (ParticleEffect)_ParticleEffect.GetState();
			ParticleInfo info = GetParticleInfo(effectType);

			auto pos = ped.GetPosition();

			if (STREAMING::HAS_NAMED_PTFX_ASSET_LOADED(Joaat(info.dict)))
			{
				GRAPHICS::USE_PARTICLE_FX_ASSET(info.dict);
				GRAPHICS::START_NETWORKED_PARTICLE_FX_NON_LOOPED_AT_COORD(
					info.name,
					pos.x, pos.y, pos.z + 1.0f,
					0.0f, 0.0f, 0.0f,
					info.scale,
					false, false, false
				);
			}
			else
			{
				STREAMING::REQUEST_NAMED_PTFX_ASSET(Joaat(info.dict));
			}
		}

		virtual void OnDisable() override
		{
			tickCount = 0;
		}
	};

	static ContinuousParticles _ContinuousParticles{"continuousparticles", "Continuous Particles", ""};

	// Special effects
	class FireworksShow : public Command
	{
		using Command::Command;

		virtual void OnCall() override
		{
			FiberPool::Push([] {
				auto ped = Self::GetPed();
				if (!ped.IsValid())
					return;

				auto pos = ped.GetPosition();

				STREAMING::REQUEST_NAMED_PTFX_ASSET("core"_J);
				while (!STREAMING::HAS_NAMED_PTFX_ASSET_LOADED("core"_J))
					ScriptMgr::Yield();

				GRAPHICS::USE_PARTICLE_FX_ASSET("core");

				// Spawn multiple fireworks
				for (int i = 0; i < 10; i++)
				{
					float xOffset = (rand() % 20) - 10.0f;
					float yOffset = (rand() % 20) - 10.0f;
					float zOffset = (rand() % 15) + 10.0f;

					const char* fireworkEffects[] = {
						"exp_firework_red",
						"exp_firework_blue",
						"exp_firework_green"
					};

					const char* effect = fireworkEffects[rand() % 3];

					GRAPHICS::START_NETWORKED_PARTICLE_FX_NON_LOOPED_AT_COORD(
						effect,
						pos.x + xOffset, pos.y + yOffset, pos.z + zOffset,
						0.0f, 0.0f, 0.0f,
						2.0f,
						false, false, false
					);

					ScriptMgr::Yield(200); // Wait 200ms between fireworks
				}

				STREAMING::REMOVE_NAMED_PTFX_ASSET("core"_J);

				Notifications::Show("Particle Effects", "Fireworks show complete!", NotificationType::Success);
			});
		}
	};

	static FireworksShow _FireworksShow{"fireworksshow", "Fireworks Show", "Spawn epic fireworks display"};
}
