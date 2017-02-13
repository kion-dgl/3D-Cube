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
#include <math.h>
#include <GL/glew.h>
#include <GL/freeglut.h>
#include "lib/dashgl.h"

GLuint program;
GLint attribute_coord2d, attribute_v_color, uniform_mvp;
GLuint vbo_triangle;

int init_resources();
void on_display();
void free_resources();
void on_idle();

int main(int argc, char *argv[]) {

	glutInit(&argc, argv);
	glutInitContextVersion(2, 0);
	glutInitDisplayMode(GLUT_RGBA|GLUT_DOUBLE|GLUT_DEPTH);
	glutInitWindowSize(640, 480);
	glutCreateWindow("OpenGL Basics");

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
	glutIdleFunc(on_idle);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glutMainLoop();
	
	free_resources();

	return 0;
}

int init_resources() {
	
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);

	GLfloat triangle_vertices[] = {
		 0.0,  0.5, 1.0, 1.0, 0.0,
		-0.5, -0.5, 0.0, 0.0, 1.0,
		 0.5, -0.5, 1.0, 0.0, 0.0
	};

	glGenBuffers(1, &vbo_triangle);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_triangle);
	glBufferData(
		GL_ARRAY_BUFFER,
		sizeof(triangle_vertices),
		triangle_vertices,
		GL_STATIC_DRAW
	);

	program = dash_create_program("shader/vertex.glsl", "shader/fragment.glsl");

	const char *attribute_name = "coord2d";
	attribute_coord2d = glGetAttribLocation(program, attribute_name);
	if(attribute_coord2d == -1) {
		fprintf(stderr, "Could not bind attribute %s\n", attribute_name);
		return 0;
	}
	
	attribute_name = "v_color";
	attribute_v_color = glGetAttribLocation(program, attribute_name);
	if(attribute_v_color == -1) {
		fprintf(stderr, "Could not bind attribute %s\n", attribute_name);
		return 0;
	}

	const char *uniform_name = "mvp";
	uniform_mvp = glGetUniformLocation(program, uniform_name);
	if(uniform_mvp == -1) {
		fprintf(stderr, "Could not bind uniform %s\n", uniform_name);
		return 0;
	}
	
	return 1;

}

void on_display() {

	glClear(GL_COLOR_BUFFER_BIT);
	
	glUseProgram(program);
	glEnableVertexAttribArray(attribute_coord2d);
	glEnableVertexAttribArray(attribute_v_color);

	glVertexAttribPointer(
		attribute_coord2d,
		2,
		GL_FLOAT,
		GL_FALSE,
		sizeof(float) * 5,
		0
	);

	glVertexAttribPointer(
		attribute_v_color,
		3,
		GL_FLOAT,
		GL_FALSE,
		sizeof(float) * 5,
		(void*)(sizeof(float) * 2)
	);
	
	glDrawArrays(GL_TRIANGLES, 0, 3);
	glDisableVertexAttribArray(attribute_coord2d);
	glDisableVertexAttribArray(attribute_v_color);

	glutSwapBuffers();

}

void on_idle() {
	
	float time = (glutGet(GLUT_ELAPSED_TIME) / 1000.0);
	float x = sinf(time * M_PI / 2.5);
	float rad = time * 45.0 * M_PI / 180.0;

	mat4 mvp, pos, rot;
	vec3 t = { x, 0.0f, 0.0f };
	mat4_identity(mvp);
	mat4_rotate_z(rad, rot);
	mat4_translate(t, pos); 
	mat4_multiply(mvp, pos, mvp);
	mat4_multiply(mvp, rot, mvp);

	glUseProgram(program);
	glUniformMatrix4fv(uniform_mvp, 1, GL_FALSE, mvp);
	glutPostRedisplay();

}

void free_resources() {
	
	glDeleteProgram(program);
	glDeleteBuffers(1, &vbo_triangle);

}
