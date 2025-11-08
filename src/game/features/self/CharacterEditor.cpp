#include "core/commands/Command.hpp"
#include "core/commands/BoolCommand.hpp"
#include "game/backend/FiberPool.hpp"
#include "game/backend/Self.hpp"
#include "game/rdr/Natives.hpp"
#include "core/frontend/Notifications.hpp"

namespace YimMenu::Features
{
	// Character Editor - Edit Arthur/John appearance
	// Note: Many features removed due to non-existent RDR2 natives (weight, honor, hair growth)

	class MaxAllStats : public Command
	{
		using Command::Command;

		virtual void OnCall() override
		{
			FiberPool::Push([] {
				auto ped = Self::GetPed();
				if (!ped.IsValid())
					return;

				// Max health
				PED::SET_PED_MAX_HEALTH(ped.GetHandle(), 999);
				ped.SetHealth(999);

				// Max stamina core
				ATTRIBUTE::_SET_ATTRIBUTE_CORE_VALUE(ped.GetHandle(), 1, 100); // ATTRIBUTE_CORE_STAMINA

				// Max dead eye core
				ATTRIBUTE::_SET_ATTRIBUTE_CORE_VALUE(ped.GetHandle(), 2, 100); // ATTRIBUTE_CORE_DEADEYE

				// Restore bars
				PLAYER::RESTORE_PLAYER_STAMINA(Self::GetPlayer().GetId(), 100.0f);
				PLAYER::_SPECIAL_ABILITY_RESTORE_BY_AMOUNT(Self::GetPlayer().GetId(), 100.0f, 0, 0, 1);

				Notifications::Show("Character Editor", "Maxed health, stamina, and dead eye!", NotificationType::Success);
			});
		}
	};

	static MaxAllStats _MaxAllStats{"maxallstats", "Max All Stats", "Max health, stamina, and dead eye"};

	class CleanCharacter : public Command
	{
		using Command::Command;

		virtual void OnCall() override
		{
			FiberPool::Push([] {
				auto ped = Self::GetPed();
				if (!ped.IsValid())
					return;

				// Clean character
				PED::CLEAR_PED_WETNESS(ped.GetHandle());
				PED::CLEAR_PED_BLOOD_DAMAGE(ped.GetHandle());
				PED::CLEAR_PED_DAMAGE_DECAL_BY_ZONE(ped.GetHandle(), 10, "ALL");
				PED::CLEAR_PED_ENV_DIRT(ped.GetHandle());

				Notifications::Show("Character Editor", "Character cleaned!", NotificationType::Success);
			});
		}
	};

	static CleanCharacter _CleanCharacter{"cleancharacter", "Clean Character", "Remove all dirt, blood, and wetness"};

	class MakeCharacterDirty : public Command
	{
		using Command::Command;

		virtual void OnCall() override
		{
			FiberPool::Push([] {
				auto ped = Self::GetPed();
				if (!ped.IsValid())
					return;

				// Make character dirty
				PED::SET_PED_WETNESS_HEIGHT(ped.GetHandle(), 2.0f);
				PED::_SET_PED_DIRT_CLEANED(ped.GetHandle(), 0.0f, 0, true, true);

				Notifications::Show("Character Editor", "Made character dirty!", NotificationType::Info);
			});
		}
	};

	static MakeCharacterDirty _MakeCharacterDirty{"makecharacterdirty", "Make Dirty", "Add dirt and wetness to character"};

	class HealCharacter : public Command
	{
		using Command::Command;

		virtual void OnCall() override
		{
			FiberPool::Push([] {
				auto ped = Self::GetPed();
				if (!ped.IsValid())
					return;

				// Heal
				ped.SetHealth(ped.GetMaxHealth());

				// Restore cores
				ATTRIBUTE::_SET_ATTRIBUTE_CORE_VALUE(ped.GetHandle(), 0, 100); // Health core
				ATTRIBUTE::_SET_ATTRIBUTE_CORE_VALUE(ped.GetHandle(), 1, 100); // Stamina core
				ATTRIBUTE::_SET_ATTRIBUTE_CORE_VALUE(ped.GetHandle(), 2, 100); // Dead eye core

				// Restore bars
				PLAYER::RESTORE_PLAYER_STAMINA(Self::GetPlayer().GetId(), 100.0f);
				PLAYER::_SPECIAL_ABILITY_RESTORE_BY_AMOUNT(Self::GetPlayer().GetId(), 100.0f, 0, 0, 1);

				Notifications::Show("Character Editor", "Character fully healed!", NotificationType::Success);
			});
		}
	};

	static HealCharacter _HealCharacter{"healcharacter", "Heal Character", "Fully heal health, stamina, and dead eye"};
}
