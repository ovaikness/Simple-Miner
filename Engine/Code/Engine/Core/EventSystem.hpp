#pragma once
#include "Engine/Core/StringUtils.hpp"

#include <vector>
#include <mutex>
#include <map>

class NamedProperties;
typedef NamedProperties EventArgs;
typedef bool(*EventCallbackFunction)(EventArgs& args);
typedef std::vector<EventCallbackFunction> SubscriptionList;

struct EventSystemConfig
{
};

class EventSystem
{
public:
	EventSystem(EventSystemConfig const& config);
	~EventSystem();
	
	void Startup();
	void Shutdown();
	void BeginFrame();
	void EndFrame();

	void SubscribeEventCallbackFunction(std::string const& eventName,   EventCallbackFunction functionCallback);
	void UnsubscribeEventCallbackFunction(std::string const& eventName, EventCallbackFunction functionCallback);

	Strings GetEventNames() const;

	bool FireEvent(std::string const& eventName, EventArgs& args);
	bool FireEvent(std::string const& eventName);

	bool HasEvent(std::string const& eventName);
protected:
	EventSystemConfig m_config;
	mutable std::recursive_mutex m_mutex;
	std::map< std::string, SubscriptionList > m_subscriptionListsByEventName;
};