#ifndef SETTINGS_HPP
#define SETTINGS_HPP

#include <raylib.h>

namespace Settings {

const Color DefaultBackgroundColor = Color { 0xb2, 0xbd, 0x08, 0xff };

// Nokia 3310 screen resolution 320x240
const int NokiaScreenWidth = 320;
const int NokiaScreenHeight = 240;

const int DefaultScreenWidth = 800;
const int DefaultScreenHeight = 600;

const float DefaultCameraZoom = 2.0f;

} // namespace Settings

#endif // SETTINGS_HPP
