#include "Logging.hpp"

#include <gl.hpp>
#include "GLProgram.hpp"
#include "GLAttribStore.hpp"
#include "Patterns.h"

#include <SDL.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_precision.hpp>

#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <vector>
#include <stdio.h>

int main(int argc, char **argv) {

	glm::uvec2 window_size = glm::uvec2(640, 640);

	if (SDL_Init(SDL_INIT_TIMER | SDL_INIT_VIDEO) != 0) {
		LOG_ERROR("Could not initialize sdl: " << SDL_GetError());
		return 1;
	}

	SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 0);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
	SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 0);
	SDL_GL_SetAttribute(SDL_GL_RETAINED_BACKING, 0);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

#ifdef WINDOWS
	SDL_Window *window = SDL_CreateWindow("rainbow", 32, 32, window_size.x, window_size.y, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
#else
	SDL_Window *window = SDL_CreateWindow("rainbow", 0, 0, window_size.x, window_size.y, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
#endif
	
	if (!window) {
		LOG_ERROR("Could not create window: " << SDL_GetError());
		exit(1);
	}
	{
		int w = 0, h = 0;
		SDL_GetWindowSize(window, &w, &h);
		if (w != int(window_size.x) || h != int(window_size.y)) {
			LOG_ERROR("Error: requested " << window_size.x << "x" << window_size.y << " window, got " << w << "x" << h << " window.");
			return 1;
		}
	}

	SDL_GLContext context = SDL_GL_CreateContext(window);

	if (!context) {
		LOG_ERROR("Could not create opengl context: " << SDL_GetError());
		exit(1);
	}

#ifdef WINDOWS
	if (!init_gl_shims()) {
		LOG_ERROR("Failed to init GL shims.");
		exit(1);
	}
#endif

	LOG_INFO("OpenGL info:");
	LOG_INFO("  Vendor: " << glGetString(GL_VENDOR));
	LOG_INFO("  Renderer: " << glGetString(GL_RENDERER));
	LOG_INFO("  Version: " << glGetString(GL_VERSION));
	LOG_INFO("  Shading Language Version: " << glGetString(GL_SHADING_LANGUAGE_VERSION));

	{ //check what we got...
		int major_version, minor_version;
		SDL_GL_GetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, &major_version);
		SDL_GL_GetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, &minor_version);

		LOG_INFO("  SDL says context is version " << major_version << "." << minor_version);
	}
	{ //another way...
		GLint major_version = 0, minor_version = 0;
		glGetIntegerv(GL_MAJOR_VERSION, &major_version);
		glGetIntegerv(GL_MINOR_VERSION, &minor_version);
		LOG_INFO("  glGet says version " << major_version << "." << minor_version);
	}

	{
		int w = 0, h = 0;
		SDL_GetWindowSize(window, &w, &h);
		glViewport(0,0,w,h);
	}
	
	{
		GLint max = 0;
		glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &max);
		LOG_INFO("  Max vertex attribs: " << max);
	}
	if (SDL_GL_SetSwapInterval(1) != 0) {
		LOG_ERROR("Error setting up VSYNC " << SDL_GetError() << "; will continue, but there may be tearing (or excessive resource usage)");
	}

	
	static GLuint vertex_array_object = 0;
	if (vertex_array_object == 0) {
		glGenVertexArrays(1, &vertex_array_object);
	}
	glBindVertexArray(vertex_array_object);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	float aspect = 1.0f;

	{
		int w = 0, h = 0;
		SDL_GetWindowSize(window, &w, &h);
		if (h) {
			aspect = w / float(h);
		}
	}

	PatternInfo *pattern[2] = {&all_patterns[0], &all_patterns[0]};
	PatternState pattern_state[2];

	init_state(pattern[0], &pattern_state[0]);
	init_state(pattern[1], &pattern_state[1]);

	int16_t mix = 0; //mix between the two patterns
	int16_t mix_target = 0;
	int16_t mix_step = 0;

	uint32_t ticks_to_remix = -1U; //just run pattern[0] "forever"

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
					ticks_to_remix = 0;
				}
				if (event.type == SDL_WINDOWEVENT) {
					if (event.window.event == SDL_WINDOWEVENT_RESIZED) {
						int w = 0, h = 0;
						SDL_GetWindowSize(window, &w, &h);
						glViewport(0,0,w,h);
						if (h) {
							aspect = w / float(h);
						}
						continue;
					}
					continue;
				}
			}
		}
		if (ticks_to_remix > 0) {
			--ticks_to_remix;
		} else {
			ticks_to_remix = 60 * 5; //5 + 10 % rand());
			uint32_t pick = rand() % 100;
			//if we happen to be mixed to just one pattern, take the opportunity
			// to swap out the other one:
			if (mix == 0 || pattern[1] == NULL) {
				pattern[1] = all_patterns + (rand() % PatternCount);
				init_state(pattern[1], &pattern_state[1]);
			}
			if (mix == 0x100 || pattern[0] == NULL) {
				pattern[0] = all_patterns + (rand() % PatternCount);
				init_state(pattern[0], &pattern_state[0]);
			}
			if (pick < 40) {
				//mix to just first pattern
				mix_target = 0;
				mix_step = 1 + (rand() % 5);
			} else if (pick < 80) {
				//mix to just second pattern
				mix_target = 0x100;
				mix_step = 1 + (rand() % 5);
			} else {
				//mix to random fade between patterns.
				mix_target = (rand() % 254) + 1;
				mix_step = 1 + (rand() % 5);
			}
		}

		if (mix > mix_target) {
			mix -= mix_step;
			if (mix < mix_target) mix = mix_target;
		}
		if (mix < mix_target) {
			mix += mix_step;
			if (mix > mix_target) mix = mix_target;
		}

		++pattern_state[0].t;
		pattern[0]->update(&pattern_state[0]);

		++pattern_state[1].t;
		pattern[1]->update(&pattern_state[1]);

		glClearColor(0.0f, 0.0f, 1.0f, 0.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		{
			GLAttribStore2< Attrib2f, Attrib4ub > quads;
			{
				typedef Attrib2< glm::vec2, glm::u8vec4 > Attrib;
				std::vector< Attrib > attribs;
				static struct Px px[1];
				for (uint16_t y = 0; y < LedsY; ++y) {
					for (uint16_t x = 0; x < LedsX; ++x) {
						pattern[0]->read(&pattern_state[0], x, y, &px[0]);
						pattern[1]->read(&pattern_state[1], x, y, &px[1]);
						px[0].r = (px[1].r * mix + px[0].r * (256 - mix)) >> 8;
						px[0].g = (px[1].g * mix + px[0].g * (256 - mix)) >> 8;
						px[0].b = (px[1].b * mix + px[0].b * (256 - mix)) >> 8;
						glm::u8vec4 col(px[0].r, px[0].g, px[0].b, 0xff);
						attribs.emplace_back(glm::vec2(x,y), col);
						attribs.emplace_back(glm::vec2(x,y), col);
						attribs.emplace_back(glm::vec2(x,y+1), col);
						attribs.emplace_back(glm::vec2(x+1,y), col);
						attribs.emplace_back(glm::vec2(x+1,y+1), col);
						attribs.emplace_back(glm::vec2(x+1,y+1), col);
					}
				}
				quads.set(&attribs[0], attribs.size());
			}
			static GLProgram color_prog(
				"uniform vec2 scale;\n"
				"uniform vec2 translate;\n"
				"in vec2 Position;\n"
				"in vec4 Color;\n"
				"out vec4 color;\n"
				"void main() {\n"
				"	color = Color;\n"
				"	gl_Position = vec4(scale * (Position + translate), 0.0, 1.0);\n"
				"}\n"
			 ,
				"in lowp vec4 color;\n"
				"void main() {\n"
				"	fragColor = color;\n"
				"}\n"
			);
			assert(color_prog.program);
			static GLuint attrib_Position = color_prog("Position");
			static GLuint attrib_Color = color_prog("Color");
			{
				UseProgram use(color_prog);

				float scale = 1.0f / JugsY;
				if (scale * JugsX > aspect) {
					scale = aspect / JugsX;
				}
				scale *= 1.9f;

				color_prog["scale"] = glm::vec2(scale / PerJugX / aspect, scale / PerJugY);
				color_prog["translate"] = glm::vec2(-0.5f * LedsX,-0.5f * LedsY);

				BindAttribStore2 bind(attrib_Position, attrib_Color, &quads);

				glDrawArrays(GL_TRIANGLE_STRIP, 0, quads.count);
			}
		}

		SDL_GL_SwapWindow(window);
		
		gl_errors("main loop");
	}

	
	glBindVertexArray(0);
	glDeleteVertexArrays(1, &vertex_array_object);


	SDL_DestroyWindow(window);
	SDL_GL_DeleteContext(context);

	SDL_Quit();

	return 0;
}
