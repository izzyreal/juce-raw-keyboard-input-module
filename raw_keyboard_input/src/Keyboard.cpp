#include "Keyboard.h"

std::set<Keyboard*> Keyboard::thisses;

Keyboard::Keyboard(juce::Component* initialParent) : parent(initialParent)
{
  thisses.emplace(this);
}

Keyboard::~Keyboard()
{
  thisses.erase(this);
}

bool Keyboard::processKeyEvent(int keyCode, bool isKeyDown)
{
  auto focusedPeer = getFocusedPeer();

  if (focusedPeer == nullptr)
    return false;

  for (auto t : thisses) {
    if (t->getPeer() == focusedPeer || t->getAuxPeer() == focusedPeer) {
      if (isKeyDown)
        t->addPressedKey(keyCode);
      else
        t->removePressedKey(keyCode);
    }
  }

  return true;
}

juce::ComponentPeer* Keyboard::getFocusedPeer()
{
  for (int i = 0; i < juce::ComponentPeer::getNumPeers(); i++) {
    if (juce::ComponentPeer::getPeer(i)->isFocused()) {
      return juce::ComponentPeer::getPeer(i);
    }
  }
  return nullptr;
}

juce::ComponentPeer* Keyboard::getPeer() const
{
  return parent ? parent->getPeer() : nullptr;
}

juce::ComponentPeer* Keyboard::getAuxPeer() const
{
  return auxParent ? auxParent->getPeer() : nullptr;
}

bool Keyboard::isKeyDown(int keyCode)
{
  std::lock_guard<std::recursive_mutex> lock(pressedKeysMutex);
  return pressedKeys.count(keyCode) == 1;
}

void Keyboard::addPressedKey(int keyCode)
{
  std::lock_guard<std::recursive_mutex> lock(pressedKeysMutex);
  pressedKeys.emplace(keyCode);
  if (onKeyDownFn) onKeyDownFn(keyCode);
}

void Keyboard::removePressedKey(int keyCode)
{
  std::lock_guard<std::recursive_mutex> lock(pressedKeysMutex);
  if (pressedKeys.count(keyCode) == 1)
    pressedKeys.erase(keyCode);
  if (onKeyUpFn) onKeyUpFn(keyCode);
}

void Keyboard::allKeysUp()
{
  std::lock_guard<std::recursive_mutex> lock(pressedKeysMutex);
  for (auto keyCode : pressedKeys)
    onKeyUpFn(keyCode);
  pressedKeys.clear();
}

void Keyboard::setAuxParent(juce::Component* newAuxParent)
{
  auxParent = newAuxParent;
}
