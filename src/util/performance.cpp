#include "performance.h"

#include <cstdio>
#include <iostream>
#include <chrono>
#include "util/logger.h"

namespace scgf::util
{
CPerformance::CPerformance()
{
	reset();
	SCGF_LOG_DEBUG("New perf tester");
}

CPerformance::~CPerformance() {}

void CPerformance::reset()
{
	m_startTime = Clock::now();
	m_lastPrintTime = m_startTime;
	m_frameCount = 0;
}

void CPerformance::increase_frame_count() { m_frameCount++; }

void CPerformance::print_every_second()
{
	auto now = Clock::now();
	auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(now - m_lastPrintTime);
	if (duration.count() >= 1000)
	{
		double elapsedSeconds =
			std::chrono::duration_cast<std::chrono::milliseconds>(now - m_startTime).count() /
			1000.0;
		double averageFps = m_frameCount / elapsedSeconds;
		double realtimeFps = (m_frameCount - m_lastFrameCount) / (duration.count() / 1000.0);
		m_lastPrintTime = now;
		m_lastFrameCount = m_frameCount;

		SCGF_LOG_DEBUG("CPerformance: FPS(AVR|RT): {0:.2f}|{1:.2f}", averageFps, realtimeFps);
	}
}
}  // namespace scgf::util
