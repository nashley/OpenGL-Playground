#include "Shaders.h"

char* read_file(const char* filename) {
	FILE* input = fopen(filename, "rb");
	if (input == NULL) return NULL;

	if (fseek(input, 0, SEEK_END) == -1) return NULL;
	long size = ftell(input);
	if (size == -1) return NULL;
	if (fseek(input, 0, SEEK_SET) == -1) return NULL;

	// If using c-compiler, dont cast malloc's return value
		// (I.e., remove "(char*))
	char* content = (char*) malloc((size_t) size +1);
	if (content == NULL) return NULL;

	fread(content, 1, (size_t) size, input);
	if (ferror(input)) {
		free(content);
		return NULL;
	}

	fclose(input);
	content[size] = '\0';
	return content;
}

void print_log(GLuint object) {
	GLint log_length = 0;
	if (glIsShader(object))
		glGetShaderiv(object, GL_INFO_LOG_LENGTH, &log_length);
	else if (glIsProgram(object))
		glGetProgramiv(object, GL_INFO_LOG_LENGTH, &log_length);
	else {
		fprintf(stderr, "printlog: Not a shader or a program\n");
		return;
	}

	char* log = (char*) malloc(log_length);

	if (glIsShader(object))
		glGetShaderInfoLog(object, log_length, NULL, log);
	else if (glIsProgram(object))
		glGetProgramInfoLog(object, log_length, NULL, log);

	fprintf(stderr, "%s", log);
	free(log);
}

GLuint create_shader(const char* filename, GLenum type) {
	const GLchar* source = read_file(filename);
	if (source == NULL) {
		fprintf(stderr, "Error opening %s: ", filename); perror("");
		return 0;
	}
	GLuint res = glCreateShader(type);
	glShaderSource(res, 1, &source, NULL);
	free((void*) source);

	glCompileShader(res);
	GLint compile_ok = GL_FALSE;
	glGetShaderiv(res, GL_COMPILE_STATUS, &compile_ok);
	if (compile_ok == GL_FALSE) {
		fprintf(stderr, "%s:", filename);
		print_log(res);
		glDeleteShader(res);
		return 0;
	}

	return res;
}

GLuint load_shaders(const char* vert_fp, const char* frag_fp) {
	GLuint vs, fs;
	if ((vs = create_shader(vert_fp, GL_VERTEX_SHADER))   == 0) return 0;
	if ((fs = create_shader(frag_fp, GL_FRAGMENT_SHADER)) == 0) return 0;

	GLuint prog;
	prog = glCreateProgram();
	glAttachShader(prog, vs);
	glAttachShader(prog, fs);
	glLinkProgram(prog);
	GLint link_ok = GL_FALSE;
	glGetProgramiv(prog, GL_LINK_STATUS, &link_ok);

	if (!link_ok) {
		fprintf(stderr, "glLinkProgram:");
		print_log(prog);
		return 0;
	}

	return prog;
}