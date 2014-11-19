#ifndef CONTROL_H
#define CONTROL_H

#define _USE_MATH_DEFINES
#include <math.h>
#include <GLFW/glfw3.h>

#include "util/linmath.h"

extern GLFWwindow* window; // Defined in main.c

void calc_matrices(mat4x4* M);

#endif
