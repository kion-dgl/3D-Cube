/*
    This file is part of DashGL.com OpenGL 2.0 Introduction

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
#include <GL/glew.h>
#include <GL/freeglut.h>
#include "lib/dashgl.h"

GLuint program;
GLint attribute_coord2d;

int init_resources();
void on_display();
void free_resources();

int main(int argc, char *argv[]) {

	glutInit(&argc, argv);
	glutInitContextVersion(2, 0);
	glutInitDisplayMode(GLUT_RGBA|GLUT_DOUBLE|GLUT_DEPTH);
	glutInitWindowSize(640, 480);
	glutCreateWindow("OpenGL Introduction");

	GLenum glew_status = glewInit();
	if(glew_status != GLEW_OK) {
		fprintf(stderr, "Error: %s\n", glewGetErrorString(glew_status));
		return 1;
	}

	if(!init_resources()) {
		fprintf(stderr, "Error: could not initialize resources\n");
		return 1;
	}

	glutDisplayFunc(on_display);
	glutMainLoop();
	
	free_resources();

	return 0;
}

int init_resources() {
	
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);

	GLuint vs = dash_create_shader("shader/vertex.glsl", GL_VERTEX_SHADER);
	GLuint fs = dash_create_shader("shader/fragment.glsl", GL_FRAGMENT_SHADER);
	if(vs == 0 || fs == 0) {
		return 0;
	}
	
	GLint link_ok;
	
	program = glCreateProgram();
	glAttachShader(program, vs);
	glAttachShader(program, fs);
	glLinkProgram(program);
	glGetProgramiv(program, GL_LINK_STATUS, &link_ok);
	if(!link_ok) {
		fprintf(stderr, "Error when linking program\n");
		return 0;
	}

	const char *attribute_name = "coord2d";
	attribute_coord2d = glGetAttribLocation(program, attribute_name);
	if(attribute_coord2d == -1) {
		fprintf(stderr, "Could not bind attribute %s\n", attribute_name);
		return 0;
	}
	
	return 1;

}

void on_display() {

	glClear(GL_COLOR_BUFFER_BIT);
	
	glUseProgram(program);
	glEnableVertexAttribArray(attribute_coord2d);
	GLfloat triangle_vertices[] = {
		 0.0,  0.5,
		-0.5, -0.5,
		 0.5, -0.5
	};

	glVertexAttribPointer(
		attribute_coord2d,
		2,
		GL_FLOAT,
		GL_FALSE,
		0,
		triangle_vertices
	);
	
	glDrawArrays(GL_TRIANGLES, 0, 3);
	glDisableVertexAttribArray(attribute_coord2d);

	glutSwapBuffers();

}

void free_resources() {
	
	glDeleteProgram(program);

}
