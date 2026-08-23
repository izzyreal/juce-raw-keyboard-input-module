#include "AndroidKeyboard.h"

#include <jni.h>

namespace juce
{
#define JNI_CLASS_MEMBERS(METHOD, STATICMETHOD, FIELD, STATICFIELD, CALLBACK) \
    STATICMETHOD(observeJucePeerView, "observeJucePeerView",                  \
                 "(Landroid/view/View;)V")                                   \
    STATICMETHOD(stopObservingJucePeerView, "stopObservingJucePeerView",     \
                 "(Landroid/view/View;)V")

DECLARE_JNI_CLASS(RawKeyboardInputActivity,
                  "nl/izmar/rawkeyboardinput/RawKeyboardInputActivity")
#undef JNI_CLASS_MEMBERS
}

AndroidKeyboard::AndroidKeyboard(juce::Component* componentToObserve)
    : juce::ComponentMovementWatcher(componentToObserve)
{
    updatePeerObservation();
}

AndroidKeyboard::~AndroidKeyboard()
{
    stopObservingPeer();
}

void AndroidKeyboard::componentPeerChanged()
{
    updatePeerObservation();
}

void AndroidKeyboard::updatePeerObservation()
{
    auto* component = getComponent();
    auto* peer = component != nullptr
                     ? static_cast<jobject>(component->getWindowHandle())
                     : nullptr;
    auto* env = juce::getEnv();

    if (env->IsSameObject(observedPeer.get(), peer))
        return;

    stopObservingPeer();

    if (peer == nullptr)
        return;

    env->CallStaticVoidMethod(juce::RawKeyboardInputActivity,
                              juce::RawKeyboardInputActivity.observeJucePeerView,
                              peer);
    observedPeer = juce::GlobalRef(juce::addLocalRefOwner(peer));
}

void AndroidKeyboard::stopObservingPeer()
{
    if (observedPeer.get() == nullptr)
        return;

    juce::getEnv()->CallStaticVoidMethod(
        juce::RawKeyboardInputActivity,
        juce::RawKeyboardInputActivity.stopObservingJucePeerView,
        observedPeer.get());
    observedPeer.clear();
}

extern "C" JNIEXPORT void JNICALL
Java_nl_izmar_rawkeyboardinput_RawKeyboardInputActivity_handleKeyEvent(
    JNIEnv *, jclass, jint, jboolean);

extern "C" JNIEXPORT void JNICALL
Java_nl_izmar_rawkeyboardinput_RawKeyboardInputActivity_releaseAllKeys(JNIEnv *,
                                                                       jclass);

extern "C" JNIEXPORT void JNICALL
Java_nl_izmar_rawkeyboardinput_RawKeyboardInputActivity_handleKeyEvent(
    JNIEnv *, jclass, const jint keyCode, const jboolean isKeyDown)
{
    Keyboard::processKeyEvent(static_cast<int>(keyCode), isKeyDown == JNI_TRUE);
}

extern "C" JNIEXPORT void JNICALL
Java_nl_izmar_rawkeyboardinput_RawKeyboardInputActivity_releaseAllKeys(JNIEnv *,
                                                                       jclass)
{
    Keyboard::releaseAllKeys();
}
