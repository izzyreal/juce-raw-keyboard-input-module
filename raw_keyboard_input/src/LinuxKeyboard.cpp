#include "LinuxKeyboard.h"

#include <juce_gui_basics/native/juce_XWindowSystem_linux.h>

#include <map>

LinuxKeyboard::LinuxKeyboard(juce::Component* parent) : Keyboard(parent)
{
    running = true;
    eventLoop = new std::thread([this](){

        XKeyboardState xKeyboardState;
        std::map<int, int> keyCodePressDuration;

        const int keyrepeatDelay = 400; // ms
        const int keyrepeatInterval = 25; // ms

        while (this->running) {
            if (display == nullptr)
            {
                auto xWindowSystem = juce::XWindowSystem::getInstanceWithoutCreating();
                display = xWindowSystem->getDisplay();
            }

            if (display != nullptr)
            {
                XGetKeyboardControl(display, &xKeyboardState);
                const int globalAutoRepeat = xKeyboardState.global_auto_repeat;
                char keymap[32];
                XQueryKeymap(display, keymap);
                static unsigned int masktable[8] = { 0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80 };

                for (int i = 0; i < 256; i++) {
                    bool wasDown = prev_keymap[i >> 3] & masktable[i & 7];
                    bool isDown = keymap[i >> 3] & masktable[i & 7];
                    KeySym ks = XKeycodeToKeysym(display, (KeyCode) i, 0);

                    if (isDown && !wasDown) {
                        processKeyEvent(ks, true);
                        keyCodePressDuration[ks] = 0;
                    }
                    else if (!isDown && wasDown)
                    {
                        processKeyEvent(ks, false);
                        keyCodePressDuration.erase(ks);
                    }
                    else if (isDown && wasDown && globalAutoRepeat != AutoRepeatModeOff)
                    {
                        keyCodePressDuration[ks] += 1;
                        const auto duration = keyCodePressDuration[ks];

                        if (duration >= keyrepeatDelay && (duration % keyrepeatInterval) == 0)
                        {
                            processKeyEvent(ks, true);
                        }
                    }
                }

                for (int i = 0; i < 32; i++)
                    prev_keymap[i] = keymap[i];
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }
    });
}

LinuxKeyboard::~LinuxKeyboard()
{
    running = false;
    while (!eventLoop->joinable())
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    eventLoop->join();
}
