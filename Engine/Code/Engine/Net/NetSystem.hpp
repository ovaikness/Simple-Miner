#pragma once

#include "Engine/Core/StringUtils.hpp"
#include <string>
#include <vector>

enum class NetMode
{
	LOCAL,
	CLIENT,
	SERVER
};

struct NetSystemConfig
{
	std::string m_modeString;
	std::string m_hostAddressString;
	std::string m_hostPortString;

	size_t m_sendBufferSize = 1024;
	size_t m_receiveBufferSize = 1024;
};

class NetSystem
{
public:
	NetSystemConfig m_config;

	bool m_isConnectionEstablished = false;

	uintptr_t m_listenSocket = ~0ull; // INVALID_SOCKET
	uintptr_t m_clientSocket = ~0ull; // INVALID_SOCKET
	unsigned long m_hostAddress = 0u;
	unsigned short m_hostPort = 0u;

	std::string m_dataToSend;

	char* m_sendBuffer = nullptr;
	int m_sendBufferByteCount = 0;
	char* m_receiveBuffer = nullptr;

	std::vector<char> m_recieveStringInProcessBuffer;
	std::vector<char> m_sendStringInProcessBuffer;
	NetMode m_mode = NetMode::LOCAL;

public:
	~NetSystem();
	NetSystem(NetSystemConfig const& config);
	void Startup();

	void BeginFrame();
	void EndFrame();
	void Shutdown();

	void ClientStartup();
	void ClientBeginFrame();
	void ClientShutdown();

	void ServerStartup();
	void ServerBeginFrame();
	void ServerShutdown();

	void SendString(std::string const& str);
	void UpdateSendBuffer();
	void UpdateFromRecieveBuffer(size_t bytesRecieved);
};