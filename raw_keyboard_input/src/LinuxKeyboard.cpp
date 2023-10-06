#include "LinuxKeyboard.h"

#include <juce_gui_basics/native/x11/juce_linux_XWindowSystem.h>

LinuxKeyboard::LinuxKeyboard(juce::Component* parent) : Keyboard(parent)
{
    running = true;
    eventLoop = new std::thread([this](){

        display = XOpenDisplay(NULL);

        if (display == nullptr) {
            // Handle error, e.g., throw an exception or return
            return;
        }

        Window root = DefaultRootWindow(display);
        XSelectInput(display, root, KeyPressMask | KeyReleaseMask);

        XEvent event;

        while (this->running) {
            // Blocking call, waits for an event
            XNextEvent(display, &event);

            if (event.type == KeyPress) {
                KeySym ks = XKeycodeToKeysym(display, event.xkey.keycode, 0);
                processKeyEvent(ks, true);
            } else if (event.type == KeyRelease) {
                KeySym ks = XKeycodeToKeysym(display, event.xkey.keycode, 0);
                processKeyEvent(ks, false);
            }
        }

        XCloseDisplay(display);
    });
}

void LinuxKeyboard::timerCallback()
{
    Keyboard::timerCallback();
}

LinuxKeyboard::~LinuxKeyboard()
{
  running = false;
  while (!eventLoop->joinable())
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
  eventLoop->join();
}
