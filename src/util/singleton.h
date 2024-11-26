#pragma once

namespace scgf::util
{
template <typename C>
class CSingleton
{
public:
	static C& get_instance()
	{
		static C s_instance;
		return s_instance;
	}

	virtual ~CSingleton() = default;

protected:
	CSingleton() = default;

private:
	C& operator=(const C&) = delete;
};
}  // namespace scgf::util
