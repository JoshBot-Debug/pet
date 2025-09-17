#pragma once

#include <chrono>

#include "Animate/Animate.h"
#include "Type.h"
#include "Window/Window.h"

class Pet {
private:
  std::chrono::_V2::steady_clock::time_point m_LastTime =
      std::chrono::steady_clock::now();

  Animate m_Animate;

public:
  Pet() = default;
  ~Pet() = default;

  int initialize(int argc, char *argv[]);

private:
  void update(Window &w);
};