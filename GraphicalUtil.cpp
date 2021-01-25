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

uint32_t createProgram(const char* vs, const char* fs) {
    GLuint vsId, fsId;
    vsId = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vsId, 1, &vs, NULL);
    glCompileShader(vsId);
    checkShaderCompileErrors(vsId);

    fsId = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fsId, 1, &fs, NULL);
    glCompileShader(fsId);
    checkShaderCompileErrors(fsId);

    GLuint programID;
    programID = glCreateProgram();
    glAttachShader(programID, vsId);
    glAttachShader(programID, fsId);
    glLinkProgram(programID);
    checkProgramCompileErrors(programID);

    glDeleteShader(vsId);
    glDeleteShader(fsId);

    return programID;
}

uint32_t createProgram(const char* vs, const char* tcs, const char* tes, const char* fs) {
    GLuint vsId, tcsId, tesId, fsId;
    vsId = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vsId, 1, &vs, NULL);
    glCompileShader(vsId);
    checkShaderCompileErrors(vsId);

    tcsId = glCreateShader(GL_TESS_CONTROL_SHADER);
    glShaderSource(tcsId, 1, &tcs, NULL);
    glCompileShader(tcsId);
    checkShaderCompileErrors(tcsId);

    tesId = glCreateShader(GL_TESS_EVALUATION_SHADER);
    glShaderSource(tesId, 1, &tes, NULL);
    glCompileShader(tesId);
    checkShaderCompileErrors(tesId);

    fsId = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fsId, 1, &fs, NULL);
    glCompileShader(fsId);
    checkShaderCompileErrors(fsId);

    GLuint programID;
    programID = glCreateProgram();
    glAttachShader(programID, vsId);
    glAttachShader(programID, tcsId);
    glAttachShader(programID, tesId);
    glAttachShader(programID, fsId);
    glLinkProgram(programID);
    checkProgramCompileErrors(programID);

    glDeleteShader(vsId);
    glDeleteShader(tcsId);
    glDeleteShader(tesId);
    glDeleteShader(fsId);

    return programID;
}

uint32_t createProgram(const char* cs) {
    GLuint csId;
    csId = glCreateShader(GL_COMPUTE_SHADER);
    glShaderSource(csId, 1, &cs, NULL);
    glCompileShader(csId);
    checkShaderCompileErrors(csId);

    GLuint programID;
    programID = glCreateProgram();
    glAttachShader(programID, csId);
    glLinkProgram(programID);
    checkProgramCompileErrors(programID);

    glDeleteShader(csId);

    return programID;
}