#pragma once
#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>

const char* PezInitialize(int width, int height); // receive window size and return window title
void PezRender();                                 // draw scene (Pez swaps the backbuffer for you)
void PezUpdate(unsigned int milliseconds);        // receive elapsed time (e.g., update physics)
void PezHandleMouse(int x, int y, int action);    // handle mouse action: PEZ_DOWN, PEZ_UP, or PEZ_MOVE

#define PEZ_VIEWPORT_WIDTH 853
#define PEZ_VIEWPORT_HEIGHT 480
#define PEZ_ENABLE_MULTISAMPLING 0
#define PEZ_VERTICAL_SYNC 1

#if defined(__APPLE__)
#define PEZ_GL_VERSION_TOKEN "GL3"
#define PEZ_FORWARD_COMPATIBLE_GL 0
#else
#define PEZ_GL_VERSION_TOKEN "GL3"
#define PEZ_FORWARD_COMPATIBLE_GL 1
#endif

enum {PEZ_DOWN, PEZ_UP, PEZ_MOVE};
#define TwoPi (6.28318531f)
#define Pi (3.14159265f)
#define countof(A) (sizeof(A) / sizeof(A[0]))

void PezDebugString(const char* pStr, ...);
void PezDebugStringW(const wchar_t* pStr, ...);
void PezFatalError(const char* pStr, ...);
void PezFatalErrorW(const wchar_t* pStr, ...);
#define PezCheckCondition(A, ...) if (!(A)) { PezFatalError(__VA_ARGS__); }
#define PezCheckConditionW(A, ...) if (!(A)) { PezFatalErrorW(__VA_ARGS__); }

#ifdef __cplusplus
}
#endif
