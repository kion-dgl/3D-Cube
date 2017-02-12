/*
    This file is part of Dash Graphics Library

    Copyright (C) 2017 Benjamin Collins

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU Affero General Public License version 2
    as published by the Free Software Foundation.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Affero General Public License for more details.

    You should have received a copy of the GNU Affero General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <stdio.h>
#include <stdlib.h>
#include <GL/glew.h>

/******************************************************************************/
/** Shader Utils                                                             **/
/******************************************************************************/

void dash_print_log(GLuint object) {

	char *log;
	GLint log_length = 0;

	if(glIsShader(object)) {
		glGetShaderiv(object, GL_INFO_LOG_LENGTH, &log_length);
	} else if(glIsProgram(object)) {
		glGetProgramiv(object, GL_INFO_LOG_LENGTH, &log_length);
	} else {
		fprintf(stderr, "Not a shader or program\n");
		return;
	}

	log = (char*)malloc(log_length);

	if(glIsShader(object)) {
		glGetShaderInfoLog(object, log_length, NULL, log);
	} else if(glIsProgram(object)) {
		glGetProgramInfoLog(object, log_length, NULL, log);
	}

	fprintf(stderr, "%s\n", log);
	free(log);

}

GLuint dash_create_shader(const char *filename, GLenum type) {

	FILE *fp;
	int file_len;
	char *source;

	fp = fopen(filename, "rb");
	if(!fp) {
		fprintf(stderr, "Could not open %s for reading\n", filename);
		return 0;
	}
	
	fseek(fp, 0, SEEK_END);
	file_len = ftell(fp);
	fseek(fp, 0, SEEK_SET);

	source = (char*)malloc(file_len + 1);
	fread(source, file_len, 1, fp);
	fclose(fp);
	source[file_len] = '\0';

	const GLchar *sources[] = {
		#ifdef GL_ES_VERSION_2_0
		"#version 100\n", //OpenGL ES 2.0
		#else
		"#version 120\n", // OpenGL 2.1
		#endif
		source
	};

	GLuint shader = glCreateShader(type);
	glShaderSource(shader, 2, sources, NULL);
	glCompileShader(shader);

	free((void*)source);

	GLint compile_ok;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &compile_ok);
	if(compile_ok == GL_FALSE) {
		fprintf(stderr, "%s: ", filename);
		dash_print_log(shader);
		glDeleteShader(shader);
		return 0;
	}

	return shader;

}

GLuint dash_create_program(const char *vertex, const char *fragment) {

	GLuint vs = dash_create_shader(vertex, GL_VERTEX_SHADER);
	GLuint fs = dash_create_shader(fragment, GL_FRAGMENT_SHADER);
	if(vs == 0 || fs == 0) {
		return 0;
	}
	
	GLint link_ok;
	
	GLuint program = glCreateProgram();
	glAttachShader(program, vs);
	glAttachShader(program, fs);
	glLinkProgram(program);
	glGetProgramiv(program, GL_LINK_STATUS, &link_ok);
	if(!link_ok) {
		fprintf(stderr, "Program Link Error: ");
		dash_print_log(program);
		return 0;
	}
	
	return program;

}

/******************************************************************************/
/** End Program                                                              **/
/******************************************************************************/
