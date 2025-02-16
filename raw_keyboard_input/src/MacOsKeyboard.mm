#include "MacOsKeyboard.h"
#include <Foundation/Foundation.h>
#include <AppKit/AppKit.h>

MacOsKeyboard::MacOsKeyboard(juce::Component* parent, const bool shouldSynthesizeSomeKeyRepeatsToUse)
: Keyboard(parent), shouldSynthesizeSomeKeyRepeats(shouldSynthesizeSomeKeyRepeatsToUse)
{
    if (shouldSynthesizeSomeKeyRepeats)
    {
        CFNumberRef keyRepeatIntervalPreference = (CFNumberRef)CFPreferencesCopyAppValue(CFSTR("KeyRepeat"), CFSTR(".GlobalPreferences"));
        if (keyRepeatIntervalPreference) {
            CFNumberGetValue(keyRepeatIntervalPreference, kCFNumberIntType, &repeatIntervalMs);
            CFRelease(keyRepeatIntervalPreference);
        }
        
        CFNumberRef repeatDelayPreference = (CFNumberRef)CFPreferencesCopyAppValue(CFSTR("InitialKeyRepeat"), CFSTR(".GlobalPreferences"));
        if (repeatDelayPreference) {
            int initialKeyRepeat;
            CFNumberGetValue(repeatDelayPreference, kCFNumberIntType, &initialKeyRepeat);
            repeatSynthesisDelayMs = (initialKeyRepeat * 1000) / 60;
            CFRelease(repeatDelayPreference);
        }
    }
  installMonitor();
}

MacOsKeyboard::~MacOsKeyboard()
{
  if (isTimerRunning()) stopTimer();
  removeMonitor();
}

void MacOsKeyboard::timerCallback()
{
    for (auto &keyCodeAndPressDuration : currentlyRepeatingKeyCodes)
    {
        if (keyCodeAndPressDuration.second >= repeatSynthesisDelayMs)
        {
            Keyboard::processKeyEvent(keyCodeAndPressDuration.first, true);
        }
        else
        {
            keyCodeAndPressDuration.second += getTimerInterval();
        }
    }
}

void MacOsKeyboard::installMonitor()
{
  if (thisses.size() > 1)
    return;
  
  keyDownMonitor = [NSEvent addLocalMonitorForEventsMatchingMask:NSEventMaskKeyDown
                                                         handler:^NSEvent*(NSEvent* event) {
      if (shouldSynthesizeSomeKeyRepeats && std::find(repeatSynthesisKeyCodes.begin(),
                    repeatSynthesisKeyCodes.end(),
                    [event keyCode]) != repeatSynthesisKeyCodes.end())
      {
          if ([event isARepeat])
          {
              // Uh oh, we're getting unexpected true repeats from the monitor anyway.
              // Let's stop the timer to prevent more trouble.
              stopTimer();
          }
          else
          {
              currentlyRepeatingKeyCodes[([event keyCode])] = 0;
              
              if (!isTimerRunning())
              {
                  startTimer(repeatIntervalMs);
              }
          }
      }
      
    Keyboard::processKeyEvent([event keyCode], true);
    return event;
  }];
  
  keyUpMonitor = [NSEvent addLocalMonitorForEventsMatchingMask:NSEventMaskKeyUp
                                                       handler:^NSEvent*(NSEvent* event) {
      if (shouldSynthesizeSomeKeyRepeats && std::find(repeatSynthesisKeyCodes.begin(),
                    repeatSynthesisKeyCodes.end(),
                    [event keyCode]) != repeatSynthesisKeyCodes.end())
      {
          currentlyRepeatingKeyCodes.erase([event keyCode]);
          
          if (currentlyRepeatingKeyCodes.empty())
          {
              stopTimer();
          }
      }
      
      Keyboard::processKeyEvent([event keyCode], false);
    return event;
  }];
  
  modifierChangedMonitor = [NSEvent addLocalMonitorForEventsMatchingMask:NSEventMaskFlagsChanged
                                                                 handler:^NSEvent*(NSEvent* event) {
    auto code = [event keyCode];
    
    if (code == 0x3A || code == 0x3D) {
      if ([event modifierFlags] & NSEventModifierFlagOption) {
        Keyboard::processKeyEvent(code, true);
      } else {
        Keyboard::processKeyEvent(code, false);
      }
    }
    else if (code == 0x38 || code == 0x3C) {
      if ([event modifierFlags] & NSEventModifierFlagShift) {
        Keyboard::processKeyEvent(code, true);
      } else {
        Keyboard::processKeyEvent(code, false);
      }
    }
    else if (code == 0x3B || code == 0x3E) {
      if ([event modifierFlags] & NSEventModifierFlagControl) {
        Keyboard::processKeyEvent(code, true);
      } else {
        Keyboard::processKeyEvent(code, false);
      }
    }
    else if (code == 0x36 || code == 0x37) {
      if ([event modifierFlags] & NSEventModifierFlagCommand) {
        Keyboard::processKeyEvent(code, true);
      } else {
        Keyboard::processKeyEvent(code, false);
      }
    }
    else if (code == 0x3F) {
      if ([event modifierFlags] & NSEventModifierFlagFunction) {
        Keyboard::processKeyEvent(code, true);
      } else {
        Keyboard::processKeyEvent(code, false);
      }
    }
    else if (code == 0x39) {
      if ([event modifierFlags] & NSEventModifierFlagCapsLock) {
        Keyboard::processKeyEvent(code, true);
      } else {
        Keyboard::processKeyEvent(code, false);
      }
    }
    
    return event;
  }];
  
}

void MacOsKeyboard::removeMonitor()
{
  if (keyDownMonitor != nullptr) {
    [NSEvent removeMonitor:(id)keyDownMonitor];
    keyDownMonitor = nullptr;
  }
  if (keyUpMonitor != nullptr) {
    [NSEvent removeMonitor:(id)keyUpMonitor];
    keyUpMonitor =  nullptr;
  }
  if (modifierChangedMonitor != nullptr) {
    [NSEvent removeMonitor:(id)modifierChangedMonitor];
    modifierChangedMonitor = nullptr;
  }
}
