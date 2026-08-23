#pragma once

#include <set>
#include <functional>
#include <mutex>

class Keyboard {
public:
    Keyboard() { thisses.emplace(this); }
    virtual ~Keyboard() { thisses.erase(this); }

    static bool processKeyEvent(int keyCode, bool isKeyDown);
    static void releaseAllKeys();

    bool isKeyDown(int keyCode);
    void allKeysUp();

    std::function<void(int)> onKeyDownFn;
    std::function<void(int)> onKeyUpFn;

    // New delegate used for focus determination
    std::function<bool()> hasFocus = [] { return false; };

protected:
    static std::set<Keyboard*> thisses;

private:
    std::recursive_mutex pressedKeysMutex;
    std::set<int> pressedKeys;

    void addPressedKey(int keyCode);
    void removePressedKey(int keyCode);
};
