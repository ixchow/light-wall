#pragma once

#include <gl.hpp>
#include <glm/glm.hpp>

#include <string>

class GLProgramUniform;

class GLProgram {
public:
	GLProgram();
	GLProgram(GLProgram const &) = delete;
	GLProgram(std::string const &vertex_shader_source, std::string const &fragment_shader_source);
	~GLProgram();

	void clear();
	void set(std::string const &vertex_shader_source, std::string const &fragment_shader_source);

	std::string vertex_shader_source;
	std::string fragment_shader_source;

	GLuint program;

	//get a uniform (for setting):
	GLProgramUniform operator[](std::string const &);

	//get index of an attribute:
	GLint operator()(std::string const &) const;
};

class GLProgramUniform {
public:
	GLProgramUniform(GLuint _program, GLuint _location) : program(_program), location(_location) { }

	void operator=(GLint const &);
	void operator=(GLfloat const &);
	void operator=(glm::vec2 const &);
	void operator=(glm::vec3 const &);
	void operator=(glm::vec4 const &);

	void operator=(glm::mat4 const &);

	GLuint program;
	GLint location;
};

class UseProgram {
private:
	void init(GLuint program) {
		glGetIntegerv(GL_CURRENT_PROGRAM, &old_program);
		glUseProgram(program);
	}
public:
	UseProgram(GLuint program) {
		init(program);
	}
	UseProgram(GLProgram &program) {
		init(program.program);
	}
	~UseProgram() {
		glUseProgram(old_program);
	}
	GLint old_program;
};

