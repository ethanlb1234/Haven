#pragma once
#include <chrono>
#include <unordered_map>

namespace YimMenu
{
	class ProtectionStats
	{
		struct Stats
		{
			int m_TotalBlocked = 0;
			int m_CrashAttempts = 0;
			int m_KickAttempts = 0;
			int m_EntitySpam = 0;
			int m_EventSpam = 0;
			std::chrono::system_clock::time_point m_LastAttack{};
		};

		Stats m_SessionStats{};
		std::unordered_map<uint64_t, Stats> m_PlayerStats{};

		static ProtectionStats& GetInstance()
		{
			static ProtectionStats instance;
			return instance;
		}

	public:
		static void RecordBlock(uint64_t attackerId, const std::string& type)
		{
			auto& stats = GetInstance().m_PlayerStats[attackerId];
			stats.m_TotalBlocked++;
			stats.m_LastAttack = std::chrono::system_clock::now();

			auto& session = GetInstance().m_SessionStats;
			session.m_TotalBlocked++;
			session.m_LastAttack = std::chrono::system_clock::now();

			if (type.find("crash") != std::string::npos)
			{
				stats.m_CrashAttempts++;
				session.m_CrashAttempts++;
			}
			else if (type.find("kick") != std::string::npos)
			{
				stats.m_KickAttempts++;
				session.m_KickAttempts++;
			}
			else if (type.find("flood") != std::string::npos || type.find("spam") != std::string::npos)
			{
				if (type.find("event") != std::string::npos || type.find("explosion") != std::string::npos || type.find("particle") != std::string::npos)
				{
					stats.m_EventSpam++;
					session.m_EventSpam++;
				}
				else
				{
					stats.m_EntitySpam++;
					session.m_EntitySpam++;
				}
			}
		}

		static Stats GetSessionStats()
		{
			return GetInstance().m_SessionStats;
		}

		static Stats GetPlayerStats(uint64_t playerId)
		{
			return GetInstance().m_PlayerStats[playerId];
		}

		static int GetPlayerTotalBlocked(uint64_t playerId)
		{
			auto it = GetInstance().m_PlayerStats.find(playerId);
			if (it != GetInstance().m_PlayerStats.end())
				return it->second.m_TotalBlocked;
			return 0;
		}

		static void Reset()
		{
			GetInstance().m_SessionStats = Stats{};
			GetInstance().m_PlayerStats.clear();
		}
	};
}
