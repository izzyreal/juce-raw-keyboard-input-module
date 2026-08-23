#pragma once

#include "Keyboard.h"

#include <juce_gui_basics/juce_gui_basics.h>

class AndroidKeyboard final : public Keyboard,
                              private juce::ComponentMovementWatcher
{
public:
    explicit AndroidKeyboard(juce::Component* componentToObserve);
    ~AndroidKeyboard() override;

private:
    void componentMovedOrResized(bool, bool) override {}
    void componentPeerChanged() override;
    void componentVisibilityChanged() override {}

    void updatePeerObservation();
    void stopObservingPeer();

    juce::GlobalRef observedPeer;
};
