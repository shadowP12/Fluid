#include "SPH.h"
#include "glad/glad.h"
#include "CommonUtil.h"
#include "GraphicalUtil.h"
#include <random>

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

SPH::SPH(int w, int h, const Input* input) {
    mInput = const_cast<Input*>(input);
    mWidth = w;
    mHeight = h;

    // cpu resource
    mCamera.position = glm::vec3(0.0f, 0.0f, 0.0f);
    mCamera.right = glm::vec3(1.0f, 0.0f, 0.0f);
    mCamera.up = glm::vec3(0.0f, 1.0f, 0.0f);
    mCamera.front = glm::vec3(0.0f, 0.0f, -1.0f);
    mCamera.yaw = -90.0f;
    mCamera.pitch = 0.0f;
    mCamera.fov = 60.0f;
    mCamera.focalDist = 0.1f;
    mCamera.aperture = 0.0f;

    mContext.cameraPosition = mCamera.position;
    mContext.cameraRight = mCamera.right;
    mContext.cameraUp = mCamera.up;
    mContext.cameraFront = mCamera.front;
    mContext.cameraParams = glm::vec4(mCamera.fov, mCamera.focalDist, mCamera.aperture, 0.0f);
    mContext.screenParams = glm::vec4((float)mWidth, (float)mHeight, 0.0f, 0.0f);
    mContext.particleCount = 0;

    float rndX, rndY, rndZ;
    float radius = 0.50f;
    std::mt19937 eng;
    std::uniform_real_distribution<float> dist(radius * (-1.0f), radius);

    for(unsigned int i = 0; i < 50; ++i) {
        rndX = dist(eng);
        rndY = dist(eng);
        rndZ = dist(eng);
        Particle particle;
        particle.curPos = glm::vec3(rndX, rndY, rndZ);
        particle.prePos = glm::vec3(rndX, rndY, rndZ);
        particle.velocity = glm::vec3(rndX, rndY, rndZ) * 0.01f;
        particle.acceleration = glm::vec3(0.0f);
        mParticles.push_back(particle);
    }

    // gpu resource
    glGenBuffers(1, &mContextBuffer);
    glBindBuffer(GL_UNIFORM_BUFFER, mContextBuffer);
    glBufferData(GL_UNIFORM_BUFFER, sizeof(Context), nullptr, GL_STATIC_DRAW);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    glGenBuffers(1, &mParticleBuffer);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, mParticleBuffer);
    glBufferData(GL_SHADER_STORAGE_BUFFER, mParticles.size() * sizeof(Particle), mParticles.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

    glGenTextures(1, &mOutputTex);
    glBindTexture(GL_TEXTURE_2D, mOutputTex);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, mWidth, mHeight, 0, GL_RGBA, GL_FLOAT, NULL);
    glBindTexture(GL_TEXTURE_2D, 0);

    // shader program
    std::string vs, fs, cs;
    vs = readFileData(getProjDir()+"/Res/SPH/display.vs");
    fs = readFileData(getProjDir()+"/Res/SPH/display.fs");
    mDisplayProgram = createProgram(vs.c_str(), fs.c_str());

    cs = readFileData(getProjDir()+"/Res/SPH/precompute.comp");
    mPrecomputeProgram = createComputeProgram(cs.c_str());

    cs = readFileData(getProjDir()+"/Res/SPH/acceleration.comp");
    mAccelerationProgram = createComputeProgram(cs.c_str());

    cs = readFileData(getProjDir()+"/Res/SPH/final.comp");
    mFinalProgram = createComputeProgram(cs.c_str());

    cs = readFileData(getProjDir()+"/Res/SPH/raymatch.comp");
    mRayMatchProgram = createComputeProgram(cs.c_str());
}

SPH::~SPH() {
    glDeleteBuffers(1, &mContextBuffer);
    glDeleteBuffers(1, &mParticleBuffer);
    glDeleteTextures(1, &mOutputTex);
    glDeleteProgram(mDisplayProgram);
    glDeleteProgram(mPrecomputeProgram);
    glDeleteProgram(mAccelerationProgram);
    glDeleteProgram(mFinalProgram);
    glDeleteProgram(mRayMatchProgram);
}

void SPH::tick(float dt) {
    updateContext(dt);
    updateBuffers();
    render();
}

void SPH::updateContext(float dt) {
    glm::vec2 offset = mInput->mousePosition - mInput->mouseLastPosition;

    if(mInput->mouseButtonHeld[1]) {
        mCamera.yaw += offset.x * 0.1f;
        mCamera.pitch -= offset.y * 0.1f;
        glm::vec3 front;
        front.x = cos(glm::radians(mCamera.yaw)) * cos(glm::radians(mCamera.pitch));
        front.y = sin(glm::radians(mCamera.pitch));
        front.z = sin(glm::radians(mCamera.yaw)) * cos(glm::radians(mCamera.pitch));
        mCamera.front = glm::normalize(front);
        mCamera.right = glm::normalize(glm::cross(mCamera.front, glm::vec3(0, 1, 0)));
        mCamera.up = glm::normalize(glm::cross(mCamera.right, mCamera.front));
    }

    if (mInput->mouseScrollWheel != 0) {
        mCamera.position += mCamera.front * mInput->mouseScrollWheel * 0.1f;
    }

    mContext.cameraPosition = mCamera.position;
    mContext.cameraRight = mCamera.right;
    mContext.cameraUp = mCamera.up;
    mContext.cameraFront = mCamera.front;
    mContext.particleCount = mParticles.size();
    mContext.deltaTime = dt;

    glBindBuffer(GL_UNIFORM_BUFFER, mContextBuffer);
    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(mContext), &mContext);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void SPH::updateBuffers() {

}

static unsigned divup(unsigned a, unsigned b) {
    return (a + b - 1)/b;
}

void SPH::render() {
    // precompute
    glUseProgram(mPrecomputeProgram);
    glBindBufferBase(GL_UNIFORM_BUFFER, 0, mContextBuffer);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, mParticleBuffer);
    glDispatchCompute(256 * divup(mParticles.size(), 256), 1, 1);
    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

    // acceleration
    glUseProgram(mAccelerationProgram);
    glBindBufferBase(GL_UNIFORM_BUFFER, 0, mContextBuffer);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, mParticleBuffer);
    glDispatchCompute(256 * divup(mParticles.size(), 256), 1, 1);
    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

    // final
    glUseProgram(mFinalProgram);
    glBindBufferBase(GL_UNIFORM_BUFFER, 0, mContextBuffer);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, mParticleBuffer);
    glDispatchCompute(256 * divup(mParticles.size(), 256), 1, 1);
    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

    // ray match
    glUseProgram(mRayMatchProgram);
    glBindImageTexture(0, mOutputTex, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);
    glBindBufferBase(GL_UNIFORM_BUFFER, 1, mContextBuffer);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, mParticleBuffer);
    glDispatchCompute((GLuint)mWidth, (GLuint)mHeight, 1);
    glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

    // display
    glViewport(0, 0, mWidth, mHeight);
    glClearColor(0.3f, 0.0f, 0.8f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glUseProgram(mDisplayProgram);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, mOutputTex);
    glUniform1i(glGetUniformLocation(mDisplayProgram, "u_texture"), 0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), &quadVertices[0]);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), &quadTexcoord[0]);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
}