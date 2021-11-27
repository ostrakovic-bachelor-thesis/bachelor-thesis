#ifndef FRAME_BUFFER_H
#define FRAME_BUFFER_H

#include <cstdint>

constexpr uint32_t DISPLAY_WIDTH             = 390u;
constexpr uint32_t DISPLAY_HEIGHT            = 390u;
constexpr uint32_t DISPLAY_PIXEL_FORMAT_SIZE = 3u;
constexpr uint32_t FRAME_BUFFER_SIZE = DISPLAY_WIDTH * DISPLAY_HEIGHT * DISPLAY_PIXEL_FORMAT_SIZE;


extern uint8_t g_frameBuffer[FRAME_BUFFER_SIZE];

#endif // #ifndef FRAME_BUFFER_H