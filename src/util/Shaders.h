#ifndef SHADERS_H
#define SHADERS_H

#include <stdio.h>
#include <stdlib.h>

#include <glew.h>

char* read_file(const char* filename);
void print_log(GLuint object); // Display compilation errors from OpenGL shader compiler
GLuint create_shader(const char* filename, GLenum type);
GLuint load_shaders(const char* vert_fp, const char* frag_fp); // fp = filepath

#endif
