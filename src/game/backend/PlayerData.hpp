#pragma once
#include "Detections.hpp"
#include "core/misc/RateLimiter.hpp"
#include "game/rdr/Player.hpp"
#include <unordered_set>
#include <chrono>

namespace YimMenu
{
	class PlayerData
	{
	public:
		std::unordered_set<Detection> m_Detections{};
		Player m_SpectatingPlayer{nullptr};
		bool m_UseSessionSplitKick{};
		bool m_BlockExplosions{};
		bool m_BlockParticles{};
		bool m_GhostMode{};

		RateLimiter m_VehicleFloodLimit{10s, 10};
		RateLimiter m_LargeVehicleFloodLimit{15s, 5};
		RateLimiter m_TickerMessageRateLimit{5s, 3};

		// Enhanced anti-spam rate limiters
		RateLimiter m_ObjectFloodLimit{10s, 15};           // Max 15 objects per 10 seconds
		RateLimiter m_PedFloodLimit{10s, 20};              // Max 20 peds per 10 seconds
		RateLimiter m_AnimalFloodLimit{10s, 15};           // Max 15 animals per 10 seconds
		RateLimiter m_ProjectileFloodLimit{5s, 30};        // Max 30 projectiles per 5 seconds
		RateLimiter m_PickupFloodLimit{10s, 10};           // Max 10 pickups per 10 seconds
		RateLimiter m_ExplosionFloodLimit{5s, 5};          // Max 5 explosions per 5 seconds
		RateLimiter m_ParticleFloodLimit{5s, 10};          // Max 10 particle effects per 5 seconds
		RateLimiter m_NetworkEventFloodLimit{3s, 50};      // Max 50 network events per 3 seconds (general)

		std::optional<std::uint64_t> m_PeerIdToSpoofTo{};
	};
}