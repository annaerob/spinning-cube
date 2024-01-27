package com.annaerob.cuuube;

import static com.annaerob.cuuube.NativeLibrary.render;

import android.content.Context;
import android.view.GestureDetector;
import android.view.MotionEvent;

public class OnSwipe implements android.view.View.OnTouchListener {

    private final GestureDetector gestureDetector;

    public OnSwipe(Context ctx){
        gestureDetector = new GestureDetector(ctx, new GestureListener());
    }
    @Override
    public boolean onTouch(android.view.View v, MotionEvent event) {
        return gestureDetector.onTouchEvent(event);
    }

    private final class GestureListener extends GestureDetector.SimpleOnGestureListener {

        private static final int SWIPE_THRESHOLD = 100;
        private static final int SWIPE_VELOCITY_THRESHOLD = 100;

        @Override
        public boolean onDown(MotionEvent e) {
            return true;
        }

        @Override
        public boolean onFling(MotionEvent e1, MotionEvent e2, float velocityX, float velocityY) {
            boolean result = false;
            try {
                float diffY = e2.getY() - e1.getY();
                float diffX = e2.getX() - e1.getX();
                if (Math.abs(diffX) > Math.abs(diffY)) {
                    if (Math.abs(diffX) > SWIPE_THRESHOLD && Math.abs(velocityX) > SWIPE_VELOCITY_THRESHOLD) {
                        if (diffX > 0) {
                            onSwipeRight();
                        } else {
                            onSwipeLeft();
                        }
                        result = true;
                    }
                }
                else if (Math.abs(diffY) > SWIPE_THRESHOLD && Math.abs(velocityY) > SWIPE_VELOCITY_THRESHOLD) {
                    if (diffY > 0) {
                        onSwipeBottom();
                    } else {
                        onSwipeTop();
                    }
                    result = true;
                }
            } catch (Exception exception) {
                exception.printStackTrace();
            }
            return result;
        }
    }

    public void onSwipeRight() {
        render(0, 1);
    }

    public void onSwipeLeft() {
        render(0, -1);
    }

    public void onSwipeTop() {
        render(1, 0);
    }

    public void onSwipeBottom() {
        render(-1, 0);
    }
}
