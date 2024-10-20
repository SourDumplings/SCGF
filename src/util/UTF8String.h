#pragma once

#include <array>
#include <string>
#include <vector>

namespace scgf::util
{
class UTF8String final
{
public:
	using OneUTF8		= std::array<char8_t, 4>;
	using UTF8Cont		= std::vector<OneUTF8>;
	using Iterator		= UTF8Cont::iterator;
	using ConstIterator = UTF8Cont::const_iterator;

	UTF8String() = default;
	UTF8String(const UTF8String&);
	explicit UTF8String(const std::u8string& s);
	explicit UTF8String(const char8_t* s);

	inline void clear()
	{
		m_data.clear();
		update_cache();
	}
	inline size_t	size() const { return m_data.size(); }
	inline size_t	length() const { return size(); }
	inline OneUTF8* data() { return m_data.data(); }

	Iterator insert(const ConstIterator& it, const OneUTF8& c);
	Iterator insert(const ConstIterator& it, const char8_t* s);
	Iterator insert(const ConstIterator& it, const std::u8string& s);
	Iterator insert(const ConstIterator& it, const UTF8String& s);

	Iterator erase(const ConstIterator& it);
	Iterator erase(const ConstIterator& first, const ConstIterator& last);

	void push_back(OneUTF8 c)
	{
		m_data.push_back(c);
		update_cache();
	}
	/* you must ensure that s contains complete UTF8 codes */
	void push_back(const std::u8string& s);
	void push_back(const char8_t* s);
	void push_back(const UTF8String& s);

	inline void pop_back()
	{
		m_data.pop_back();
		update_cache();
	}
	inline OneUTF8& back() { return m_data.back(); }

	inline const OneUTF8& at(size_t idx) const { return m_data.at(idx); }

	inline auto begin() const { return m_data.begin(); }
	inline auto end() const { return m_data.end(); }

	inline const OneUTF8& operator[](size_t idx) const { return m_data[idx]; }
	inline bool			  empty() const { return m_data.empty(); }

	std::u8string  to_string() const;
	const char8_t* c_str() const;

	inline UTF8String operator+(const UTF8String& s) const
	{
		UTF8String result(*this);
		result.push_back(s);
		return result;
	}
	inline UTF8String operator+(const char8_t* s) const
	{
		UTF8String result(*this);
		result.push_back(s);
		return result;
	}
	inline UTF8String operator+(const std::u8string& s) const
	{
		UTF8String result(*this);
		result.push_back(s);
		return result;
	}
	inline UTF8String& operator+=(const char8_t* s)
	{
		this->push_back(s);
		return *this;
	}
	inline UTF8String& operator+=(const std::u8string& s)
	{
		this->push_back(s);
		return *this;
	}
	inline UTF8String& operator+=(const UTF8String& s)
	{
		this->push_back(s);
		return *this;
	}
	inline UTF8String& operator=(const char8_t* s)
	{
		clear();
		push_back(s);
		return *this;
	}
	inline UTF8String& operator=(const std::u8string& s)
	{
		clear();
		push_back(s);
		return *this;
	}

	friend bool operator==(const UTF8String& lhs, const UTF8String& rhs);
	friend bool operator!=(const UTF8String& lhs, const UTF8String& rhs);

private:
	UTF8Cont			  m_data;
	mutable std::u8string m_cache;
	mutable bool		  m_isCacheValid;

	void update_cache() const
	{
		m_cache		   = to_string();
		m_isCacheValid = true;
	}
};

bool operator==(const scgf::util::UTF8String& lhs, const scgf::util::UTF8String& rhs);
bool operator!=(const scgf::util::UTF8String& lhs, const scgf::util::UTF8String& rhs);
}  // namespace scgf::util

std::u8string UTF8ToString(const scgf::util::UTF8String::OneUTF8& c);
std::ostream& operator<<(std::ostream& o, const scgf::util::UTF8String::OneUTF8& c);
std::ostream& operator<<(std::ostream& o, const scgf::util::UTF8String& s);
