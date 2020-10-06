#ifndef COMPLEXPLANE_H
#define COMPLEXPLANE_H
#include <memory>
#include <vector>

class Framebuffer;
class DoubleFramebuffer;

class ComplexPlane {
public:
    ComplexPlane(int w, int h);
    ~ComplexPlane();
    void update(float dt);
    void render();
private:
    int mWidth;
    int mHeight;
    float mTime = 0.0f;
    uint32_t mLineProgram;
    uint32_t mPointProgram;
    uint32_t mBlitProgram;
    std::vector<float> mLines;
    std::vector<float> mPoints;
    std::shared_ptr<Framebuffer> mDisplayTarget;
    std::shared_ptr<Framebuffer> mUpdateTarget;
};


#endif
