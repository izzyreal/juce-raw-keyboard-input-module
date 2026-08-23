#if defined(__ANDROID__)
#define JUCE_CORE_INCLUDE_JNI_HELPERS 1
#endif

#include "src/Keyboard.cpp"
#include "src/KeyboardFactory.cpp"

#if defined (_WIN32)
#include "src/WindowsKeyboard.cpp"
#elif defined(__ANDROID__)
#include "src/AndroidKeyboard.cpp"
#elif defined(__linux__) && !defined(__ANDROID__)
#include "src/LinuxKeyboard.cpp"
#endif
