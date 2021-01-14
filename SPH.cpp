#include "SPH.h"
#include "glad/glad.h"
#include "CommonUtil.h"
#include "GraphicalUtil.h"

static float quadVertices[] = {
        -1.0f,  1.0f,
        -1.0f, -1.0f,
        1.0f, -1.0f,

        -1.0f,  1.0f,
        1.0f, -1.0f,
        1.0f,  1.0f
};

static float quadTexcoord[] = {
        0.0f, 1.0f,
        0.0f, 0.0f,
        1.0f, 0.0f,

        0.0f, 1.0f,
        1.0f, 0.0f,
        1.0f, 1.0f
};

SPH::SPH(int w, int h, const Input* input) {
    mInput = const_cast<Input*>(input);
    mWidth = w;
    mHeight = h;

    // gpu resource
    glGenTextures(1, &mOutputTex);
    glBindTexture(GL_TEXTURE_2D, mOutputTex);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, mWidth, mHeight, 0, GL_RGBA, GL_FLOAT, NULL);
    glBindTexture(GL_TEXTURE_2D, 0);

    // shader program
    std::string vs, fs, cs;
    vs = readFileData(getProjDir()+"/Res/SPH/display.vs");
    fs = readFileData(getProjDir()+"/Res/SPH/display.fs");
    mDisplayProgram = createProgram(vs.c_str(), fs.c_str());

    cs = readFileData(getProjDir()+"/Res/SPH/raymatch.comp");
    mRayMatchProgram = createComputeProgram(cs.c_str());
}

SPH::~SPH() {
    glDeleteTextures(1, &mOutputTex);
    glDeleteProgram(mDisplayProgram);
    glDeleteProgram(mRayMatchProgram);
}

void SPH::tick(float dt) {
    // ray match
    glUseProgram(mRayMatchProgram);
    glBindImageTexture(0, mOutputTex, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);
    glDispatchCompute((GLuint)mWidth, (GLuint)mHeight, 1);
    glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

    // display
    glViewport(0, 0, mWidth, mHeight);
    glClearColor(0.3f, 0.0f, 0.8f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glUseProgram(mDisplayProgram);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, mOutputTex);
    glUniform1i(glGetUniformLocation(mDisplayProgram, "u_texture"), 0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), &quadVertices[0]);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), &quadTexcoord[0]);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
}