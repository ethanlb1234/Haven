#include "core/commands/ListCommand.hpp"
#include "core/commands/LoopedCommand.hpp"
#include "game/backend/Self.hpp"
#include "game/rdr/Natives.hpp"

namespace YimMenu::Features
{
	enum class ParticleTrailType
	{
		NONE,
		BUTTERFLIES,
		FIREFLIES,
		SPARKLES,
		FIRE,
		WATER_SPLASH,
		SNOW,
		FLOWER_PETALS,
		MAGIC_SPARKLES,
		SMOKE_TRAIL
	};

	static const std::pair<ParticleTrailType, const char*> g_ParticleTrails[] = {
		{ParticleTrailType::NONE, "None"},
		{ParticleTrailType::BUTTERFLIES, "Butterflies"},
		{ParticleTrailType::FIREFLIES, "Fireflies"},
		{ParticleTrailType::SPARKLES, "Sparkles"},
		{ParticleTrailType::FIRE, "Fire Trail"},
		{ParticleTrailType::WATER_SPLASH, "Water Droplets"},
		{ParticleTrailType::SNOW, "Snowflakes"},
		{ParticleTrailType::FLOWER_PETALS, "Flower Petals"},
		{ParticleTrailType::MAGIC_SPARKLES, "Magic Sparkles"},
		{ParticleTrailType::SMOKE_TRAIL, "Smoke Trail"}
	};

	static ListCommand _ParticleTrail{"particletrail", "Particle Trail", "Leave a trail of particles as you move", g_ParticleTrails, (int)ParticleTrailType::NONE};

	class ParticleTrails : public LoopedCommand
	{
		using LoopedCommand::LoopedCommand;

		const char* GetParticleEffect(ParticleTrailType type)
		{
			switch (type)
			{
			case ParticleTrailType::BUTTERFLIES:
				return "ent_anim_butterfly"; // Butterfly swarm
			case ParticleTrailType::FIREFLIES:
				return "scr_mob_fireflies"; // Fireflies
			case ParticleTrailType::SPARKLES:
				return "scr_win_collect_gold"; // Gold sparkles
			case ParticleTrailType::FIRE:
				return "proj_flare_fuse"; // Fire trail
			case ParticleTrailType::WATER_SPLASH:
				return "scr_wheel_splash_water"; // Water droplets
			case ParticleTrailType::SNOW:
				return "scr_snow_impact"; // Snow particles
			case ParticleTrailType::FLOWER_PETALS:
				return "scr_flowers_fallen"; // Flower petals
			case ParticleTrailType::MAGIC_SPARKLES:
				return "scr_win_magic_sparkle"; // Magical sparkles
			case ParticleTrailType::SMOKE_TRAIL:
				return "scr_firedamp_smoke"; // Smoke trail
			default:
				return nullptr;
			}
		}

		const char* GetParticleAsset(ParticleTrailType type)
		{
			// Most effects use "core" asset
			return "core";
		}

		virtual void OnTick() override
		{
			auto type = (ParticleTrailType)_ParticleTrail.GetState();
			if (type == ParticleTrailType::NONE)
				return;

			auto ped = Self::GetPed();
			if (!ped.IsValid())
				return;

			const char* asset = GetParticleAsset(type);
			const char* effect = GetParticleEffect(type);

			if (!effect)
				return;

			// Request and load particle asset
			STREAMING::REQUEST_NAMED_PTFX_ASSET(rage::joaat(asset));
			if (STREAMING::HAS_NAMED_PTFX_ASSET_LOADED(rage::joaat(asset)))
			{
				GRAPHICS::USE_PARTICLE_FX_ASSET(asset);

				// Get player position
				auto pos = ped.GetPosition();

				// Spawn particle effect at player's feet
				int boneIndex = PED::GET_PED_BONE_INDEX(ped.GetHandle(), 0x0); // Root bone

				GRAPHICS::_START_PARTICLE_FX_NON_LOOPED_ON_PED_BONE_2(
					effect,
					ped.GetHandle(),
					0.0f, 0.0f, -0.5f,  // Offset to feet
					0.0f, 0.0f, 0.0f,   // No rotation
					boneIndex,
					0.5f,               // Medium scale
					false, false, false
				);
			}
		}
	};

	static ParticleTrails _ParticleTrails{"particletrails", "Particle Trails", "Magical trails follow you everywhere"};
}
