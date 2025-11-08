#include "core/commands/Command.hpp"
#include "core/commands/BoolCommand.hpp"
#include "core/commands/IntCommand.hpp"
#include "game/backend/FiberPool.hpp"
#include "game/backend/ScriptMgr.hpp"
#include "game/backend/Self.hpp"
#include "game/rdr/Natives.hpp"
#include "core/frontend/Notifications.hpp"

namespace YimMenu::Features
{
	// Character Editor - Edit Arthur/John appearance

	static IntCommand _CharacterWeight{"characterweight", "Weight", "Character weight (-100 to 100)", 0, -100, 100};
	static BoolCommand _MaxHonor{"maxhonor", "Max Honor", "Set honor to maximum"};
	static BoolCommand _MaxDeadEye{"maxdeadeye", "Infinite Dead Eye", "Unlimited dead eye"};

	class ApplyCharacterWeight : public Command
	{
		using Command::Command;

		virtual void OnCall() override
		{
			FiberPool::Push([] {
				auto ped = Self::GetPed();
				if (!ped.IsValid())
					return;

				float weight = _CharacterWeight.GetState() / 100.0f;

				// Set character weight
				PED::_SET_PED_BODY_COMPONENT(ped.GetHandle(), weight);

				Notifications::Show("Character Editor", std::format("Set weight to {}", _CharacterWeight.GetState()), NotificationType::Success);
			});
		}
	};

	static ApplyCharacterWeight _ApplyCharacterWeight{"applycharacterweight", "Apply Weight", "Set character weight"};

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

				// Max stamina
				PED::_CHANGE_PED_STAMINA(ped.GetHandle(), 999.0f);

				// Max dead eye
				PLAYER::_SET_PLAYER_MAX_DEAD_EYE(Self::GetPlayer().GetId(), 999.0f, 0.0f);
				PLAYER::_SET_PLAYER_DEAD_EYE(Self::GetPlayer().GetId(), 999.0f);

				// Max honor
				PLAYER::_SET_PLAYER_HONOR(Self::GetPlayer().GetId(), 1.0f);

				Notifications::Show("Character Editor", "Maxed all character stats!", NotificationType::Success);
			});
		}
	};

	static MaxAllStats _MaxAllStats{"maxallstats", "Max All Stats", "Max health, stamina, dead eye, and honor"};

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

	class ResetCharacter : public Command
	{
		using Command::Command;

		virtual void OnCall() override
		{
			FiberPool::Push([] {
				auto ped = Self::GetPed();
				if (!ped.IsValid())
					return;

				// Reset to default appearance
				PED::_SET_PED_BODY_COMPONENT(ped.GetHandle(), 0.0f);
				PED::CLEAR_PED_WETNESS(ped.GetHandle());
				PED::CLEAR_PED_BLOOD_DAMAGE(ped.GetHandle());
				PED::CLEAR_PED_DAMAGE_DECAL_BY_ZONE(ped.GetHandle(), 10, "ALL");
				PED::CLEAR_PED_ENV_DIRT(ped.GetHandle());

				Notifications::Show("Character Editor", "Reset character appearance!", NotificationType::Info);
			});
		}
	};

	static ResetCharacter _ResetCharacter{"resetcharacter", "Reset Character", "Reset to default appearance"};

	class SetHonorMax : public Command
	{
		using Command::Command;

		virtual void OnCall() override
		{
			PLAYER::_SET_PLAYER_HONOR(Self::GetPlayer().GetId(), 1.0f);
			Notifications::Show("Character Editor", "Honor set to maximum!", NotificationType::Success);
		}
	};

	class SetHonorMin : public Command
	{
		using Command::Command;

		virtual void OnCall() override
		{
			PLAYER::_SET_PLAYER_HONOR(Self::GetPlayer().GetId(), -1.0f);
			Notifications::Show("Character Editor", "Honor set to minimum (dishonorable)!", NotificationType::Warning);
		}
	};

	static SetHonorMax _SetHonorMax{"sethonormax", "Max Honor", "Set honor to maximum (honorable)"};
	static SetHonorMin _SetHonorMin{"sethonormin", "Min Honor", "Set honor to minimum (dishonorable)"};

	class GrowHair : public Command
	{
		using Command::Command;

		virtual void OnCall() override
		{
			FiberPool::Push([] {
				auto ped = Self::GetPed();
				if (!ped.IsValid())
					return;

				// Grow hair and beard to maximum
				PED::_SET_PED_HAIR_GROWTH(ped.GetHandle(), 1.0f);

				Notifications::Show("Character Editor", "Hair grown to max length!", NotificationType::Success);
			});
		}
	};

	static GrowHair _GrowHair{"growhair", "Grow Hair", "Instantly grow hair and beard to max"};
}
