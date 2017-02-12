#ifndef DASHGL_UTILS
#define DASHGL_UTILS

	/**********************************************************************/
	/** Shader Utilities                                                 **/	
	/**********************************************************************/

	GLuint dash_create_shader(const char *filename, GLenum type);
	void dash_print_log(GLuint object);

#endif
