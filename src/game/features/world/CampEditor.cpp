#include "core/commands/Command.hpp"
#include "core/commands/BoolCommand.hpp"
#include "game/backend/FiberPool.hpp"
#include "game/backend/Self.hpp"
#include "game/rdr/Natives.hpp"
#include "core/frontend/Notifications.hpp"

namespace YimMenu::Features
{
	// Camp Editor - Customize gang camp beyond normal limits

	class FullyUpgradeCamp : public Command
	{
		using Command::Command;

		virtual void OnCall() override
		{
			FiberPool::Push([] {
				// Unlock all camp upgrades
				// Note: These are the main camp upgrade flags in RDR2

				// Arthur's Lodging Upgrades
				STATS::STAT_SET_INT("CAMP_HEALTH_CURRENT"_J, 100, true);
				STATS::STAT_SET_INT("CAMP_PROVISIONS_CURRENT"_J, 100, true);
				STATS::STAT_SET_INT("CAMP_AMMO_CURRENT"_J, 100, true);

				// Unlock all camp features
				STATS::STAT_SET_BOOL("CAMP_UPGRADED_LODGINGS_1"_J, true, true);
				STATS::STAT_SET_BOOL("CAMP_UPGRADED_LODGINGS_2"_J, true, true);
				STATS::STAT_SET_BOOL("CAMP_UPGRADED_PROVISIONS"_J, true, true);
				STATS::STAT_SET_BOOL("CAMP_UPGRADED_ARMORY"_J, true, true);
				STATS::STAT_SET_BOOL("CAMP_UPGRADED_MEDICINE"_J, true, true);
				STATS::STAT_SET_BOOL("CAMP_UPGRADED_AMMUNITION"_J, true, true);

				Notifications::Show("Camp Editor", "Fully upgraded camp with all features!", NotificationType::Success);
			});
		}
	};

	static FullyUpgradeCamp _FullyUpgradeCamp{"fullyupgradecamp", "Fully Upgrade Camp", "Unlock all camp upgrades"};

	class FillCampSupplies : public Command
	{
		using Command::Command;

		virtual void OnCall() override
		{
			FiberPool::Push([] {
				// Max out all camp supplies
				STATS::STAT_SET_INT("CAMP_HEALTH_CURRENT"_J, 100, true);
				STATS::STAT_SET_INT("CAMP_PROVISIONS_CURRENT"_J, 100, true);
				STATS::STAT_SET_INT("CAMP_AMMO_CURRENT"_J, 100, true);

				Notifications::Show("Camp Editor", "Filled all camp supplies to max!", NotificationType::Success);
			});
		}
	};

	static FillCampSupplies _FillCampSupplies{"fillcampsupplies", "Fill Camp Supplies", "Max health, provisions, and ammo"};

	// Note: Teleport to camp removed - BLIP_GET_FIRST_INFO_ID native doesn't exist in RDR2

	class MaxCampMorale : public Command
	{
		using Command::Command;

		virtual void OnCall() override
		{
			FiberPool::Push([] {
				// Max camp morale
				STATS::STAT_SET_INT("CAMP_MORALE"_J, 100, true);
				STATS::STAT_SET_INT("CAMP_HEALTH_CURRENT"_J, 100, true);

				Notifications::Show("Camp Editor", "Maximized camp morale!", NotificationType::Success);
			});
		}
	};

	static MaxCampMorale _MaxCampMorale{"maxcampmorale", "Max Camp Morale", "Boost gang morale to maximum"};

	class DisableCampCosts : public BoolCommand
	{
		using BoolCommand::BoolCommand;

		virtual void OnEnable() override
		{
			// Set camp contribution to very high so costs are covered
			STATS::STAT_SET_INT("CAMP_HEALTH_CURRENT"_J, 999, true);
			STATS::STAT_SET_INT("CAMP_PROVISIONS_CURRENT"_J, 999, true);
			STATS::STAT_SET_INT("CAMP_AMMO_CURRENT"_J, 999, true);

			Notifications::Show("Camp Editor", "Camp costs disabled!", NotificationType::Success);
		}

		virtual void OnDisable() override
		{
			Notifications::Show("Camp Editor", "Camp costs re-enabled", NotificationType::Info);
		}
	};

	static DisableCampCosts _DisableCampCosts{"disablecampcosts", "Disable Camp Costs", "Keep camp supplies always full"};
}
