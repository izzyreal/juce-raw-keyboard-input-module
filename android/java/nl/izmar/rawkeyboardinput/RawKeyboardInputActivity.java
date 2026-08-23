package nl.izmar.rawkeyboardinput;

import android.app.Activity;
import android.view.KeyEvent;
import android.view.View;

/**
 * Activity base that observes hardware key events without consuming them.
 *
 * The native methods are implemented by the raw_keyboard_input JUCE module.
 */
public class RawKeyboardInputActivity extends Activity
{
    private static native void handleKeyEvent (int keyCode, boolean isKeyDown);
    private static native void releaseAllKeys();

    private static boolean dispatchingKeyEventThroughActivity;

    private static void forwardKeyEvent (KeyEvent event)
    {
        final int action = event.getAction();

        if (action != KeyEvent.ACTION_DOWN && action != KeyEvent.ACTION_UP)
            return;

        handleKeyEvent (event.getKeyCode(), action == KeyEvent.ACTION_DOWN);
    }

    static boolean isJucePeerView (View view)
    {
        // JUCE loads this class from bytecode embedded in the native library, so
        // it is intentionally identified without a compile-time dependency.
        return view != null
                && "com.rmsl.juce.ComponentPeerView".equals (view.getClass().getName());
    }

    static void observeJucePeerView (View view)
    {
        if (!isJucePeerView (view))
            return;

        view.setOnKeyListener ((ignoredView, ignoredKeyCode, event) ->
        {
            // A peer hosted in the Activity window is observed first by
            // dispatchKeyEvent and then by this listener. Forward it once.
            if (!dispatchingKeyEventThroughActivity)
                forwardKeyEvent (event);

            return false;
        });
    }

    static void stopObservingJucePeerView (View view)
    {
        if (isJucePeerView (view))
            view.setOnKeyListener (null);
    }

    @Override
    public boolean dispatchKeyEvent (KeyEvent event)
    {
        dispatchingKeyEventThroughActivity = true;

        try
        {
            forwardKeyEvent (event);
            return super.dispatchKeyEvent (event);
        }
        finally
        {
            dispatchingKeyEventThroughActivity = false;
        }
    }

    @Override
    public void onWindowFocusChanged (boolean hasFocus)
    {
        if (!hasFocus)
            releaseAllKeys();

        super.onWindowFocusChanged (hasFocus);
    }

    @Override
    protected void onPause()
    {
        releaseAllKeys();
        super.onPause();
    }
}
