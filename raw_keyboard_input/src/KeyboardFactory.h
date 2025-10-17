#pragma once

class Keyboard;
class KeyboardFactory {
public:
  static Keyboard* instance(const bool shouldSynthesizeSomeKeyRepeats = false);
};
