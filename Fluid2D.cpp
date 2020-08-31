#include "Fluid2D.h"
#include "glad/glad.h"
#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>

#define SAFE_DELETE(x) \
    { \
        delete x; \
        x = nullptr; \
    }

#define SAFE_DELETE_ARRAY(x) \
    { \
        delete[] x; \
        x = nullptr; \
    }
static bool readFileData(const std::string& path, std::string& out);
static void checkShaderCompileErrors(GLuint id);
static void checkProgramCompileErrors(GLuint id);
static uint32_t createProgram(const char* vertexSource, const char* fragmentSource);

float quadVertices[] = {
        // positions   // texCoords
        -1.0f,  1.0f,  0.0f, 1.0f,
        -1.0f, -1.0f,  0.0f, 0.0f,
        1.0f, -1.0f,  1.0f, 0.0f,

        -1.0f,  1.0f,  0.0f, 1.0f,
        1.0f, -1.0f,  1.0f, 0.0f,
        1.0f,  1.0f,  1.0f, 1.0f
};

Framebuffer::Framebuffer(uint32_t w, uint32_t h, uint32_t internalFormat, uint32_t format, uint32_t type, uint32_t param) {
    glCreateTextures(GL_TEXTURE_2D, 1, &mTextureID);
    glBindTexture(GL_TEXTURE_2D, mTextureID);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, param);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, param);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, w, h, 0, format, type, nullptr);
    glCreateFramebuffers(1, &mFramebufferID);
    glBindFramebuffer(GL_FRAMEBUFFER, mFramebufferID);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, mTextureID, 0);
    glViewport(0, 0, w, h);
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

Framebuffer::~Framebuffer() {
    glDeleteTextures(1, &mTextureID);
    glDeleteFramebuffers(1, &mFramebufferID);
}

DoubleFramebuffer::DoubleFramebuffer(uint32_t w, uint32_t h, uint32_t internalFormat, uint32_t format, uint32_t type, uint32_t param) {
    mFramebufferA = new Framebuffer(w, h, internalFormat, format, type, param);
    mFramebufferB = new Framebuffer(w, h, internalFormat, format, type, param);
}

DoubleFramebuffer::~DoubleFramebuffer() {
    SAFE_DELETE(mFramebufferA);
    SAFE_DELETE(mFramebufferB);
}

Fluid2D::Fluid2D(int size) {
    mSize = size;
    mVelocity = std::shared_ptr<DoubleFramebuffer>(new DoubleFramebuffer(mSize, mSize, GL_RGBA16F, GL_RGBA, GL_FLOAT, GL_LINEAR));
    glGenVertexArrays(1, &mQuadVAO);
    glGenBuffers(1, &mQuadVBO);
    glBindVertexArray(mQuadVAO);
    glBindBuffer(GL_ARRAY_BUFFER, mQuadVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    std::string vs, fs;
    readFileData("Res/Fluid2D/common.vs", vs);
    readFileData("Res/Fluid2D/common.fs", fs);
    mCommonProgram = createProgram(vs.c_str(), fs.c_str());
    readFileData("Res/Fluid2D/advection.fs", fs);
    mAdvectionProgram = createProgram(vs.c_str(), fs.c_str());
}

Fluid2D::~Fluid2D() {
    glDeleteVertexArrays(1, &mQuadVAO);
    glDeleteBuffers(1, &mQuadVBO);
    glDeleteProgram(mCommonProgram);
    glDeleteProgram(mAdvectionProgram);
}

void Fluid2D::update(float dt) {
    glBindFramebuffer(GL_FRAMEBUFFER, mVelocity->getWriteFramebuffer()->getFramebuffer());
    glViewport(0, 0, mSize, mSize);
    glUseProgram(mAdvectionProgram);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, mVelocity->getReadFramebuffer()->getTexture());
    glUniform1i(glGetUniformLocation(mAdvectionProgram, "u_texture"), 0);
    glBindVertexArray(mQuadVAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    mVelocity->swap();
}

void  Fluid2D::render() {
    glViewport(0, 0, mSize, mSize);
    glUseProgram(mCommonProgram);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, mVelocity->getReadFramebuffer()->getTexture());
    glUniform1i(glGetUniformLocation(mCommonProgram, "u_texture"), 0);
    glBindVertexArray(mQuadVAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
}

static void checkShaderCompileErrors(GLuint id) {
    GLint success;
    GLchar log[1024];
    glGetShaderiv(id, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(id, 1024, NULL, log);
        printf("shader compile error : %s \n", log);
    }
}

static void checkProgramCompileErrors(GLuint id) {
    GLint success;
    GLchar log[1024];
    glGetProgramiv(id, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(id, 1024, NULL, log);
        printf("program compile error : %s \n", log);
    }
}

uint32_t createProgram(const char* vertexSource, const char* fragmentSource) {
    GLuint vertexID, fragmentID;
    vertexID = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexID, 1, &vertexSource, NULL);
    glCompileShader(vertexID);
    checkShaderCompileErrors(vertexID);

    fragmentID = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentID, 1, &fragmentSource, NULL);
    glCompileShader(fragmentID);
    checkShaderCompileErrors(fragmentID);

    GLuint programID;
    programID = glCreateProgram();
    glAttachShader(programID, vertexID);
    glAttachShader(programID, fragmentID);
    glLinkProgram(programID);
    checkProgramCompileErrors(programID);

    glDeleteShader(vertexID);
    glDeleteShader(fragmentID);

    return programID;
}

static bool readFileData(const std::string& path, std::string& out) {
    std::istream* stream = &std::cin;
    std::ifstream file;

    file.open(path, std::ios_base::binary);
    stream = &file;
    if (file.fail()) {
        printf("cannot open input file %s \n", path.c_str());
        return false;
    }
    out = std::string((std::istreambuf_iterator<char>(*stream)), std::istreambuf_iterator<char>());
    return true;
}