#include "ComplexPlane.h"
#include "glad/glad.h"
#include "CommonUtil.h"
#include "GraphicalUtil.h"
#include <glm.hpp>
#include <vector>

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

ComplexPlane::ComplexPlane(int w, int h) {
    mWidth = w;
    mHeight = h;
    mDisplayTarget = std::make_shared<Framebuffer>(mWidth, mHeight, GL_RGBA16F, GL_RGBA, GL_FLOAT, GL_LINEAR);
    mUpdateTarget = std::make_shared<Framebuffer>(mWidth, mHeight, GL_RGBA16F, GL_RGBA, GL_FLOAT, GL_LINEAR);

    std::string vs, fs;
    vs = readFileData(getProjDir()+"/Res/ComplexPlane/blit.vs");
    fs = readFileData(getProjDir()+"/Res/ComplexPlane/blit.fs");
    mBlitProgram = createProgram(vs.c_str(), fs.c_str());
    vs = readFileData(getProjDir()+"/Res/ComplexPlane/point.vs");
    fs = readFileData(getProjDir()+"/Res/ComplexPlane/point.fs");
    mPointProgram = createProgram(vs.c_str(), fs.c_str());
    vs = readFileData(getProjDir()+"/Res/ComplexPlane/line.vs");
    fs = readFileData(getProjDir()+"/Res/ComplexPlane/line.fs");
    mLineProgram = createProgram(vs.c_str(), fs.c_str());

    mTime = 0.0f;
    mPoints.resize(6);
    mPoints[0] = 0.2f * (glm::cos(2.0f * 3.1415926f * mTime + 0.25f * 3.1415926f)); //real
    mPoints[1] = 0.2f * (glm::sin(2.0f * 3.1415926f * mTime + 0.25f * 3.1415926f)); //imag
    mPoints[2] = mPoints[0] + 0.1f * (glm::cos(4.0f * 3.1415926f * mTime + 0.0f * 3.1415926f)); //real
    mPoints[3] = mPoints[1] + 0.1f * (glm::sin(4.0f * 3.1415926f * mTime + 0.0f * 3.1415926f)); //imag
    mPoints[4] = mPoints[2] + 0.25f * (glm::cos(16.0f * 3.1415926f * mTime + 0.6f * 3.1415926f)); //real
    mPoints[5] = mPoints[3] + 0.25f * (glm::sin(16.0f * 3.1415926f * mTime + 0.6f * 3.1415926f)); //imag
    mLines.resize(8);
    mLines[0] = 0.0f;
    mLines[1] = 0.0f;
    mLines[2] = mPoints[0];
    mLines[3] = mPoints[1];
    mLines[4] = mPoints[2];
    mLines[5] = mPoints[3];
    mLines[6] = mPoints[4];
    mLines[7] = mPoints[5];
}

ComplexPlane::~ComplexPlane() {
    glDeleteProgram(mBlitProgram);
    glDeleteProgram(mPointProgram);
    glDeleteProgram(mLineProgram);
}

void ComplexPlane::update(float dt) {
    mTime += dt;
    mPoints[0] = 0.2f * (glm::cos(2.0f * 3.1415926f * mTime + 0.25f * 3.1415926f)); //real
    mPoints[1] = 0.2f * (glm::sin(2.0f * 3.1415926f * mTime + 0.25f * 3.1415926f)); //imag
    mPoints[2] = mPoints[0] + 0.1f * (glm::cos(4.0f * 3.1415926f * mTime + 0.0f * 3.1415926f)); //real
    mPoints[3] = mPoints[1] + 0.1f * (glm::sin(4.0f * 3.1415926f * mTime + 0.0f * 3.1415926f)); //imag
    mPoints[4] = mPoints[2] + 0.25f * (glm::cos(16.0f * 3.1415926f * mTime + 0.6f * 3.1415926f)); //real
    mPoints[5] = mPoints[3] + 0.25f * (glm::sin(16.0f * 3.1415926f * mTime + 0.6f * 3.1415926f)); //imag

    mLines[2] = mPoints[0];
    mLines[3] = mPoints[1];
    mLines[4] = mPoints[2];
    mLines[5] = mPoints[3];
    mLines[6] = mPoints[4];
    mLines[7] = mPoints[5];
}

void ComplexPlane::render() {
    glBindFramebuffer(GL_FRAMEBUFFER, mUpdateTarget->getFramebuffer());
    glViewport(0, 0, mWidth, mHeight);
    glUseProgram(mPointProgram);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), &mPoints[4]);
    glDrawArrays(GL_POINTS, 0, 1);
    glDisableVertexAttribArray(0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    glViewport(0, 0, mWidth, mHeight);
    glClearColor(0.3f, 0.0f, 0.8f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glUseProgram(mBlitProgram);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, mUpdateTarget->getTexture());
    glUniform1i(glGetUniformLocation(mBlitProgram, "u_texture"), 0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), &quadVertices[0]);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), &quadTexcoord[0]);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);

    glUseProgram(mLineProgram);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), &mLines[0]);
    glDrawArrays(GL_LINE_STRIP, 0, 6);
    glDisableVertexAttribArray(0);
}