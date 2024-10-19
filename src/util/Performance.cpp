#include "Performance.h"

#include <cstdio>
#include <iostream>
#include <chrono>
#include "util/Logger.h"

namespace scgf::util
{
Performance::Performance()
{
	Reset();
	SCGF_LOG_DEBUG("New perf tester");
}

Performance::~Performance() {}

void Performance::Reset()
{
	m_startTime = Clock::now();
	m_lastPrintTime = m_startTime;
	m_frameCount = 0;
}

void Performance::IncreaseFrameCount() { m_frameCount++; }

void Performance::PrintEverySecond()
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

		SCGF_LOG_DEBUG("Performance: FPS(AVR|RT): {0:.2f}|{1:.2f}", averageFps, realtimeFps);
	}
}
}  // namespace scgf::util
