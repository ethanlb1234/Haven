#include "core/commands/BoolCommand.hpp"
#include "core/commands/LoopedCommand.hpp"
#include "game/backend/Self.hpp"
#include "game/rdr/Natives.hpp"

namespace YimMenu::Features
{
	static BoolCommand _WalkOnAir{"walkonair", "Walk On Air", "Walk on invisible platforms in the sky"};
	static BoolCommand _WalkOnWater{"walkonwater", "Walk On Water", "Walk across water surfaces"};

	class WalkOnWater : public LoopedCommand
	{
		using LoopedCommand::LoopedCommand;

		virtual void OnTick() override
		{
			auto ped = Self::GetPed();
			if (!ped.IsValid())
				return;

			if (_WalkOnWater.GetState())
			{
				// Prevent falling through water
				PED::SET_PED_CONFIG_FLAG(ped.GetHandle(), 65, true); // Walk on water flag
			}
		}

		virtual void OnDisable() override
		{
			auto ped = Self::GetPed();
			if (ped.IsValid())
			{
				PED::SET_PED_CONFIG_FLAG(ped.GetHandle(), 65, false);
			}
		}
	};

	static WalkOnWater _WalkOnWaterImpl{"walkonwaterimpl", "Walk On Water Impl", ""};

	class WalkOnAir : public LoopedCommand
	{
		using LoopedCommand::LoopedCommand;
		static inline Object platform = 0;

		virtual void OnTick() override
		{
			auto ped = Self::GetPed();
			if (!ped.IsValid())
				return;

			if (_WalkOnAir.GetState())
			{
				auto pos = ped.GetPosition();

				// Create invisible platform beneath player
				if (!platform || !ENTITY::DOES_ENTITY_EXIST(platform))
				{
					Hash model = "p_gen_plateform01x"_J; // Platform model
					STREAMING::REQUEST_MODEL(model, false);

					while (!STREAMING::HAS_MODEL_LOADED(model))
						ScriptMgr::Yield();

					platform = OBJECT::CREATE_OBJECT(model, pos.x, pos.y, pos.z - 1.5f, true, true, false, false, false);

					// Make invisible
					ENTITY::SET_ENTITY_VISIBLE(platform, false);
					ENTITY::SET_ENTITY_COLLISION(platform, true, false);
					ENTITY::FREEZE_ENTITY_POSITION(platform, true);

					STREAMING::SET_MODEL_AS_NO_LONGER_NEEDED(model);
				}
				else
				{
					// Move platform with player
					ENTITY::SET_ENTITY_COORDS(platform, pos.x, pos.y, pos.z - 1.5f, false, false, false, false);
				}
			}
		}

		virtual void OnDisable() override
		{
			if (platform && ENTITY::DOES_ENTITY_EXIST(platform))
			{
				ENTITY::DELETE_ENTITY(&platform);
				platform = 0;
			}
		}
	};

	static WalkOnAir _WalkOnAirImpl{"walkonairimpl", "Walk On Air Impl", ""};
}
