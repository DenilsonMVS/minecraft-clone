
#include "renderer.hpp"

#include "utils.hpp"
#include <iostream>


static void framebuffer_size_callback(GLFWwindow * const window, const int width, const int height) {
	glViewport(0, 0, width, height);
}


static void glDebugOutput(
	const unsigned source_id,
	const unsigned type_id,
	const unsigned id,
	const unsigned severity_id,
	const int length,
	const char * const message,
	const void * const user_param
) {
	const char *source;
	const char *type;
	const char *severity;

	switch(source_id) {
    case GL_DEBUG_SOURCE_API:
        source = "API";
    	break;

    case GL_DEBUG_SOURCE_WINDOW_SYSTEM:
        source = "WINDOW SYSTEM";
        break;

    case GL_DEBUG_SOURCE_SHADER_COMPILER:
        source = "SHADER COMPILER";
        break;

    case GL_DEBUG_SOURCE_THIRD_PARTY:
        source = "THIRD PARTY";
        break;

    case GL_DEBUG_SOURCE_APPLICATION:
        source = "APPLICATION";
        break;

    case GL_DEBUG_SOURCE_OTHER:
        source = "OTHER";
        break;

    default:
        source = "UNKNOWN";
        break;
    }

    switch(type_id) {
    case GL_DEBUG_TYPE_ERROR:
        type = "ERROR";
        break;

    case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
        type = "DEPRECATED BEHAVIOR";
        break;

    case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
        type = "UDEFINED BEHAVIOR";
        break;

    case GL_DEBUG_TYPE_PORTABILITY:
        type = "PORTABILITY";
        break;

    case GL_DEBUG_TYPE_PERFORMANCE:
        type = "PERFORMANCE";
        break;

    case GL_DEBUG_TYPE_OTHER:
        type = "OTHER";
        break;

    case GL_DEBUG_TYPE_MARKER:
        type = "MARKER";
        break;

	default:
        type = "UNKNOWN";
        break;
    }

    switch(severity_id) {
    case GL_DEBUG_SEVERITY_HIGH:
        severity = "HIGH";
        break;

    case GL_DEBUG_SEVERITY_MEDIUM:
    	severity = "MEDIUM";
        break;

    case GL_DEBUG_SEVERITY_LOW:
        severity = "LOW";
        break;

    case GL_DEBUG_SEVERITY_NOTIFICATION:
        severity = "NOTIFICATION";
        break;

    default:
        severity = "UNKNOWN";
        break;
    }

	std::cout << id << ": " << type << " of " << severity << " severity, raised from " << source << ": " << message << '\n';
	if(severity_id != GL_DEBUG_SEVERITY_NOTIFICATION)
		std::cin.get();
}


Renderer::RendererConstructor::RendererConstructor() {
	det::check(glfwInit(), "Unable to initate glfw.\n");

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	#ifndef NDEBUG
	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);
	#else
	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_FALSE);
	#endif
}

Renderer::RendererConstructor::~RendererConstructor() {
	glfwTerminate();
}

Window Renderer::create_window(const glm::ivec2 dimensions, const char * const title) {
	auto window = Window(dimensions, title);
	det::check(window.w != nullptr, "Unable to create window.\n");
	glfwMakeContextCurrent(window.w);
	
	glfwSetFramebufferSizeCallback(window.w, framebuffer_size_callback);

	det::check(gladLoadGLLoader((GLADloadproc) glfwGetProcAddress), "Unable to initiate glad.\n");

	
	#ifndef NDEBUG

	glEnable(GL_DEBUG_OUTPUT);
	glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
	glDebugMessageCallback(glDebugOutput, nullptr);

	std::cout << "GL Vendor: " << glGetString(GL_RENDERER) << '\n';
	std::cout << "GL Renderer: " << glGetString(GL_VENDOR) << '\n';
	std::cout << "GL Version: " << glGetString(GL_VERSION) << '\n';
	std::cout << "GLSL Version: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << '\n';

	#endif

	return window;
}

Window Renderer::create_window(const glm::ivec2 dimensions, const char * const title, const Window &share) {
	auto window = Window(dimensions, title, share);
	det::check(window.w != nullptr, "Unable to create window.\n");
	
	glfwSetFramebufferSizeCallback(window.w, framebuffer_size_callback);

	return window;
}

void Renderer::set_clear_color(const float r, const float g, const float b) {
	glClearColor(r, g, b, 1.0f);
}

void Renderer::set_clear_color(const glm::vec3 &color) {
    glClearColor(color.r, color.g, color.b, 1.0f);
}

void Renderer::poll_events() {
	glfwPollEvents();
}

void Renderer::clear(const gl::BitField mask) {
    glClear(mask);
}

void Renderer::use_wireframe() {
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
}

void Renderer::use_filling() {
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}



Renderer::RendererConstructor Renderer::instance;
