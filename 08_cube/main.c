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
#include <stdlib.h>
#include <math.h>
#include <GL/glew.h>
#include <GL/freeglut.h>

#include "lib/dashgl.h"

#define WIDTH 640
#define HEIGHT 480

GLuint vbo_cube_vertices;
GLuint ibo_cube_elements;
GLuint program;
GLint attribute_coord3d, attribute_v_color;
GLint uniform_mvp;

int init_resources();
void on_idle();
void on_display();
void free_resources();

int main(int argc, char* argv[]) {

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA|GLUT_ALPHA|GLUT_DOUBLE|GLUT_DEPTH);
	glutInitWindowSize(WIDTH, HEIGHT);
	glutCreateWindow("My Rotating Cube");

	GLenum glew_status = glewInit();
	if (glew_status != GLEW_OK) {
		fprintf(stderr, "Error: %s\n", glewGetErrorString(glew_status));
		return 1;
	}

	if (!GLEW_VERSION_2_0) {
		fprintf(stderr, "Error: your GPU does not support OpenGL 2.0\n");
		return 1;
	}
	
	if (!init_resources()) {
  		free_resources();
		return 1;
	}

    glutDisplayFunc(on_display);
    glutIdleFunc(on_idle);
    glEnable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glutMainLoop();

	free_resources();
	return 0;

}

int init_resources() {

  GLfloat cube_vertices[] = {
    // front
    -1.0, -1.0,  1.0, 1.0, 0.0, 0.0,
     1.0, -1.0,  1.0, 0.0, 1.0, 0.0,
     1.0,  1.0,  1.0, 0.0, 0.0, 1.0,
    -1.0,  1.0,  1.0, 1.0, 1.0, 1.0,
    -1.0, -1.0, -1.0, 1.0, 0.0, 0.0,
     1.0, -1.0, -1.0, 0.0, 1.0, 0.0,
     1.0,  1.0, -1.0, 0.0, 0.0, 1.0,
    -1.0,  1.0, -1.0, 1.0, 1.0, 1.0
  };
  glGenBuffers(1, &vbo_cube_vertices);
  glBindBuffer(GL_ARRAY_BUFFER, vbo_cube_vertices);
  glBufferData(GL_ARRAY_BUFFER, sizeof(cube_vertices), cube_vertices, GL_STATIC_DRAW);

	GLushort cube_elements[] = {
    	// front
    	0, 1, 2,
    	2, 3, 0,
    	// top
    	1, 5, 6,
    	6, 2, 1,
    	// back
    	7, 6, 5,
    	5, 4, 7,
    	// bottom
    	4, 0, 3,
    	3, 7, 4,
    	// left
    	4, 5, 1,
    	1, 0, 4,
    	// right
    	3, 2, 6,
    	6, 7, 3,
	};
	glGenBuffers(1, &ibo_cube_elements);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo_cube_elements);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cube_elements), cube_elements, GL_STATIC_DRAW);

	program = dash_create_program("shader/vertex.glsl", "shader/fragment.glsl");
	if(!program) {
		return 0;
	}

	const char* attribute_name;
	attribute_name = "coord3d";
	attribute_coord3d = glGetAttribLocation(program, attribute_name);
	if (attribute_coord3d == -1) {
		fprintf(stderr, "Could not bind attribute %s\n", attribute_name);
		return 0;
	}

	attribute_name = "v_color";
	attribute_v_color = glGetAttribLocation(program, attribute_name);
	if (attribute_v_color == -1) {
		fprintf(stderr, "Could not bind attribute %s\n", attribute_name);
		return 0;
	}

	const char* uniform_name;
	uniform_name = "mvp";
	uniform_mvp = glGetUniformLocation(program, uniform_name);
		if (uniform_mvp == -1) {
		fprintf(stderr, "Could not bind uniform %s\n", uniform_name);
		return 0;
	}

	return 1;
}

void on_idle() {
  
	float angle = glutGet(GLUT_ELAPSED_TIME) / 1000.0 * 45;
	float rad = angle * M_PI / 180.0;

	vec3 eye = { 0.0f, 2.0f, 0.0f };
	vec3 target = { 0.0f, 0.0f, -4.0f };
	vec3 axis = { 0.0f, 1.0f, 0.0f };
	vec3 t = { 0.0, 0.0, -4.0f };

	mat4 mvp, pos, rot, projection, view;
	mat4_identity(mvp);
	mat4_perspective(45.0f, 1.0f*WIDTH/HEIGHT, 0.1f, 10.0f, projection);
	mat4_look_at(eye, target, axis, view);
	mat4_translate(t, pos);
	mat4_rotate_y(rad, rot);

	mat4_multiply(mvp, projection, mvp);
	mat4_multiply(mvp, view, mvp);
	mat4_multiply(mvp, pos, mvp);
	mat4_multiply(mvp, rot, mvp);

	glUseProgram(program);
	glUniformMatrix4fv(uniform_mvp, 1, GL_FALSE, mvp);
	glutPostRedisplay();

}

void on_display() {

	glClearColor(1.0, 1.0, 1.0, 1.0);
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

	glUseProgram(program);
	glEnableVertexAttribArray(attribute_coord3d);
  
	glBindBuffer(GL_ARRAY_BUFFER, vbo_cube_vertices);
	glVertexAttribPointer(
    	attribute_coord3d, 
    	3, 
    	GL_FLOAT,  
    	GL_FALSE, 
    	sizeof(float)*6, 
    	0 
  	);

	glEnableVertexAttribArray(attribute_v_color);
	glVertexAttribPointer(
    	attribute_v_color,
    	3, 
    	GL_FLOAT,
    	GL_FALSE,
    	sizeof(float)*6,
    	(void*)(sizeof(float)*3)
  	);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo_cube_elements);
	int size;  
	glGetBufferParameteriv(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_SIZE, &size);
	glDrawElements(GL_TRIANGLES, size/sizeof(GLushort), GL_UNSIGNED_SHORT, 0);

	glDisableVertexAttribArray(attribute_coord3d);
	glDisableVertexAttribArray(attribute_v_color);
	glutSwapBuffers();

}

void free_resources() {

  glDeleteProgram(program);
  glDeleteBuffers(1, &vbo_cube_vertices);
  glDeleteBuffers(1, &ibo_cube_elements);

}



