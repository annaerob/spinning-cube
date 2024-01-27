package com.annaerob.cuuube;

public class AngleStore {
    protected float x;
    protected float y;

    public AngleStore() {
        x = 0;
        y = 0;
    }

    public float incX() {
        ++x;

        if (x > 360) {
            x -= 360;
        }

        return x;
    }

    public float incY() {
        ++y;

        if (y > 360) {
            y -= 360;
        }

        return y;
    }

    public float decX() {
        --x;

        if (x < 0) {
            x = 0;
        }

        return x;
    }

    public float decY() {
        --y;

        if (y < 0) {
            y =  0;
        }

        return y;
    }
}
