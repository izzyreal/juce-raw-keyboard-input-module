# juce-raw-keyboard-input-module
Raw keyboard input module for JUCE on Linux, Windows, macOS, iOS and Android

## Usage

```
#include <raw_keyboard_input/src/Keyboard.h>

MyComponent::MyComponent() // extends juce::Component
{
  // We need access to the ComponentPeer in order to distinguish
  // between multiple instances in a DAW, so we pass the holder of
  // the Keyboard instance to the factory method. The holder can be
  // any component in your component tree.
  
  keyboard = KeyboardFactory::instance(this);
  
  // Install callbacks
  
  keyboard->onKeyDownFn = [&](int keyCode){ keyEvent(juce::KeyEvent(keyCode, true)); };
  keyboard->onKeyUpFn = [&](int keyCode){ keyEvent(juce::KeyEvent(keyCode, false)); };

  // or call this in a juce::Timer instead
  
  keyboard->isKeyDown(keyCode);
}
```

On iOS you have to make sure that any components in your tree that
return `true` for `getWantsKeyboardFocus()` and that implement `keyPressed`, should
return `false` in that implementation for iOS for any key presses that you want
your JUCE project to handle via raw keyboard input.

On Android, add the module's `android/java` directory to the app's Java source
sets and derive the app activity from
`nl.izmar.rawkeyboardinput.RawKeyboardInputActivity`. The activity observes
hardware `KeyEvent`s dispatched through the activity window and then passes
them on normally. The component supplied to `KeyboardFactory::instance` is
used to observe JUCE's separate `ComponentPeerView` window directly when its
native peer becomes available.

The Android Emulator's normal key translation synthesizes repeats for modifier
keys. For faithful physical keyboard events, start the emulator with
`-feature QtRawKeyboardInput` and enable `hw.keyboard=yes` in the AVD config.
