#ifndef FLUID2D_H
#define FLUID2D_H
#include <memory>

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

class Fluid2D {
public:
    Fluid2D(int size);
    ~Fluid2D();
    void update(float dt);
    void render();
    void splat(float x, float y, float dx, float dy);
private:
    void advect(float dt);
    void buoyancy(float dt);
    void divergence();
    void pressure();
    void subtractGradient();
private:
    int mSize;
    uint32_t mQuadVAO;
    uint32_t mQuadVBO;
    uint32_t mCommonProgram;
    uint32_t mBoundaryProgram;
    uint32_t mSplatProgram;
    uint32_t mAdvectionProgram;
    uint32_t mBuoyancyProgram;
    uint32_t mDivergenceProgram;
    uint32_t mJacobiProgram;
    uint32_t mSubtractGradientProgram;
    std::shared_ptr<Framebuffer> mBoundary;
    std::shared_ptr<Framebuffer> mDivergence;
    std::shared_ptr<DoubleFramebuffer> mDensity;
    std::shared_ptr<DoubleFramebuffer> mVelocity;
    std::shared_ptr<DoubleFramebuffer> mTemperature;
    std::shared_ptr<DoubleFramebuffer> mPressure;
};


#endif
