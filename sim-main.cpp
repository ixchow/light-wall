#include "Patterns.h"

#include <SDL.h>
#include <SDL_opengl.h>

#include <string>
#include <iostream>
#include <vector>

static inline bool gl_errors(std::string const &where) {
	GLuint err;
	bool had_errors = false;
	while ((err = glGetError()) != GL_NO_ERROR) {
		std::cerr << "(in " << where << ") OpenGL error ";
		if (err == GL_INVALID_ENUM) {
			std::cerr << "INVALID_ENUM";
		} else if (err == GL_INVALID_VALUE) {
			std::cerr << "INVALID_VALUE";
		} else if (err == GL_INVALID_OPERATION) {
			std::cerr << "INVALID_OPERATION";
		} else if (err == GL_INVALID_FRAMEBUFFER_OPERATION) {
			std::cerr << "INVALID_FRAMEBUFFER_OPERATION";
		} else if (err == GL_OUT_OF_MEMORY) {
			std::cerr << "OUT_OF_MEMORY";
#ifndef USE_GLES
		} else if (err == GL_STACK_UNDERFLOW) {
			std::cerr << "STACK_UNDERFLOW";
		} else if (err == GL_STACK_OVERFLOW) {
			std::cerr << "STACK_OVERFLOW";
#endif //USE_GLES
		} else {
			std::cerr << "0x" << std::hex << err;
		}
		std::cerr << "\n";
		had_errors = true;
	}
	if (had_errors) {
		std::cerr.flush();
	}
	return had_errors;
}

int main(int argc, char **argv) {
	if (SDL_Init(SDL_INIT_TIMER | SDL_INIT_VIDEO) != 0) {
		std::cerr << "Could not initialize sdl: " << SDL_GetError() << std::endl;
		return 1;
	}

	SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 0);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 0);
	SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 0);
	SDL_GL_SetAttribute(SDL_GL_RETAINED_BACKING, 0);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

	SDL_Window *window = SDL_CreateWindow("wall", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, JugsX * 16, JugsY * 16, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
	
	if (!window) {
		std::cerr << "Could not create window: " << SDL_GetError() << std::endl;
		return 1;
	}

	float aspect;
	int window_width, window_height;
	SDL_GetWindowSize(window, &window_width, &window_height);
	aspect = window_width / float(window_height);

	SDL_GLContext context = SDL_GL_CreateContext(window);

	if (!context) {
		std::cout << "Could not create opengl context: " << SDL_GetError() << std::endl;
		return 1;
	}

	{ //check what we got...
		int major_version, minor_version;
		SDL_GL_GetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, &major_version);
		SDL_GL_GetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, &minor_version);

		std::cout << "  SDL says context is version " << major_version << "." << minor_version << std::endl;
	}
	{ //another way...
		GLint major_version = 0, minor_version = 0;
		glGetIntegerv(GL_MAJOR_VERSION, &major_version);
		glGetIntegerv(GL_MINOR_VERSION, &minor_version);
		std::cout << "  glGet says version " << major_version << "." << minor_version << std::endl;
	}

	glViewport(0,0,window_width, window_height);
	
	if (SDL_GL_SetSwapInterval(1) != 0) {
		std::cout << "Error setting up VSYNC " << SDL_GetError() << "; will continue, but there may be tearing (or excessive resource usage)" << std::endl;
	}
	
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glEnable(GL_TEXTURE_2D);

	std::vector< uint8_t > led_buffer(LedsX * LedsY * 3, 0);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glBindTexture(GL_TEXTURE_2D, 1);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, LedsX, LedsY, 0, GL_RGB, GL_UNSIGNED_BYTE, &led_buffer[0]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	gl_errors("init");

	Pattern *pattern = NULL;

	bool quit_flag = false;
	while (!quit_flag) {
		{ //events:
			static SDL_Event event;
			while (SDL_PollEvent(&event)) {
				if (event.type == SDL_QUIT) {
					quit_flag = true;
					continue;
				}
				if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_SPACE) {
					delete pattern;
					pattern = NULL;
				}
				if (event.type == SDL_WINDOWEVENT) {
					if (event.window.event == SDL_WINDOWEVENT_RESIZED) {
						SDL_GetWindowSize(window, &window_width, &window_height);
						glViewport(0,0,window_width,window_height);
						aspect = window_width / float(window_height);
						continue;
					}
					continue;
				}
			}
		}

		//TODO: pattern update
		//TODO: pattern draw(led_buffer)
		for (auto c = led_buffer.begin(); c != led_buffer.end(); ++c) {
			*c = rand();
		}

		if (pattern == NULL) {
			pattern = all_patterns[rand() % PatternCount]();
		}
		pattern->tick(&led_buffer[0]);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, LedsX, LedsY, 0, GL_RGB, GL_UNSIGNED_BYTE, &led_buffer[0]);

		glClearColor(0.0f, 0.0f, 1.0f, 0.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		float h = JugsY + 1;
		float w = JugsX + 2;
		float s = 1.0f;
		if (s * h > 1.0f) {
			s = 1.0f / h;
		}
		if (s * w > aspect) {
			s = aspect / w;
		}

		glLoadIdentity();
		glScalef(s / aspect, s, 1.0f);

		glBegin(GL_TRIANGLE_STRIP);
		glTexCoord2f(0.0f, 0.0f); glVertex2f(-float(JugsX),-float(JugsY));
		glTexCoord2f(0.0f, 1.0f); glVertex2f(-float(JugsX), float(JugsY));
		glTexCoord2f(1.0f, 0.0f); glVertex2f( float(JugsX),-float(JugsY));
		glTexCoord2f(1.0f, 1.0f); glVertex2f( float(JugsX), float(JugsY));
		glEnd();


		SDL_GL_SwapWindow(window);
		
		gl_errors("main loop");
	}


	SDL_DestroyWindow(window);
	SDL_GL_DeleteContext(context);

	SDL_Quit();

	return 0;
}
