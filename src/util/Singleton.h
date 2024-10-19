#pragma once

namespace scgf::util
{
template <typename C>
class Singleton
{
public:
	static C& get_instance()
	{
		static C instance;
		return instance;
	}

	virtual ~Singleton() = default;

protected:
	Singleton() = default;

private:
	C& operator=(const C&) = delete;
};
}  // namespace scgf::util
