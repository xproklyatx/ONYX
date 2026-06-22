#pragma once
#include <chrono>
class DeltaTime
{
  public:
    static void Init();
    static void Update();
    static float GetDelta();
    static float GetTime();
    static void PauseNUnpause();
    static bool IsPaused();
    static void SetTimeScale(float scale);
    static float GetTimeScale();

  private:
    DeltaTime() = default;
    static float currentTime;
    static float lastTime;
    static float delta;
    static bool paused;
    static float timeScale;
    using Clock = std::chrono::high_resolution_clock;
    using TimePoint = std::chrono::time_point<Clock>;
    static TimePoint startTime;
    static TimePoint lastFrameTime;
};
