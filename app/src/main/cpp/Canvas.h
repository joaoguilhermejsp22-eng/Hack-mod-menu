#pragma once
#include <cstdint>
namespace Canvas {
    void Init();
    void DrawBox(float x, float y, float w, float h, float th, uint32_t argb);
    void DrawLine(float x1, float y1, float x2, float y2, float th, uint32_t argb);
    void DrawText(float x, float y, const char* t, uint32_t argb, float sz);
}
