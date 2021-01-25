#ifndef GRAPHICAL_UTIL_H
#define GRAPHICAL_UTIL_H
#include <vector>

class Framebuffer {
public:
    Framebuffer(uint32_t w, uint32_t h, uint32_t internalFormat, uint32_t format, uint32_t type, uint32_t param);
    ~Framebuffer();
    uint32_t getWidth() {
        return mWidth;
    }
    uint32_t getHeight() {
        return mHeight;
    }
    uint32_t getFramebuffer() {
        return mFramebufferID;
    }
    uint32_t getTexture() {
        return mTextureID;
    }
protected:
    friend class DoubleFramebuffer;
    uint32_t mWidth;
    uint32_t mHeight;
    uint32_t mFramebufferID;
    uint32_t mTextureID;
};

class DoubleFramebuffer {
public:
    DoubleFramebuffer(uint32_t w, uint32_t h, uint32_t internalFormat, uint32_t format, uint32_t type, uint32_t param);
    ~DoubleFramebuffer();
    Framebuffer* getReadFramebuffer() {
        return mFramebufferA;
    }
    Framebuffer* getWriteFramebuffer() {
        return mFramebufferB;
    }
    void swap() {
        Framebuffer* temp = nullptr;
        temp = mFramebufferA;
        mFramebufferA = mFramebufferB;
        mFramebufferB = temp;
    }
protected:
    Framebuffer* mFramebufferA = nullptr;
    Framebuffer* mFramebufferB = nullptr;
};

uint32_t createProgram(const char* vs, const char* fs);

uint32_t createProgram(const char* vs, const char* tcs, const char* tes, const char* fs);

uint32_t createProgram(const char* cs);

#endif
