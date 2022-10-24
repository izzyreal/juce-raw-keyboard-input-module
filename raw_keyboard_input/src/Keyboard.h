#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

#include <set>
#include <functional>
#include <mutex>

namespace juce { class ComponentPeer; }

class Keyboard : public juce::Timer {

public:
  Keyboard(juce::Component* parent);
  ~Keyboard() override;

  void setAuxParent(juce::Component* auxParent);

  void timerCallback() override;

  juce::ComponentPeer* peer = nullptr;
  juce::ComponentPeer* auxPeer = nullptr;

  static bool processKeyEvent(int keyCode, bool isKeyDown);

  bool isKeyDown(int keyCode);
  void allKeysUp();
  
  std::function<void(int)> onKeyDownFn;
  std::function<void(int)> onKeyUpFn;

protected:
  static std::set<Keyboard*> thisses;

  static juce::ComponentPeer* getFocusedPeer();
  
  void addPressedKey(int keyCode);
  void removePressedKey(int keyCode);

private:
  std::recursive_mutex pressedKeysMutex;
  juce::Component* parent;
  juce::Component* auxParent = nullptr;
  std::set<int> pressedKeys;
  
};
