package com.annaerob.cuuube;

import android.content.Context;
import android.opengl.GLSurfaceView;

import javax.microedition.khronos.egl.EGL10;
import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.egl.EGLContext;
import javax.microedition.khronos.egl.EGLDisplay;
import javax.microedition.khronos.opengles.GL10;

class View extends GLSurfaceView {
    public static float ANGLE_X = 0;
    public static float ANGLE_Y = 0;

    protected int redSize = 5;
    protected int greenSize = 6;
    protected int blueSize = 5;
    protected int alphaSize = 0;
    protected int depthSize = 16;
    protected int sampleSize = 4;
    protected int stencilSize = 0;
    protected int[] value = new int[1];

    public View(Context context) {
        super(context);
        setEGLContextFactory(new ContextFactory());
        setEGLConfigChooser(new ConfigChooser());
        setRenderer(new Renderer());


    }

    private static class ContextFactory implements GLSurfaceView.EGLContextFactory {
        public EGLContext createContext(EGL10 egl, EGLDisplay display, EGLConfig eglConfig) {
            final int EGL_CONTEXT_CLIENT_VERSION = 0x3098;
            int[] attrib_list = {EGL_CONTEXT_CLIENT_VERSION, 2, EGL10.EGL_NONE };
            EGLContext context = egl.eglCreateContext(display, eglConfig, EGL10.EGL_NO_CONTEXT, attrib_list);

            return context;
        }

        public void destroyContext(EGL10 egl, EGLDisplay display, EGLContext context) {
            egl.eglDestroyContext(display, context);
        }
    }

    protected class ConfigChooser implements GLSurfaceView.EGLConfigChooser {
        public EGLConfig chooseConfig(EGL10 egl, EGLDisplay display) {
            final int EGL_OPENGL_ES2_BIT = 4;
            int[] configAttributes = {
                    EGL10.EGL_RED_SIZE, redSize,
                    EGL10.EGL_GREEN_SIZE, greenSize,
                    EGL10.EGL_BLUE_SIZE, blueSize,
                    EGL10.EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
                    EGL10.EGL_SAMPLES, sampleSize,
                    EGL10.EGL_DEPTH_SIZE, depthSize,
                    EGL10.EGL_STENCIL_SIZE, stencilSize,
                    EGL10.EGL_NONE
            };

            int[] num_config = new int[1];
            egl.eglChooseConfig(display, configAttributes, null, 0, num_config);
            int numConfigs = num_config[0];
            EGLConfig[] configs = new EGLConfig[numConfigs];
            egl.eglChooseConfig(display, configAttributes, configs, numConfigs, num_config);
            return configs[0];
        }
    }

    private static class Renderer implements GLSurfaceView.Renderer {
        @Override
        public void onDrawFrame(GL10 gl) {
            NativeLibrary.step();
        }

        @Override
        public void onSurfaceChanged(GL10 gl, int width, int height) {
            NativeLibrary.init(width, height);
        }

        @Override
        public void onSurfaceCreated(GL10 gl, EGLConfig config) {

        }
    }
}
