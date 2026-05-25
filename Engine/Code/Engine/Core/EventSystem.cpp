#include "EventSystem.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Core/NamedProperties.hpp"
#include <algorithm>

EventSystem::EventSystem(EventSystemConfig const& config)
	:m_config(config)
{

}

EventSystem::~EventSystem()
{

}

void EventSystem::Startup()
{

}

void EventSystem::Shutdown()
{
}

void EventSystem::BeginFrame()
{

}

void EventSystem::EndFrame()
{

}

void EventSystem::SubscribeEventCallbackFunction(std::string const& eventName, EventCallbackFunction functionCallback)
{
	std::lock_guard<std::recursive_mutex> lock(m_mutex);
	m_subscriptionListsByEventName[eventName].push_back(functionCallback);
}

void EventSystem::UnsubscribeEventCallbackFunction(std::string const& eventName, EventCallbackFunction functionCallback)
{
	std::lock_guard<std::recursive_mutex> lock(m_mutex);
	for(auto itr = m_subscriptionListsByEventName.begin(); itr != m_subscriptionListsByEventName.end(); ++itr)
	{
		std::string const& name = itr->first;
		SubscriptionList& subscribers = itr->second;
		if (name == eventName)
		{
			for (auto subItr = subscribers.begin(); subItr != subscribers.end(); ++subItr)
			{
				if (*subItr == functionCallback)
				{
					subscribers.erase(subItr);
					return;
				}
			}
		}
	}
}

Strings EventSystem::GetEventNames() const
{
	std::lock_guard<std::recursive_mutex> lock(m_mutex);
	Strings eventNames;
	for (std::pair<std::string, SubscriptionList> const& pair : m_subscriptionListsByEventName)
	{
		eventNames.push_back(pair.first);
	}
	return eventNames;
}

bool EventSystem::FireEvent(std::string const& eventName, EventArgs& args)
{
	std::lock_guard<std::recursive_mutex> lock(m_mutex);
	for (auto itr = m_subscriptionListsByEventName.begin(); itr != m_subscriptionListsByEventName.end(); ++itr)
	{
		std::string const& name = itr->first;
		SubscriptionList& subscribers = itr->second;
		std::string lowerName = name;
		std::string eventNameLower = eventName;
		std::transform(eventNameLower.begin(), eventNameLower.end(), eventNameLower.begin(), 
			[](unsigned char c) -> char
			{
				return (char)std::tolower(c);
			}
		);
		std::transform(lowerName.begin(), lowerName.end(), lowerName.begin(), 
			[](unsigned char c) -> char
			{
				return (char)std::tolower(c);
			}
		);
		if (lowerName == eventNameLower)
		{
			for (SubscriptionList::iterator subItr = subscribers.begin(); subItr != subscribers.end(); ++subItr)
			{
				EventCallbackFunction function = *subItr;
				if (function(args))
				{
					return true;
				}
			}
		}
	}
	return false;
}

bool EventSystem::FireEvent(std::string const& eventName)
{
	std::lock_guard<std::recursive_mutex> lock(m_mutex);
	for (auto itr = m_subscriptionListsByEventName.begin(); itr != m_subscriptionListsByEventName.end(); ++itr)
	{
		std::string const& name = itr->first;
		SubscriptionList& subscribers = itr->second;
		if (name == eventName)
		{
			for (SubscriptionList::iterator subItr = subscribers.begin(); subItr != subscribers.end(); ++subItr)
			{
				EventCallbackFunction function = *subItr;
				EventArgs args;
				if (function(args))
				{
					return true;
				}
			}
		}
	}
	return false;
}

bool EventSystem::HasEvent(std::string const& eventName)
{
	std::lock_guard<std::recursive_mutex> lock(m_mutex);
	if (m_subscriptionListsByEventName.find(eventName) != m_subscriptionListsByEventName.end())
	{
		return true;
	}

	for (auto itr = m_subscriptionListsByEventName.begin(); itr != m_subscriptionListsByEventName.end(); ++itr)
	{
		std::string const& name = itr->first;
		std::string eventNameLower = eventName;
		std::string lowerName = name;
		std::transform(eventNameLower.begin(), eventNameLower.end(), eventNameLower.begin(), 
			[](unsigned char c) -> char
			{
				return (char)std::tolower(c);
			}
		);
		std::transform(lowerName.begin(), lowerName.end(), lowerName.begin(), 
			[](unsigned char c) -> char
			{
				return (char)std::tolower(c);
			}
		);
		if (lowerName == eventNameLower)
		{
			return true;
		}
	}

	return false;
}

