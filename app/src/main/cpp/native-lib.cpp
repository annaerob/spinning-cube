#include <jni.h>
#include <string>
#include <GLES3/gl32.h>
#include <GLES3/gl3ext.h>
#include <math.h>
#include <algorithm>

static const char  glVertexShader[] =
        "attribute vec4 vertexPosition;\n"
        "attribute vec3 vertexColour;\n"
        "varying vec3 fragColour;\n"
        "uniform mat4 projection;\n"
        "uniform mat4 modelView;\n"
        "void main()\n"
        "{\n"
        "    gl_Position = projection * modelView * vertexPosition;\n"
        "    fragColour = vertexColour;\n"
        "}\n";

static const char  glFragmentShader[] =
        "precision mediump float;\n"
        "varying vec3 fragColour;\n"
        "void main()\n"
        "{\n"
        "    gl_FragColor = vec4(fragColour, 1.0);\n"
        "}\n";

GLfloat vertices[] = {
        -1.0f,  1.0f, -1.0f, /* Back. */
        1.0f,  1.0f, -1.0f,
        -1.0f, -1.0f, -1.0f,
        1.0f, -1.0f, -1.0f,
        1.0f,  1.0f,  1.0f, /* Front. */
        1.0f,  1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,
        1.0f, -1.0f,  1.0f,
        -1.0f,  1.0f, -1.0f, /* Left. */
        -1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
        1.0f,  1.0f, -1.0f, /* Right. */
        1.0f, -1.0f, -1.0f,
        1.0f, -1.0f,  1.0f,
        1.0f,  1.0f,  1.0f,
        -1.0f, -1.0f, -1.0f, /* Top. */
        -1.0f, -1.0f,  1.0f,
        1.0f, -1.0f,  1.0f,
        1.0f, -1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f, /* Bottom. */
        -1.0f,  1.0f,  1.0f,
        1.0f,  1.0f,  1.0f,
        1.0f,  1.0f, -1.0f
};

GLfloat color[72] = {
        1.0f, 0.0f, 0.0f,
         1.0f, 0.0f, 0.0f,
        1.0f, 0.0f, 0.0f,
        1.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f,
        0.0f, 1.0f, 0.0f,
        0.0f, 1.0f, 0.0f,
        0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 1.0f,
        0.0f, 0.0f, 1.0f,
        0.0f, 0.0f, 1.0f,
        0.0f, 0.0f, 1.0f,
        1.0f, 1.0f, 0.0f,
        1.0f, 1.0f, 0.0f,
        1.0f, 1.0f, 0.0f,
        1.0f, 1.0f, 0.0f,
        0.0f, 1.0f, 1.0f,
        0.0f, 1.0f, 1.0f,
        0.0f, 1.0f, 1.0f,
        0.0f, 1.0f, 1.0f,
        1.0f, 0.0f, 1.0f,
        1.0f, 0.0f, 1.0f,
        1.0f, 0.0f, 1.0f,
        1.0f, 0.0f, 1.0f
};

GLushort indices[] = { 0, 2, 3, 0, 1, 3, 4, 6, 7, 4, 5, 7, 8, 9, 10, 11, 8, 10, 12, 13, 14, 15, 12, 14, 16, 17, 18, 16, 19, 18, 20, 21, 22, 20, 23, 22 };

GLuint cubeProg;
GLuint vertexLocation;
GLuint vertexColorLocation;
GLuint projectionLocation;
GLuint modelViewLocation;

float projectionMatrix[16];
float modelViewMatrix[16];
float angleX = 0;
float angleY = 0;

#ifndef GLES_IMPL
#define GLES_IMPL

float matDegToRad(float deg)
{
    return M_PI * deg / 180.0f;
}

void matIdentity(float *m)
{
    if (m == NULL)
    {
        return;
    }

    m[0] = 1;
    m[1] = 0;
    m[2] = 0;
    m[3] = 0;
    m[4] = 0;
    m[5] = 1;
    m[6] = 0;
    m[7] = 0;
    m[8] = 0;
    m[9] = 0;
    m[10] = 1;
    m[11] = 0;
    m[12] = 0;
    m[13] = 0;
    m[14] = 0;
    m[15] = 1;
}

void matMultiply(float* dest, float* lhs, float* rhs) {
    float res[16];

    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            res[4 * i + j] = lhs[j] * rhs[4 * i] + lhs[4 + j] * rhs[4 * i + 1] +
                             lhs[8 + j] * rhs[4 * i + 2] + lhs[12 + j] * rhs[4 * i + 3];
        }
    }

    for (int i = 0; i < 16; ++i)
    {
        dest[i] = res[i];
    }
}

void matTranslate(float* mat, float x, float y, float z)
{
    float temp[16];

    matIdentity(temp);

    temp[12] = x;
    temp[13] = y;
    temp[14] = z;

    matMultiply(mat, temp, mat);
}

void matFrustum(float* mat, float left, float right, float bottom, float top, float zNear, float zFar)
{
    float temp, xDist, yDist, zDist;

    temp = 2.0 * zNear;

    xDist = right - left;
    yDist = top - bottom;
    zDist = zFar - zNear;

    matIdentity(mat);

    mat[0] = temp / xDist;
    mat[5] = temp / yDist;
    mat[8] = (right + left) / xDist;
    mat[9] = (top + bottom) / yDist;
    mat[10] = (-zFar - zNear) / zDist;
    mat[11] = -1.0f;
    mat[14] = (-temp * zFar) / zDist;
    mat[15] = 0.0f;
}

void matPerspective(float* mat, float viewField, float aspRatio, float zNear, float zFar)
{
    float xMax, yMax;

    yMax = zNear * tanf(viewField * M_PI / 360.0);
    xMax = yMax * aspRatio;

    matFrustum(mat, -xMax, xMax, -yMax, yMax, zNear, zFar);
}

void matRotateX(float* mat, float angle)
{
    float temp[16];
    matIdentity(temp);

    temp[5] = cos(matDegToRad(angle));
    temp[9] = -sin(matDegToRad(angle));
    temp[6] = sin(matDegToRad(angle));
    temp[10] = cos(matDegToRad(angle));

    matMultiply(mat, temp, mat);
}

void matRotateY(float* mat, float angle)
{
    float temp[16];
    matIdentity(temp);

    temp[0] = cos(matDegToRad(angle));
    temp[8] = sin(matDegToRad(angle));
    temp[2] = -sin(matDegToRad(angle));
    temp[10] = cos(matDegToRad(angle));

    matMultiply(mat, temp, mat);
}

void matRotateZ(float* mat, float angle)
{
    float temp[16];
    matIdentity(temp);

    temp[0] = cos(matDegToRad(angle));
    temp[4] = -sin(matDegToRad(angle));
    temp[1] = sin(matDegToRad(angle));
    temp[5] = cos(matDegToRad(angle));

    matMultiply(mat, temp, mat);
}

void matScale(float* mat, float x, float y, float z)
{
    float temp[16];
    matIdentity(temp);

    temp[0] = x;
    temp[5] = y;
    temp[10] = z;

    matMultiply(mat, temp, mat);
}

GLuint loadShader(GLenum shaderType, const char * shaderSource)
{
    GLuint shader = glCreateShader(shaderType);

    if (shader != 0)
    {
        glShaderSource(shader, 1, &shaderSource, NULL);
        glCompileShader(shader);

        GLint compiled = 0;

        glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);

        if (compiled != GL_TRUE)
        {
            GLint infoLen = 0;
            glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLen);

            if (infoLen > 0)
            {
                glDeleteShader(shader);
                shader = 0;
            }

        }
    }

    return shader;
}

GLuint create(const char* vertex, const char* fragment)
{
    GLuint  vertexShader = loadShader(GL_VERTEX_SHADER, vertex);
    if (vertexShader == 0)
    {
        return 0;
    }

    GLuint fragmentShader = loadShader(GL_FRAGMENT_SHADER, fragment);
    if (fragmentShader == 0)
    {
        return 0;
    }

    GLuint program = glCreateProgram();

    if (program != 0)
    {
        glAttachShader(program, vertexShader);
        glAttachShader(program, fragmentShader);
        glLinkProgram(program);
        GLint linkStatus = GL_FALSE;
        glGetProgramiv(program, GL_LINK_STATUS, &linkStatus);

        if (linkStatus != GL_TRUE)
        {
            glDeleteProgram(program);
            program = 0;
        }
    }

    return program;
}

bool setupGraphics(int width, int height)
{
    cubeProg = create(glVertexShader, glFragmentShader);

    if (cubeProg == 0)
    {
        return false;
    }

    vertexLocation = glGetAttribLocation(cubeProg, "vertexPosition");
    vertexColorLocation = glGetAttribLocation(cubeProg, "vertexColour");
    projectionLocation = glGetUniformLocation(cubeProg, "projeciton");
    modelViewLocation = glGetUniformLocation(cubeProg, "modelView");

    matPerspective(projectionMatrix, 45, (float)width / (float)height, 0.1f, 100);
    glEnable(GL_DEPTH_TEST);

    glViewport(0, 0, width, height);

    return true;
}

void render(int x, int y)
{
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

    matIdentity(modelViewMatrix);

    matRotateX(modelViewMatrix, angleX);
    matRotateY(modelViewMatrix, angleY);

    matTranslate(modelViewMatrix, 0.0f, 0.0f, -10.0f);

    glUseProgram(cubeProg);
    glVertexAttribPointer(vertexLocation, 3, GL_FLOAT, GL_FALSE, 0, vertices);
    glEnableVertexAttribArray(vertexLocation);
    glVertexAttribPointer(vertexColorLocation, 3, GL_FLOAT, GL_FALSE, 0, color);
    glEnableVertexAttribArray(vertexColorLocation);

    glUniformMatrix4fv(projectionLocation, 1, GL_FALSE, projectionMatrix);
    glUniformMatrix4fv(modelViewLocation, 1, GL_FALSE, modelViewMatrix);

    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_SHORT, indices);

    angleX += x;
    if (angleX > 360 || angleX < 0) {
        angleX = 1;
    }

    angleY += y;
    if (angleY > 360 || angleY < 0) {
        angleY = 1;
    }
}

#endif

extern "C"
{
JNIEXPORT void JNICALL Java_com_annaerob_cuuube_NativeLibrary_init(JNIEnv * env, jclass cl, jint wi, jint he) {
    setupGraphics(wi, he);
}

JNIEXPORT void JNICALL
Java_com_annaerob_cuuube_NativeLibrary_step(JNIEnv * env, jclass cl) {
    render(0, 0);
}

JNIEXPORT void JNICALL
Java_com_annaerob_cuuube_NativeLibrary_callrender(JNIEnv * env, jclass cl, jint x, jint y) {
    render(x, y);
}
}
