#include "core/commands/LoopedCommand.hpp"
#include "game/backend/Self.hpp"
#include "game/rdr/Natives.hpp"

namespace YimMenu::Features
{
	class FlamingLasso : public LoopedCommand
	{
		using LoopedCommand::LoopedCommand;

		virtual void OnTick() override
		{
			auto ped = Self::GetPed();
			if (!ped.IsValid())
				return;

			// Check if player is using lasso
			Hash currentWeapon;
			if (WEAPON::GET_CURRENT_PED_WEAPON(ped.GetHandle(), &currentWeapon, false, 0, false))
			{
				// Lasso weapon hash
				if (currentWeapon == "WEAPON_LASSO"_J)
				{
					// Get right hand bone for particle attachment
					int boneIndex = PED::GET_PED_BONE_INDEX(ped.GetHandle(), 0x6F06); // SKEL_R_Hand

					// Request particle effect asset
					STREAMING::REQUEST_NAMED_PTFX_ASSET("core"_J);
					if (STREAMING::HAS_NAMED_PTFX_ASSET_LOADED("core"_J))
					{
						GRAPHICS::USE_PARTICLE_FX_ASSET("core");

						// Create fire effect on lasso hand
						GRAPHICS::_START_PARTICLE_FX_NON_LOOPED_ON_PED_BONE_2(
							"proj_flare_fuse", // Fire trail effect
							ped.GetHandle(),
							0.0f, 0.0f, 0.0f,  // Offset
							0.0f, 0.0f, 0.0f,  // Rotation
							boneIndex,
							0.3f,              // Scale
							false, false, false
						);
					}
				}
			}
		}
	};

	static FlamingLasso _FlamingLasso{"flaminglasso", "Flaming Lasso", "Your lasso burns with fire!"};
}
