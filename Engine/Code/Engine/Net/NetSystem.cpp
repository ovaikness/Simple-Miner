#include "Engine/Net/NetSystem.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"

#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define WIN32_LEAN_AND_MEAN
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <iphlpapi.h>
#include <windows.h>
#pragma comment(lib, "Ws2_32.lib")

#include "Engine/Core/DevConsole.hpp"
#include "Engine/Core/EventSystem.hpp"
#include "Engine/Core/EngineCommon.hpp"

#include <algorithm>
#include <iterator>

NetSystem::~NetSystem()
{
	delete[] m_sendBuffer;
	delete[] m_receiveBuffer;
}

NetSystem::NetSystem(NetSystemConfig const& config)
	: m_config(config)
{
}

void NetSystem::Startup()
{
	m_sendBuffer = new char[m_config.m_sendBufferSize];
	m_receiveBuffer = new char[m_config.m_receiveBufferSize];

	if (m_config.m_modeString == "Local" || m_config.m_modeString == "None")
	{
		m_mode = NetMode::LOCAL;
	}
	else if (m_config.m_modeString == "Client")
	{
		m_mode = NetMode::CLIENT;
	}
	else if (m_config.m_modeString == "Server")
	{
		m_mode = NetMode::SERVER;
	}
	else
	{
		ERROR_AND_DIE("Invalid NetMode");
	}

	int result = 0;
	IN_ADDR addr;
	result = ::inet_pton(AF_INET, m_config.m_hostAddressString.c_str(), &addr);
	if (result != 1)
	{
		ERROR_AND_DIE("Failed to convert host address string");
	}

	m_hostAddress = ntohl(addr.S_un.S_addr);

	m_hostPort = (unsigned short)atoi(m_config.m_hostPortString.c_str());

	if (m_mode == NetMode::CLIENT)
	{
		ClientStartup();
	}
	else if (m_mode == NetMode::SERVER)
	{
		ServerStartup();
	}

	m_sendBuffer = new char[m_config.m_sendBufferSize];
	m_receiveBuffer = new char[m_config.m_receiveBufferSize];
}

void NetSystem::BeginFrame()
{
	if (m_mode == NetMode::CLIENT)
	{
		ClientBeginFrame();
	}
	else if (m_mode == NetMode::SERVER)
	{
		ServerBeginFrame();
	}
}

void NetSystem::Shutdown()
{
	switch (m_mode)
	{
		case NetMode::LOCAL:
		{
			break;
		}
		case NetMode::CLIENT:
		{
			ClientShutdown();
			break;
		}
		case NetMode::SERVER:
		{
			ServerShutdown();
			break;
		}
	}

	delete[] m_sendBuffer;
	delete[] m_receiveBuffer;
	m_sendBuffer = nullptr;
	m_receiveBuffer = nullptr;
}

void NetSystem::EndFrame()
{
	// Clear buffers
	for (size_t i = 0; i < m_config.m_sendBufferSize; i++)
	{
		m_sendBuffer[i] = '\0';
		m_receiveBuffer[i] = '\0';
	}
}

void NetSystem::ClientStartup()
{
	WSADATA data;
	int result = ::WSAStartup(MAKEWORD(2, 2), &data);
	if (result != 0)
	{
		ERROR_AND_DIE("Failed to start WSA");
	}

	m_clientSocket = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (m_clientSocket == INVALID_SOCKET)
	{
		ERROR_AND_DIE("Failed to create client socket");
	}

	unsigned long blockingMode = 1;
	result = ::ioctlsocket(m_clientSocket, FIONBIO, &blockingMode);
	if (result == SOCKET_ERROR)
	{
		ERROR_AND_DIE("Failed to set client socket to non-blocking mode");
	}
}

void NetSystem::ClientBeginFrame()
{
	int result;

	if (!m_isConnectionEstablished)
	{
		sockaddr_in soc_addr;
		soc_addr.sin_family = AF_INET;
		soc_addr.sin_addr.S_un.S_addr = htonl(m_hostAddress);
		soc_addr.sin_port = htons(m_hostPort);

		result = connect(m_clientSocket, (sockaddr*)&soc_addr, (int)sizeof(soc_addr));
		if (result == SOCKET_ERROR)
		{
			int error = WSAGetLastError();
			if (error == WSAEWOULDBLOCK)
			{
				fd_set sockets;
				FD_ZERO(&sockets);
				FD_SET(m_clientSocket, &sockets);

				timeval timeout = { 0, 100 };
				int selectResult = select(0, NULL, &sockets, NULL, &timeout);
				if (selectResult > 0 && FD_ISSET(m_clientSocket, &sockets))
				{
					m_isConnectionEstablished = true;
				}
			}
			else
			{
				// Reset socket if error other than WSAEWOULDBLOCK occurs
				closesocket(m_clientSocket);
				m_clientSocket = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
				unsigned long blockingMode = 1;
				result = ::ioctlsocket(m_clientSocket, FIONBIO, &blockingMode);
				if (result == SOCKET_ERROR)
				{
					ERROR_AND_DIE("Failed to reset client socket");
				}
				return;
			}
		}
		else
		{
			m_isConnectionEstablished = true;
		}
	}

	// Send data if connected
	if (m_isConnectionEstablished)
	{
		UpdateSendBuffer();
		result = send(m_clientSocket, m_sendBuffer, (int)m_sendBufferByteCount, 0);
		if (result == SOCKET_ERROR)
		{
			// Connection lost, reset socket
			m_isConnectionEstablished = false;
			closesocket(m_clientSocket);
			m_clientSocket = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
			unsigned long blockingMode = 1;
			result = ::ioctlsocket(m_clientSocket, FIONBIO, &blockingMode);
			if (result == SOCKET_ERROR)
			{
				ERROR_AND_DIE("Failed to reset client socket");
			}
			memset(m_sendBuffer, 0, m_config.m_sendBufferSize);
			memset(m_receiveBuffer, 0, m_config.m_receiveBufferSize);
			m_sendStringInProcessBuffer.clear();
			m_recieveStringInProcessBuffer.clear();
			m_sendBufferByteCount = 0;
			return;
		}

		// Receive data
		result = recv(m_clientSocket, m_receiveBuffer, (int)m_config.m_receiveBufferSize, 0);
		if (result == SOCKET_ERROR)
		{
			return;
		}

		UpdateFromRecieveBuffer(result);
	}
}

void NetSystem::ClientShutdown()
{
	closesocket(m_clientSocket);
	WSACleanup();
}

void NetSystem::ServerStartup()
{
	WSADATA data;
	int result = ::WSAStartup(MAKEWORD(2, 2), &data);

	if (result != 0)
	{
		ERROR_AND_DIE("Failed to start WSA");
	}

	m_listenSocket = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	if (m_listenSocket == INVALID_SOCKET)
	{
		ERROR_AND_DIE("Failed to create listen socket");
	}

	unsigned long blockingMode = 1;
	result = ::ioctlsocket(m_listenSocket, FIONBIO, &blockingMode);

	sockaddr_in addr;

	addr.sin_family = AF_INET;
	addr.sin_addr.S_un.S_addr = htonl(m_hostAddress);
	addr.sin_port = htons(m_hostPort);

	result = ::bind(m_listenSocket, (sockaddr*)&addr, sizeof(addr));

	if (result == SOCKET_ERROR)
	{
		ERROR_AND_DIE("Failed to bind listen socket");
	}

	result = ::listen(m_listenSocket, SOMAXCONN);

	if (result == SOCKET_ERROR)
	{
		ERROR_AND_DIE("Failed to listen on listen socket");
	}

	m_hostAddress = ntohl(addr.sin_addr.S_un.S_addr);

	m_hostPort = ntohs(addr.sin_port);
}

void NetSystem::ServerBeginFrame()
{
	if (m_listenSocket == INVALID_SOCKET)
	{
		return;
	}

	if (m_clientSocket == INVALID_SOCKET)
	{
		sockaddr_in client_addr;
		client_addr.sin_family = AF_INET;
		client_addr.sin_addr.S_un.S_addr = htonl(m_hostAddress);
		client_addr.sin_port = htons(m_hostPort);

		int client_addr_len = sizeof(client_addr);

		m_clientSocket = ::accept(m_listenSocket, (sockaddr*)&client_addr, &client_addr_len);

		if (m_clientSocket == INVALID_SOCKET)
		{
			return;
		}

		unsigned long blockingMode = 1;
		int result = ::ioctlsocket(m_clientSocket, FIONBIO, &blockingMode);

		if (result == SOCKET_ERROR)
		{
			return;
		}
	}

	UpdateSendBuffer();

	int result = ::send(m_clientSocket, m_sendBuffer, (int)m_sendBufferByteCount, 0);

	if (result == SOCKET_ERROR)
	{
		m_clientSocket = INVALID_SOCKET;
		memset(m_sendBuffer, 0, m_config.m_sendBufferSize);
		memset(m_receiveBuffer, 0, m_config.m_receiveBufferSize);
		m_sendStringInProcessBuffer.clear();
		m_recieveStringInProcessBuffer.clear();
		m_sendBufferByteCount = 0;
		return;
	}

	result = ::recv(m_clientSocket, m_receiveBuffer, (int)m_config.m_receiveBufferSize, 0);

	if (result == SOCKET_ERROR)
	{
		return;
	}

	if (result == 0)
	{
		m_clientSocket = INVALID_SOCKET;
		memset(m_sendBuffer, 0, m_config.m_sendBufferSize);
		memset(m_receiveBuffer, 0, m_config.m_receiveBufferSize);
		m_sendStringInProcessBuffer.clear();
		m_recieveStringInProcessBuffer.clear();
		m_sendBufferByteCount = 0;
		return;
	}

	UpdateFromRecieveBuffer(result);
}

void NetSystem::ServerShutdown()
{
	closesocket(m_clientSocket);
	closesocket(m_listenSocket);
	WSACleanup();
}
void NetSystem::UpdateSendBuffer()
{
	m_sendBufferByteCount = (int)fmin(m_sendStringInProcessBuffer.size(), m_config.m_sendBufferSize);
	std::memcpy(m_sendBuffer, m_sendStringInProcessBuffer.data(), m_sendBufferByteCount);
	m_sendStringInProcessBuffer.erase(m_sendStringInProcessBuffer.begin(), m_sendStringInProcessBuffer.begin() + m_sendBufferByteCount);
}

void NetSystem::UpdateFromRecieveBuffer(size_t bytesReceived)
{
	// Append the received bytes to the processing buffer
	m_recieveStringInProcessBuffer.insert(
		m_recieveStringInProcessBuffer.end(),
		m_receiveBuffer,
		m_receiveBuffer + bytesReceived
	);

	// Process all complete messages in the buffer (split by null terminators)
	for(;;)
	{
		// Find the next null terminator
		auto nullTerminatorPos = std::find(m_recieveStringInProcessBuffer.begin(), m_recieveStringInProcessBuffer.end(), '\0');

		// If no null terminator is found, we're done for now
		if (nullTerminatorPos == m_recieveStringInProcessBuffer.end())
		{
			break;
		}

		// Extract the full message (up to and including the null terminator)
		std::string str(m_recieveStringInProcessBuffer.begin(), nullTerminatorPos);

		// Execute the command or process the string
		g_theConsole->Execute(str);

		// Remove the processed data from the buffer (including the null terminator)
		m_recieveStringInProcessBuffer.erase(m_recieveStringInProcessBuffer.begin(), nullTerminatorPos + 1);
	}
}

void NetSystem::SendString(std::string const& str)
{
	for (char c : str)
	{
		m_sendStringInProcessBuffer.push_back(c);
	}
	m_sendStringInProcessBuffer.push_back('\0');
}
