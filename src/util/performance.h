#pragma once

#include <chrono>

namespace scgf::util
{
class CPerformance final
{
public:
	CPerformance();
	~CPerformance();

	void reset();
	void increase_frame_count();
	void print_every_second();

private:
	using Clock		= std::chrono::high_resolution_clock;
	using TimePoint = std::chrono::time_point<Clock>;

	TimePoint m_startTime;
	uint64_t m_frameCount = 0;

	TimePoint m_lastPrintTime;
	uint64_t m_lastFrameCount = 0;
};
}  // namespace scgf::util
