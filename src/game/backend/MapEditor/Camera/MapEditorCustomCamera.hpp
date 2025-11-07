#pragma once
#include "MapEditorCamera.hpp"

namespace YimMenu
{
	class MapEditorCustomCamera final : public MapEditorCamera
	{
	public:
		enum class CameraMode
		{
			FREE,      // Free-flying camera (original mode)
			TOP_DOWN   // Sims-style top-down view for building
		};

		Cam m_Camera;
		rage::fvector3 m_Position;
		rage::fvector3 m_Rotation;
		float m_Speed;
		float m_PanAcceleration;
		float m_ZoomAcceleration;
		rage::fvector3 m_OldPlayerPos;
		CameraMode m_CameraMode = CameraMode::TOP_DOWN; // Default to top-down for easier building

		virtual void Setup() override;
		virtual void Destroy() override;
		virtual void Update(MapControls& controls) override;
		virtual bool IsPlayerMode() override;
		virtual std::optional<RaycastResult> Raycast(Entity ignoreEntity) override;

		// Toggle between camera modes
		void ToggleCameraMode();
		CameraMode GetCameraMode() const { return m_CameraMode; }
		void SetCameraMode(CameraMode mode) { m_CameraMode = mode; }
	};
}