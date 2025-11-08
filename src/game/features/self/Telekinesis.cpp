#include "core/commands/BoolCommand.hpp"
#include "core/commands/LoopedCommand.hpp"
#include "game/backend/Self.hpp"
#include "game/frontend/GUI.hpp"
#include "game/rdr/Enums.hpp"
#include "game/rdr/Natives.hpp"

namespace YimMenu::Features
{
	class Telekinesis : public LoopedCommand
	{
		using LoopedCommand::LoopedCommand;
		static inline Object heldObject = 0;
		static inline rage::fvector3 objectOffset = {0.0f, 3.0f, 1.0f};

		virtual void OnTick() override
		{
			auto player = Self::GetPed();
			if (!player.IsValid())
				return;

			if (GUI::IsOpen())
				return;

			// Hold E to grab nearest object
			if (PAD::IS_CONTROL_PRESSED(0, (int)NativeInputs::INPUT_PICKUP))
			{
				if (!heldObject || !ENTITY::DOES_ENTITY_EXIST(heldObject))
				{
					// Find nearest object
					auto playerPos = player.GetPosition();
					heldObject = OBJECT::GET_CLOSEST_OBJECT_OF_TYPE(
						playerPos.x, playerPos.y, playerPos.z,
						10.0f, // 10m radius
						0, // Any type
						false, false, true
					);

					if (heldObject && ENTITY::DOES_ENTITY_EXIST(heldObject))
					{
						// Disable collision
						ENTITY::SET_ENTITY_COLLISION(heldObject, false, false);

						// Remove physics
						ENTITY::FREEZE_ENTITY_POSITION(heldObject, true);
					}
				}
			}
			else if (heldObject && ENTITY::DOES_ENTITY_EXIST(heldObject))
			{
				// Release object
				ENTITY::SET_ENTITY_COLLISION(heldObject, true, true);
				ENTITY::FREEZE_ENTITY_POSITION(heldObject, false);

				// Give it a push forward
				auto playerRot = ENTITY::GET_ENTITY_ROTATION(player.GetHandle(), 2);
				float heading = playerRot.z * (3.14159f / 180.0f);

				ENTITY::SET_ENTITY_VELOCITY(heldObject,
					cos(heading) * 10.0f,
					sin(heading) * 10.0f,
					5.0f
				);

				heldObject = 0;
			}

			// Move held object
			if (heldObject && ENTITY::DOES_ENTITY_EXIST(heldObject))
			{
				auto playerPos = player.GetPosition();
				auto playerRot = ENTITY::GET_ENTITY_ROTATION(player.GetHandle(), 2);
				float heading = playerRot.z * (3.14159f / 180.0f);

				// Position in front of player
				rage::fvector3 targetPos;
				targetPos.x = playerPos.x + cos(heading) * objectOffset.y;
				targetPos.y = playerPos.y + sin(heading) * objectOffset.y;
				targetPos.z = playerPos.z + objectOffset.z;

				ENTITY::SET_ENTITY_COORDS(heldObject, targetPos.x, targetPos.y, targetPos.z, false, false, false, false);

				// Rotate with mouse wheel
				if (PAD::IS_CONTROL_PRESSED(0, (int)NativeInputs::INPUT_COVER))
				{
					auto objRot = ENTITY::GET_ENTITY_ROTATION(heldObject, 2);
					objRot.z += 5.0f;
					ENTITY::SET_ENTITY_ROTATION(heldObject, objRot.x, objRot.y, objRot.z, 2, true);
				}

				// Move closer/farther
				if (PAD::IS_CONTROL_PRESSED(0, (int)NativeInputs::INPUT_AIM))
				{
					objectOffset.y -= 0.1f;
					if (objectOffset.y < 1.0f) objectOffset.y = 1.0f;
				}
				if (PAD::IS_CONTROL_PRESSED(0, (int)NativeInputs::INPUT_DUCK))
				{
					objectOffset.y += 0.1f;
					if (objectOffset.y > 10.0f) objectOffset.y = 10.0f;
				}
			}
		}

		virtual void OnDisable() override
		{
			if (heldObject && ENTITY::DOES_ENTITY_EXIST(heldObject))
			{
				ENTITY::SET_ENTITY_COLLISION(heldObject, true, true);
				ENTITY::FREEZE_ENTITY_POSITION(heldObject, false);
				heldObject = 0;
			}
		}
	};

	static Telekinesis _Telekinesis{"telekinesis", "Telekinesis", "Move objects with your mind! Hold E to grab, release to throw!"};
}
