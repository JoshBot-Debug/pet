#pragma once

#include <stdint.h>
#include <vector>

struct Frame {
  std::vector<uint8_t> pixels = {};
  uint32_t width = 0, height = 0, delay = 0;
};

struct AABB {
  int x, y, w, h;

  bool intersects(const AABB &aabb) const {
    return x < aabb.x + aabb.w && x + w > aabb.x && y < aabb.y + aabb.h &&
           y + h > aabb.y;
  }
};

enum class Animation : uint32_t { UNSET = 0, IDLE, WALK, RUN, JUMP, SIT, EXTRA };

enum class State : uint32_t {
  IDLING = 0,
  WALKING,
  RUNNING,
  SITTING,
  FALLING,
  JUMPING
};