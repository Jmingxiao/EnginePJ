#pragma once
#include "Mint/Core/TimeStep.h"

namespace Mint{

class PhysicsTimer {
public:

	float GetDeltatime() { return m_deltatime; }
	float Getfactor() { return m_accumulator / m_timestep; }
	float GetAccumulator() { return m_accumulator; }
	bool GetEnoughtime() { return m_accumulator >= m_timestep; }
	void UpdateAccumulator() { m_accumulator -= m_timestep; }
	void Update() 
	{
		if (firsttime) {
			m_previousTime = Timestep::GetTime();
			firsttime = false;
		}
		auto currenttime = Timestep::GetTime();
		m_deltatime = currenttime - m_previousTime;
		m_previousTime = currenttime;
		m_accumulator += m_deltatime;
	}
private:
	bool firsttime=true;
	float m_deltatime{};
	float m_accumulator{};
	float m_previousTime{};
public:
	const float m_timestep = 1.0f / 60.0f;
};

}