#ifndef SPH_H
#define SPH_H

#include <vector>

struct Input;

class SPH{
public:
    SPH(int w, int h, const Input *input);
    ~SPH();
    void tick(float dt);
private:
    Input* mInput;
    int mWidth;
    int mHeight;
    uint32_t mOutputTex;
    uint32_t mDisplayProgram;
    uint32_t mRayMatchProgram;
};

#endif