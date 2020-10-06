#include "GraphicalUtil.h"
#include "glad/glad.h"

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
    if (mFramebufferA)
        delete mFramebufferA;
    if (mFramebufferB)
        delete mFramebufferB;
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

static void checkShaderCompileErrors(GLuint id) {
    GLint success;
    GLchar log[1024];
    glGetShaderiv(id, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(id, 1024, NULL, log);
        printf("shader compile error : %s \n", log);
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