#include "GLProgram.hpp"

#include "Logging.hpp"

#include <glm/gtc/type_ptr.hpp>

#include <vector>
#include <cassert>
#include <iostream>

//will return 0 on failure.
static GLuint compile_shader(GLenum type, std::string const &source) {
	GLuint shader = glCreateShader(type);
	GLchar const *str = source.c_str();
	GLint length = source.size();
	glShaderSource(shader, 1, &str, &length);
	glCompileShader(shader);
	GLint compile_status = GL_FALSE;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &compile_status);
	if (compile_status != GL_TRUE) {
		//compile failed; drats.
		GLint info_log_length = 0;
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &info_log_length);
		std::vector< GLchar > info_log(info_log_length, 0);
		GLsizei length = 0;
		glGetShaderInfoLog(shader, info_log.size(), &length, &info_log[0]);
		assert(length >= 0 && size_t(length) < info_log.size());
		std::string out;
		for (GLsizei i = 0; i < length; ++i) {
			out += info_log[i];
		}
		std::cout << "Compiling ";
		if (type == GL_VERTEX_SHADER) {
			std::cout << "vertex";
#ifndef USE_GLES
		} else if (type == GL_GEOMETRY_SHADER) {
			std::cout << "geometry";
#endif //USE_GLES
		} else if (type == GL_FRAGMENT_SHADER) {
			std::cout << "fragment";
		} else {
			std::cout << "unknown-type (0x" << std::hex << type << ")";
		}
		std::cout << " shader fails\n" << out << "\n";
		std::cout.flush();
		glDeleteShader(shader);
		gl_errors("GLProgram::GLProgram [failed program]");
		return 0;
	} else {
		gl_errors("GLProgram::GLProgram");
		return shader;
	}
}

GLProgram::GLProgram() : program(0) {
}

GLProgram::GLProgram(std::string const &_vertex_shader_source, std::string const &_fragment_shader_source) : program(0) {
	set(_vertex_shader_source, _fragment_shader_source);
}

void GLProgram::clear() {
	if (program) {
		glDeleteProgram(program);
		program = 0;
	}
	vertex_shader_source = "";
	fragment_shader_source = "";
}

void GLProgram::set(std::string const &_vertex_shader_source, std::string const &_fragment_shader_source) {
	clear();

	vertex_shader_source = 
#ifdef USE_GLES
		"#define in attribute\n"
		"#define out varying\n"
#else
		"#version 150\n"
#endif
		+ _vertex_shader_source;
	fragment_shader_source =
#ifdef USE_GLES
		"#define in varying\n"
		"#define texture texture2D\n"
		"#define fragColor gl_FragColor\n"
#else
		"#version 150\n"
		"out vec4 fragColor;\n"
#endif
		+ _fragment_shader_source;

	program = glCreateProgram();

	GLuint vertex_shader = compile_shader(GL_VERTEX_SHADER, vertex_shader_source);
	if (vertex_shader != 0) {
		glAttachShader(program, vertex_shader);
		glDeleteShader(vertex_shader);
	}
	GLuint fragment_shader = compile_shader(GL_FRAGMENT_SHADER, fragment_shader_source);
	if (fragment_shader != 0) {
		glAttachShader(program, fragment_shader);
		glDeleteShader(fragment_shader);
	}
#ifndef USE_GLES
	//hack around what appears to be a driver bug on OSX:
	std::string bind_to_zero = "";
	if (vertex_shader_source.find(" Position") != std::string::npos) {
		bind_to_zero = "Position";
	} else if (vertex_shader_source.find(" Data") != std::string::npos) {
		bind_to_zero = "Data";
	} else {
		LOG_INFO("Warning: no obvious attrib 0 when compiling shader:\n" << vertex_shader_source);
	}
	if (bind_to_zero != "") {
		glBindAttribLocation(program, 0, bind_to_zero.c_str());
	}
#endif

	glLinkProgram(program);
	GLint link_status = GL_FALSE;
	glGetProgramiv(program, GL_LINK_STATUS, &link_status);
	if (link_status != GL_TRUE) {
		GLint info_log_length = 0;
		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &info_log_length);
		std::vector< GLchar > info_log(info_log_length, 0);
		GLsizei length = 0;
		glGetProgramInfoLog(program, info_log.size(), &length, &info_log[0]);
		assert(length >= 0 && size_t(length) < info_log.size());
		std::string out;
		for (GLsizei i = 0; i < length; ++i) {
			out += info_log[i];
		}
		std::cout << "Linking program fails:\n" << out << "\n";
		std::cout.flush();
	}
	if (link_status != GL_TRUE || vertex_shader == 0 || fragment_shader == 0) {
		glDeleteProgram(program);
		program = 0;
	} else {
#ifndef USE_GLES
		if (bind_to_zero != "") {
			GLuint loc = glGetAttribLocation(program, bind_to_zero.c_str());
			if (loc != 0) {
				std::cout << "WARNING: Bound attrib '" << bind_to_zero << "' to zero, but it ended up at " << loc << " when compiling:\n" << vertex_shader_source << std::endl;
			}
		}
#endif //USE_GLES
	}
}

GLProgram::~GLProgram() {
	clear();
}


GLProgramUniform GLProgram::operator[](std::string const &name) {
	return GLProgramUniform(program, glGetUniformLocation(program, name.c_str()));
}

GLint GLProgram::operator()(std::string const &name) const {
	return glGetAttribLocation(program, name.c_str());
}

//----------------------------------------------


void GLProgramUniform::operator=(GLint const &val) {
	UseProgram use_program(program);
	glUniform1iv(location, 1, &val);
}

void GLProgramUniform::operator=(GLfloat const &val) {
	UseProgram use_program(program);
	glUniform1fv(location, 1, &val);
}

void GLProgramUniform::operator=(glm::vec2 const &val) {
	static_assert(sizeof(glm::vec2) == 8, "vec2 is the size of two 32-bit floats");
	UseProgram use_program(program);
	glUniform2fv(location, 1, glm::value_ptr(val));
}

void GLProgramUniform::operator=(glm::vec3 const &val) {
	static_assert(sizeof(glm::vec3) == 12, "vec3 is the size of three 32-bit floats");
	UseProgram use_program(program);
	glUniform3fv(location, 1, glm::value_ptr(val));
}

void GLProgramUniform::operator=(glm::vec4 const &val) {
	static_assert(sizeof(glm::vec4) == 16, "vec4 is the size of four 32-bit floats");
	UseProgram use_program(program);
	glUniform4fv(location, 1, glm::value_ptr(val));
}

void GLProgramUniform::operator=(glm::mat4 const &val) {
	static_assert(sizeof(glm::mat4) == 64, "mat4 is the size of sixteen 32-bit floats");
	UseProgram use_program(program);
	glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(val));
}


