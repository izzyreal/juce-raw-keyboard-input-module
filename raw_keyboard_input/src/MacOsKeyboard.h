#pragma once
#include "Keyboard.h"

namespace juce { class Component; }

class MacOsKeyboard : public Keyboard, juce::Timer {
public:
  explicit MacOsKeyboard(juce::Component* parent, const bool shouldSynthesizeSomeKeyRepeats = false);
    ~MacOsKeyboard() override;
    
    void timerCallback() override;
    
private:
  void installMonitor();
  void removeMonitor();
    /*
     * The _- and += keys for some reason (sandbox?) don't get key repeats in the AUv3,
     * so we synthesize them ourselves.
     */
    const bool shouldSynthesizeSomeKeyRepeats;
    const std::vector<uint8_t> repeatSynthesisKeyCodes { 0x18, 0x1B };
    /*
     * key = key code, value = how it's been pressed.
     * We stop counting how it's been pressed after the initial delay of 400ms.
     */
    std::map<uint8_t, uint16_t> currentlyRepeatingKeyCodes;
  
  void* keyDownMonitor = nullptr;
  void* keyUpMonitor = nullptr;
  void* modifierChangedMonitor = nullptr;
    uint16_t repeatSynthesisDelayMs = 400;
    uint16_t repeatIntervalMs = 200;
};
