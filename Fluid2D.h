#ifndef FLUID2D_H
#define FLUID2D_H
#include <memory>

class Framebuffer;
class DoubleFramebuffer;

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
