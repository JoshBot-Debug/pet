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