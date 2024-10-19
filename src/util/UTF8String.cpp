#include "UTF8String.h"

#include <iostream>

namespace scgf::util
{
UTF8String::UTF8String(const UTF8String& s)
	: m_isCacheValid(false)
{
	push_back(s);
	update_cache();
}

UTF8String::UTF8String(const char8_t* s)
{
	push_back(s);
	update_cache();
}

UTF8String::UTF8String(const std::u8string& s)
{
	push_back(s);
	update_cache();
}

UTF8String::Iterator UTF8String::insert(const UTF8String::ConstIterator& it, const OneUTF8& c)
{
	Iterator ret = m_data.insert(it, c);
	update_cache();
	return ret;
}

UTF8String::Iterator UTF8String::insert(const UTF8String::ConstIterator& it, const char8_t* s)
{
	UTF8String str(s);
	Iterator   ret = m_data.insert(it, str.m_data.begin(), str.m_data.end());
	update_cache();
	return ret;
}

UTF8String::Iterator UTF8String::insert(const UTF8String::ConstIterator& it, const std::u8string& s)
{
	UTF8String str(s);
	Iterator   ret = m_data.insert(it, str.m_data.begin(), str.m_data.end());
	update_cache();
	return ret;
}

UTF8String::Iterator UTF8String::insert(const UTF8String::ConstIterator& it, const UTF8String& s)
{
	Iterator ret = m_data.insert(it, s.begin(), s.end());
	update_cache();
	return ret;
}

UTF8String::Iterator UTF8String::erase(const UTF8String::ConstIterator& it)
{
	Iterator ret = m_data.erase(it);
	update_cache();
	return ret;
}

UTF8String::Iterator
	UTF8String::erase(const UTF8String::ConstIterator& first, const UTF8String::ConstIterator& last)
{
	Iterator ret = m_data.erase(first, last);
	update_cache();
	return ret;
}

void UTF8String::push_back(const std::u8string& s)
{
	push_back(s.c_str());
	update_cache();
}

void UTF8String::push_back(const UTF8String& s)
{
	for (const auto& c : s)
		m_data.push_back(c);
	update_cache();
}

void UTF8String::push_back(const char8_t* s)
{
	if (!s || std::strlen(reinterpret_cast<const char*>(s)) == 0) return;

	int idx = 0;
	int len = std::strlen(reinterpret_cast<const char*>(s));
	while (idx < len)
	{
		OneUTF8 c;
		c.fill(0);
		if ((s[idx] & 0xF0) == 0xF0)
		{
			c[0] = s[idx++];
			c[1] = s[idx++];
			c[2] = s[idx++];
			c[3] = s[idx++];
			m_data.push_back(c);
		}
		else if ((s[idx] & 0xE0) == 0xE0)
		{
			c[0] = s[idx++];
			c[1] = s[idx++];
			c[2] = s[idx++];
			m_data.push_back(c);
		}
		else if ((s[idx] & 0xC0) == 0xC0)
		{
			c[0] = s[idx++];
			c[1] = s[idx++];
			m_data.push_back(c);
		}
		else if (s[idx] >= 0)
		{
			c[0] = s[idx++];
			m_data.push_back(c);
		}
		else { idx++; }
	}
	update_cache();
}

std::u8string UTF8String::to_string() const
{
	std::u8string result;
	for (auto& c : m_data)
	{
		if ((c[0] & 0xF0) == 0xF0)
		{
			result.push_back(c[0]);
			result.push_back(c[1]);
			result.push_back(c[2]);
			result.push_back(c[3]);
		}
		else if ((c[0] & 0xE0) == 0xE0)
		{
			result.push_back(c[0]);
			result.push_back(c[1]);
			result.push_back(c[2]);
		}
		else if ((c[0] & 0xC0) == 0xC0)
		{
			result.push_back(c[0]);
			result.push_back(c[1]);
		}
		else if (c[0] >= 0) { result.push_back(c[0]); }
	}
	return result;
}

const char8_t* UTF8String::c_str() const
{
	if (!m_isCacheValid) { update_cache(); }
	return m_cache.c_str();
}

std::u8string UTF8ToString(const UTF8String::OneUTF8& c)
{
	std::u8string result;
	if ((c[0] & 0xF0) == 0xF0)
	{
		result.push_back(c[0]);
		result.push_back(c[1]);
		result.push_back(c[2]);
		result.push_back(c[3]);
	}
	else if ((c[0] & 0xE0) == 0xE0)
	{
		result.push_back(c[0]);
		result.push_back(c[1]);
		result.push_back(c[2]);
	}
	else if ((c[0] & 0xC0) == 0xC0)
	{
		result.push_back(c[0]);
		result.push_back(c[1]);
	}
	else if (c[0] >= 0) { result.push_back(c[0]); }
	return result;
}

bool operator==(const scgf::util::UTF8String& lhs, const scgf::util::UTF8String& rhs)
{
	if (lhs.m_data.size() != rhs.m_data.size()) return false;

	for (size_t i = 0; i < lhs.m_data.size(); ++i)
	{
		for (size_t j = 0; j < 4; ++j)
		{
			if (lhs.m_data[i][j] != rhs.m_data[i][j]) return false;
		}
	}
	return true;
}

bool operator!=(const scgf::util::UTF8String& lhs, const scgf::util::UTF8String& rhs)
{
	return !(lhs == rhs);
}
}  // namespace scgf::util

std::ostream& operator<<(std::ostream& o, const scgf::util::UTF8String::OneUTF8& c)
{
	for (auto ch : c)
	{
		if (ch != u8'\0') { o << std::string(1, static_cast<char>(ch)); }
	}
	return o;
}

std::ostream& operator<<(std::ostream& o, const scgf::util::UTF8String& s)
{
	for (const auto& c : s)
		o << c;
	return o;
}