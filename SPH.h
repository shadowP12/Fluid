#ifndef SPH_H
#define SPH_H

#include <vector>
#include "glm.hpp"

struct Input;

struct Context
{
    alignas(16) glm::vec3 cameraPosition;
    alignas(16) glm::vec3 cameraRight;
    alignas(16) glm::vec3 cameraUp;
    alignas(16) glm::vec3 cameraFront;
    alignas(16) glm::vec4 cameraParams;
    alignas(16) glm::vec4 screenParams;
    alignas(4) int particleCount;
    alignas(4) float deltaTime;
};

struct Camera {
    glm::vec3 position;
    glm::vec3 front;
    glm::vec3 up;
    glm::vec3 right;
    float yaw;
    float pitch;
    float fov;
    float aperture;
    float focalDist;
};

struct Particle {
    alignas(16) glm::vec3 curPos;
    alignas(16) glm::vec3 prePos;
    alignas(16) glm::vec3 acceleration;
    alignas(16) glm::vec3 velocity;
    alignas(4) float density;
    alignas(4) float pressure;
};

class SPH {
public:
    SPH(int w, int h, const Input *input);
    ~SPH();
    void tick(float dt);
private:
    void updateContext(float dt);
    void updateBuffers();
    void render();
private:
    Input* mInput;
    int mWidth;
    int mHeight;
    Context mContext;
    Camera mCamera;
    std::vector<Particle> mParticles;
    uint32_t mContextBuffer;
    uint32_t mParticleBuffer;
    uint32_t mOutputTex;
    uint32_t mPrecomputeProgram;
    uint32_t mAccelerationProgram;
    uint32_t mFinalProgram;
    uint32_t mDisplayProgram;
    uint32_t mRayMatchProgram;
};

#endif