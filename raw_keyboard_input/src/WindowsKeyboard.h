#pragma once
#include "Keyboard.h"

#include <Windows.h>

class WindowsKeyboard : public Keyboard {
public:
  explicit WindowsKeyboard();
  ~WindowsKeyboard() override;
  
private:
	static LRESULT CALLBACK keyHandler(int keyCode, WPARAM w, LPARAM l);
  HHOOK hook;
  
};
