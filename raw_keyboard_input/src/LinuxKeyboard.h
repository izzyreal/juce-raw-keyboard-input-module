#pragma once
#include "Keyboard.h"

#include <X11/Xlib.h>

#include <thread>

class LinuxKeyboard : public Keyboard {
public:
  explicit LinuxKeyboard();
  ~LinuxKeyboard() override;

private:
    char prev_keymap[32];
    std::thread* eventLoop;
    Display* display = nullptr;
    bool running = false;
};
