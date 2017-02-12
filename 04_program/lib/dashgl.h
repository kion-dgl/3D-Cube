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

#ifndef DASHGL_UTILS
#define DASHGL_UTILS

	/**********************************************************************/
	/** Shader Utilities                                                 **/	
	/**********************************************************************/

	GLuint dash_create_shader(const char *filename, GLenum type);
	void dash_print_log(GLuint object);
	GLuint dash_create_program(const char *vertex, const char *fragment);

#endif
