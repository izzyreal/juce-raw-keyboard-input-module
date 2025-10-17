#include "Keyboard.h"

std::set<Keyboard*> Keyboard::thisses;

bool Keyboard::processKeyEvent(int keyCode, bool isKeyDown)
{
    bool handled = false;

    for (auto* t : thisses)
    {
        if (t->hasFocus && t->hasFocus())
        {
            if (isKeyDown)
                t->addPressedKey(keyCode);
            else
                t->removePressedKey(keyCode);

            handled = true;
        }
    }

    return handled;
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
    if (pressedKeys.erase(keyCode) > 0 && onKeyUpFn)
        onKeyUpFn(keyCode);
}

void Keyboard::allKeysUp()
{
    std::lock_guard<std::recursive_mutex> lock(pressedKeysMutex);
    for (auto keyCode : pressedKeys)
        if (onKeyUpFn)
            onKeyUpFn(keyCode);
    pressedKeys.clear();
}

