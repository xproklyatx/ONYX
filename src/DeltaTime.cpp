#include "DeltaTime.hpp"
float DeltaTime::currentTime = 0.0f;
float DeltaTime::lastTime = 0.0f;
float DeltaTime::delta = 0.0f;
bool DeltaTime::paused = false;
float DeltaTime::timeScale = 1.0f;

DeltaTime::TimePoint DeltaTime::startTime = Clock::now();
DeltaTime::TimePoint DeltaTime::lastFrameTime = Clock::now();

void DeltaTime::Init()
{
    startTime = Clock::now();
    lastFrameTime = startTime;
    currentTime = 0.0f;
    lastTime = 0.0f;
    delta = 0.0f;
    paused = false;
    timeScale = 1.0f;
}

void DeltaTime::Update()
{
    if (paused)
    {
        delta = 0.0f;
        return;
    }

    TimePoint currentFrameTime = Clock::now();

    std::chrono::duration<float> deltaDuration = currentFrameTime - lastFrameTime;
    delta = deltaDuration.count();

    const float MAX_DELTA = 0.1f;
    if (delta > MAX_DELTA)
    {
        delta = MAX_DELTA;
    }

    delta *= timeScale;

    currentTime += delta;
    lastTime = currentTime;

    lastFrameTime = currentFrameTime;
}

float DeltaTime::GetDelta()
{
    return delta;
}

float DeltaTime::GetTime()
{
    return currentTime;
}

void DeltaTime::PauseNUnpause()
{
    paused = !paused;

    if (!paused)
    {

        lastFrameTime = Clock::now();
    }
}

bool DeltaTime::IsPaused()
{
    return paused;
}

void DeltaTime::SetTimeScale(float scale)
{
    timeScale = scale;
}

float DeltaTime::GetTimeScale()
{
    return timeScale;
}
