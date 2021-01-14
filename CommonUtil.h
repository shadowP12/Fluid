#ifndef COMMONUTIL_H
#define COMMONUTIL_H

#include <string>
#include <vector>
#include <string.h>
#include <stdarg.h>
#include "glm.hpp"

#define LOGE(...)                           \
	do                                            \
	{                                             \
		fprintf(stderr, "[ERROR]: " __VA_ARGS__); \
		fflush(stderr);                           \
	} while (false)

#define LOGW(...)                          \
	do                                           \
	{                                            \
		fprintf(stderr, "[WARN]: " __VA_ARGS__); \
		fflush(stderr);                          \
	} while (false)

#define LOGI(...)                          \
	do                                           \
	{                                            \
		fprintf(stderr, "[INFO]: " __VA_ARGS__); \
		fflush(stderr);                          \
	} while (false)

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

struct Input {
    float mouseScrollWheel;
    bool mouseButtonHeld[3];
    bool mouseButtonDown[3];
    bool mouseButtonUp[3];
    glm::vec2 mousePosition;
    glm::vec2 mouseLastPosition;
};

std::string getProjDir();

std::string readProjFileData(const std::string& path);

std::string readFileData(const std::string& path);

#endif
