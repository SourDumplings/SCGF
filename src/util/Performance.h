#pragma once

#include <chrono>

namespace scgf::util
{
class Performance final
{
public:
	Performance();
	~Performance();

	void Reset();
	void IncreaseFrameCount();
	void PrintEverySecond();

private:
	using Clock		= std::chrono::high_resolution_clock;
	using TimePoint = std::chrono::time_point<Clock>;

	TimePoint m_startTime;
	uint64_t m_frameCount = 0;

	TimePoint m_lastPrintTime;
	uint64_t m_lastFrameCount = 0;
};
}  // namespace scgf::util
