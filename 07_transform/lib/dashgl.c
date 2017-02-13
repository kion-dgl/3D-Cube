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

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <GL/glew.h>
#include "dashgl.h"

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
/** Matrix Utils                                                             **/
/******************************************************************************/


void mat4_identity(mat4 m) {
	
	m[M_00] = 1.0f;
	m[M_01] = 0.0f;
	m[M_02] = 0.0f;
	m[M_03] = 0.0f;
	m[M_10] = 0.0f;
	m[M_11] = 1.0f;
	m[M_12] = 0.0f;
	m[M_13] = 0.0f;
	m[M_20] = 0.0f;
	m[M_21] = 0.0f;
	m[M_22] = 1.0f;
	m[M_23] = 0.0f;
	m[M_30] = 0.0f;
	m[M_31] = 0.0f;
	m[M_32] = 0.0f;
	m[M_33] = 1.0f;

}

void mat4_copy(mat4 a, mat4 m) {
	
	m[M_00] = a[M_00];
	m[M_01] = a[M_01];
	m[M_02] = a[M_02];
	m[M_03] = a[M_03];
	m[M_10] = a[M_10];
	m[M_11] = a[M_11];
	m[M_12] = a[M_12];
	m[M_13] = a[M_13];
	m[M_20] = a[M_20];
	m[M_21] = a[M_21];
	m[M_22] = a[M_22];
	m[M_23] = a[M_23];
	m[M_30] = a[M_30];
	m[M_31] = a[M_31];
	m[M_32] = a[M_32];
	m[M_33] = a[M_33];

}

void mat4_translate(vec3 t, mat4 m) {

	m[M_00] = 1.0f;
	m[M_10] = 0.0f;
	m[M_20] = 0.0f;
	m[M_30] = 0.0f;
	
	m[M_01] = 0.0f;
	m[M_11] = 1.0f;
	m[M_21] = 0.0f;
	m[M_31] = 0.0f;
	
	m[M_02] = 0.0f;
	m[M_12] = 0.0f;
	m[M_22] = 1.0f;
	m[M_32] = 0.0f;
	
	m[M_03] = t[0];
	m[M_13] = t[1];
	m[M_23] = t[2];
	m[M_33] = 1.0f;

}


void mat4_rotate_z(float z, mat4 m) {

	m[M_00] = cos(z);
	m[M_01] =-sin(z);
	m[M_02] = 0.0f;
	m[M_03] = 0.0f;
	m[M_10] = sin(z);
	m[M_11] = cos(z);
	m[M_12] = 0.0f;
	m[M_13] = 0.0f;
	m[M_20] = 0.0f;
	m[M_21] = 0.0f;
	m[M_22] = 1.0f;
	m[M_23] = 0.0f;
	m[M_30] = 0.0f;
	m[M_31] = 0.0f;
	m[M_32] = 0.0f;
	m[M_33] = 1.0f;

}


void mat4_multiply(mat4 a, mat4 b, mat4 m) {

	mat4 tmp;

	tmp[M_00] = a[M_00]*b[M_00]+a[M_01]*b[M_10]+a[M_02]*b[M_20]+a[M_03]*b[M_30];
	tmp[M_01] = a[M_00]*b[M_01]+a[M_01]*b[M_11]+a[M_02]*b[M_21]+a[M_03]*b[M_31];
	tmp[M_02] = a[M_00]*b[M_02]+a[M_01]*b[M_12]+a[M_02]*b[M_22]+a[M_03]*b[M_32];
	tmp[M_03] = a[M_00]*b[M_03]+a[M_01]*b[M_13]+a[M_02]*b[M_23]+a[M_03]*b[M_33];
   
	tmp[M_10] = a[M_10]*b[M_00]+a[M_11]*b[M_10]+a[M_12]*b[M_20]+a[M_13]*b[M_30];
	tmp[M_11] = a[M_10]*b[M_01]+a[M_11]*b[M_11]+a[M_12]*b[M_21]+a[M_13]*b[M_31];
	tmp[M_12] = a[M_10]*b[M_02]+a[M_11]*b[M_12]+a[M_12]*b[M_22]+a[M_13]*b[M_32];
	tmp[M_13] = a[M_10]*b[M_03]+a[M_11]*b[M_13]+a[M_12]*b[M_23]+a[M_13]*b[M_33];
 
	tmp[M_20] = a[M_20]*b[M_00]+a[M_21]*b[M_10]+a[M_22]*b[M_20]+a[M_23]*b[M_30];
	tmp[M_21] = a[M_20]*b[M_01]+a[M_21]*b[M_11]+a[M_22]*b[M_21]+a[M_23]*b[M_31];
	tmp[M_22] = a[M_20]*b[M_02]+a[M_21]*b[M_12]+a[M_22]*b[M_22]+a[M_23]*b[M_32];
	tmp[M_23] = a[M_20]*b[M_03]+a[M_21]*b[M_13]+a[M_22]*b[M_23]+a[M_23]*b[M_33];
 
	tmp[M_30] = a[M_30]*b[M_00]+a[M_31]*b[M_10]+a[M_32]*b[M_20]+a[M_33]*b[M_30];
	tmp[M_31] = a[M_30]*b[M_01]+a[M_31]*b[M_11]+a[M_32]*b[M_21]+a[M_33]*b[M_31];
	tmp[M_32] = a[M_30]*b[M_02]+a[M_31]*b[M_12]+a[M_32]*b[M_22]+a[M_33]*b[M_32];
	tmp[M_33] = a[M_30]*b[M_03]+a[M_31]*b[M_13]+a[M_32]*b[M_23]+a[M_33]*b[M_33];

	mat4_copy(tmp, m);

}

/******************************************************************************/
/** End Program	                                                             **/
/******************************************************************************/
