#include "Engine/Core/HCIString.hpp"

HCIString::HCIString(char const* cString)
	: m_CasedString(cString)
{
	Rehash();
}

HCIString::HCIString(std::string const& string)
	: m_CasedString(string)
{
	m_CasedString = string;
	Rehash();
}

HCIString::HCIString(std::string&& string)
	: m_CasedString(std::move(string))
{
	Rehash();
}

HCIString::HCIString(HCIString const& other)
	: m_CasedString(other.m_CasedString)
	, m_Hash(other.m_Hash)
{
}
HCIString::HCIString(HCIString&& other) noexcept
	: m_CasedString(other.m_CasedString)
	, m_Hash(other.m_Hash)
{
}

HCIString& HCIString::operator=(std::string const& string)
{
	*this = HCIString(string);
	return *this;
}

HCIString& HCIString::operator=(char const* cString)
{
	*this = HCIString(cString);
	return *this;
}

HCIString& HCIString::operator=(std::string&& string) noexcept
{
	*this = HCIString(std::move(string));
	return *this;
}

HCIString& HCIString::operator=(HCIString const& other)
{
	m_Hash = other.m_Hash;
	m_CasedString = other.m_CasedString;
	return *this;
}

HCIString& HCIString::operator=(HCIString&& other) noexcept
{
	m_Hash = other.m_Hash;
	m_CasedString = std::move(other.m_CasedString);
	return *this;
}

bool HCIString::operator==(HCIString const& other) const
{
	if (m_Hash != other.m_Hash)
		return false;

	if (m_CasedString.size() != other.m_CasedString.size())
		return false;

	for (int i = 0; i < m_CasedString.size(); ++i)
	{
		if (std::tolower(m_CasedString[i]) != std::tolower(other.m_CasedString[i]))
			return false;
	}

	return true;
}

bool HCIString::operator!=(HCIString const& other) const
{
	return m_Hash != other.m_Hash;
}

bool HCIString::operator<(HCIString const& other) const
{
	if (m_Hash == other.m_Hash)
	{
		return m_CasedString < other.m_CasedString;
	}

	return m_Hash < other.m_Hash;
}

bool HCIString::operator==(std::string_view other) const
{
	HCIString otherHCIString(other.data());
	return (*this == otherHCIString);
}

bool HCIString::operator!=(std::string_view other) const
{
	HCIString otherHCIString(other.data());
	return (*this != otherHCIString);
}

uint32_t HCIString::GetHash() const
{
	return m_Hash;
}

std::string const& HCIString::GetOriginalString() const
{
	return m_CasedString;
}

void HCIString::Rehash()
{
	m_Hash = 0;
	for (char c : m_CasedString)
	{
		m_Hash *= 31;
		m_Hash += (unsigned int)std::tolower(c);
	}
}