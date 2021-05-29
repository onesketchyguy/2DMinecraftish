#pragma once

#ifndef NETWORKCOMMON_H
#define NETWORKCOMMON_H

#include "olcPGEX_Network.h"
#include "ConstantData.h"

enum class GameMsg : uint32_t
{
	Server_GetStatus,
	Server_GetPing,

	Client_Accepted,
	Client_AssignID,
	Client_RegisterWithServer,
	Client_UnregisterWithServer,

	Game_AddPlayer,
	Game_RemovePlayer,
	Game_UpdatePlayer,
};

enum class Tools : uint32_t
{
	None,
	Shovel,
	Pickaxe,
	Axe,
	Hoe
};

struct PlayerDescription
{
	uint32_t uniqueID = 0;
	uint32_t avatarIndex_x = 0;
	uint32_t avatarIndex_y = 0;
	bool flip_x = false;

	uint32_t health = 100;

	float radius = 0.5f;

	olc::vf2d position;
	olc::vf2d velocity;
};

class PlayerObject
{
public:
	PlayerDescription* desc;
	Tools currentTool = Tools::None;

	const byte WATER_INDEX = 3;
	olc::vi2d cellIndex = { 0, 0 };

	LOOK_DIR lookDir = LOOK_DIR::down;

	bool inWater = false;

	void Update(float fElapsedTime)
	{
		if (desc->velocity.x > 0)
		{
			// Should face right
			lookDir = LOOK_DIR::right;
		}
		else if (desc->velocity.x < 0)
		{
			// Should face left
			lookDir = LOOK_DIR::left;
		}

		if (desc->velocity.y > 0)
		{
			// Should face down
			lookDir = LOOK_DIR::down;
		}
		else if (desc->velocity.y < 0)
		{
			// Should face up
			lookDir = LOOK_DIR::up;
		}

		int waterIndex = inWater ? WATER_INDEX : 0;

		switch (lookDir)
		{
		case LOOK_DIR::down:
			cellIndex = { waterIndex + 2, 0 };
			break;
		case LOOK_DIR::right:
			cellIndex = { waterIndex + 0, 0 };
			break;
		case LOOK_DIR::up:
			cellIndex = { waterIndex + 1, 0 };
			break;
		case LOOK_DIR::left:
			cellIndex = { waterIndex + 0, 0 };
			break;
		default:
			break;
		}

		desc->avatarIndex_x = cellIndex.x;
		desc->avatarIndex_y = cellIndex.y;

		desc->flip_x = lookDir == LOOK_DIR::left;
	}

public:
	PlayerObject(PlayerDescription* desc)
	{
		this->desc = desc;
	}
};

class GameServer : public olc::net::server_interface<GameMsg>
{
public:
	GameServer(uint16_t nPort) : olc::net::server_interface<GameMsg>(nPort)
	{
	}

	std::unordered_map<uint32_t, PlayerDescription> m_mapPlayerRoster;
	std::vector<uint32_t> m_vGarbageIDs;

protected:
	bool OnClientConnect(std::shared_ptr<olc::net::connection<GameMsg>> client) override
	{
		// For now we will allow all
		return true;
	}

	void OnClientValidated(std::shared_ptr<olc::net::connection<GameMsg>> client) override
	{
		// Client passed validation check, so send them a message informing
		// them they can continue to communicate
		olc::net::message<GameMsg> msg;
		msg.header.id = GameMsg::Client_Accepted;
		client->Send(msg);
	}

	void OnClientDisconnect(std::shared_ptr<olc::net::connection<GameMsg>> client) override
	{
		if (client)
		{
			if (m_mapPlayerRoster.find(client->GetID()) == m_mapPlayerRoster.end())
			{
				// client never added to roster, so just let it disappear
			}
			else
			{
				auto& pd = m_mapPlayerRoster[client->GetID()];
				std::cout << "[UNGRACEFUL REMOVAL]:" + std::to_string(pd.uniqueID) + "\n";
				m_mapPlayerRoster.erase(client->GetID());
				m_vGarbageIDs.push_back(client->GetID());
			}
		}
	}

	void OnMessage(std::shared_ptr<olc::net::connection<GameMsg>> client, olc::net::message<GameMsg>& msg) override
	{
		if (!m_vGarbageIDs.empty())
		{
			for (auto pid : m_vGarbageIDs)
			{
				olc::net::message<GameMsg> m;
				m.header.id = GameMsg::Game_RemovePlayer;
				m << pid;
				std::cout << "Removing " << pid << "\n";
				MessageAllClients(m);
			}
			m_vGarbageIDs.clear();
		}

		switch (msg.header.id)
		{
		case GameMsg::Client_RegisterWithServer:
		{
			PlayerDescription desc;
			msg >> desc;
			desc.uniqueID = client->GetID();
			m_mapPlayerRoster.insert_or_assign(desc.uniqueID, desc);

			olc::net::message<GameMsg> msgSendID;
			msgSendID.header.id = GameMsg::Client_AssignID;
			msgSendID << desc.uniqueID;
			MessageClient(client, msgSendID);

			olc::net::message<GameMsg> msgAddPlayer;
			msgAddPlayer.header.id = GameMsg::Game_AddPlayer;
			msgAddPlayer << desc;
			MessageAllClients(msgAddPlayer);

			for (const auto& player : m_mapPlayerRoster)
			{
				olc::net::message<GameMsg> msgAddOtherPlayers;
				msgAddOtherPlayers.header.id = GameMsg::Game_AddPlayer;
				msgAddOtherPlayers << player.second;
				MessageClient(client, msgAddOtherPlayers);
			}

			break;
		}

		case GameMsg::Client_UnregisterWithServer:
		{
			break;
		}

		case GameMsg::Game_UpdatePlayer:
		{
			// Simply bounce update to everyone except incoming client
			MessageAllClients(msg, client);
			break;
		}
		}
	}
};

class Client : public olc::net::client_interface<GameMsg>
{
};

#endif // !NETWORKCOMMON_H