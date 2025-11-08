// DISABLED: Vehicle custom color natives don't exist in RDR2
// Ref: scrCommand_dump_b1491.50 has no SET_VEHICLE_CUSTOM_PRIMARY_COLOUR or CLEAR_VEHICLE_CUSTOM_PRIMARY_COLOUR
// These are GTA5-only natives

#if 0
#include "core/commands/LoopedCommand.hpp"
#include "game/backend/Self.hpp"
#include "game/rdr/Natives.hpp"

namespace YimMenu::Features
{
	class RainbowVehicle : public LoopedCommand
	{
		using LoopedCommand::LoopedCommand;

		int m_ColorIndex = 0;
		int m_UpdateTick = 0;

		virtual void OnTick() override
		{
			auto ped = Self::GetPed();
			if (!ped.IsValid())
				return;

			auto vehicle = PED::GET_VEHICLE_PED_IS_IN(ped.GetHandle(), false);
			if (!ENTITY::DOES_ENTITY_EXIST(vehicle))
				return;

			// Update color every 5 ticks
			if (++m_UpdateTick < 5)
				return;

			m_UpdateTick = 0;

			// Rainbow RGB values
			static const int rainbowR[] = {255, 255, 255, 0, 0, 0, 128, 255};
			static const int rainbowG[] = {0, 165, 255, 255, 255, 0, 0, 0};
			static const int rainbowB[] = {0, 0, 0, 0, 255, 255, 255, 192};

			int r = rainbowR[m_ColorIndex];
			int g = rainbowG[m_ColorIndex];
			int b = rainbowB[m_ColorIndex];

			VEHICLE::SET_VEHICLE_CUSTOM_PRIMARY_COLOUR(vehicle, r, g, b);
			VEHICLE::SET_VEHICLE_CUSTOM_SECONDARY_COLOUR(vehicle, r, g, b);

			m_ColorIndex = (m_ColorIndex + 1) % 8;
		}

		virtual void OnDisable() override
		{
			auto ped = Self::GetPed();
			if (!ped.IsValid())
				return;

			auto vehicle = PED::GET_VEHICLE_PED_IS_IN(ped.GetHandle(), false);
			if (ENTITY::DOES_ENTITY_EXIST(vehicle))
			{
				// Reset to default colors
				VEHICLE::CLEAR_VEHICLE_CUSTOM_PRIMARY_COLOUR(vehicle);
				VEHICLE::CLEAR_VEHICLE_CUSTOM_SECONDARY_COLOUR(vehicle);
			}
		}
	};

	static RainbowVehicle _RainbowVehicle{"rainbowvehicle", "Rainbow Vehicle", "Makes your vehicle cycle through rainbow colors!"};
}
#endif
