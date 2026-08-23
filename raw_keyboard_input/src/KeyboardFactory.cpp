#include "KeyboardFactory.h"
#include "Keyboard.h"
#if defined (__APPLE__)
#include <TargetConditionals.h>
#if !TARGET_OS_IPHONE
#include "MacOsKeyboard.h"
#endif
#elif defined (_WIN32)
#include "WindowsKeyboard.h"
#elif defined (__ANDROID__)
#include "AndroidKeyboard.h"
#elif defined (__linux__) && !defined (__ANDROID__)
#include "LinuxKeyboard.h"
#endif

Keyboard* KeyboardFactory::instance(
    juce::Component* parent, const bool shouldSynthesizeSomeKeyRepeats)
{
  static_cast<void>(parent);
  static_cast<void>(shouldSynthesizeSomeKeyRepeats);

#if defined (__APPLE__)
#include <TargetConditionals.h>
#if TARGET_OS_IPHONE
  return new Keyboard();
#else
  return new MacOsKeyboard(shouldSynthesizeSomeKeyRepeats);
#endif
#elif defined (_WIN32)
  return new WindowsKeyboard();
#elif defined (__ANDROID__)
  return new AndroidKeyboard(parent);
#elif defined (__linux__) && !defined (__ANDROID__)
  return new LinuxKeyboard();
#else
  return new Keyboard();
#endif
}
