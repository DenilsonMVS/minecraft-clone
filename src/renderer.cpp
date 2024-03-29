
#include "renderer.hpp"

#include "utils.hpp"

#include <iostream>
#include <vector>
#include <array>


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


Window Renderer::create_window(const glm::ivec2 dimensions, const char * const title) {
	auto window = Window(dimensions, title);
	det::check(window.w != nullptr, "Unable to create window.\n");
	glfwMakeContextCurrent(window.w);
	
	glfwSetFramebufferSizeCallback(window.w, framebuffer_size_callback);
	glfwSetScrollCallback(window.w, scroll_callback);

	det::check(gladLoadGLLoader((GLADloadproc) glfwGetProcAddress), "Unable to initiate glad.\n");

	
	#ifndef NDEBUG

	glEnable((unsigned) gl::Capability::DEBUG_OUTPT);
	glEnable((unsigned) gl::Capability::DEBUG_OUTPUT_SYNCHRONOUS);
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
	glfwSetScrollCallback(window.w, scroll_callback);

	return window;
}

void Renderer::scroll_callback(GLFWwindow * const window, const double d_x, const double d_y) {
	scroll_accumulator += d_y + d_x;
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
	glClear((unsigned) mask);
}

void Renderer::use_wireframe() {
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
}

void Renderer::use_filling() {
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

void Renderer::enable(const gl::Capability cap) {
	glEnable((unsigned) cap);
}

void Renderer::enable(const gl::Capability cap, const int index) {
	glEnablei((unsigned) cap, index);
}

void Renderer::disable(const gl::Capability cap) {
	glDisable((unsigned) cap);
}

void Renderer::disable(const gl::Capability cap, const int index) {
	glDisablei((unsigned) cap, index);
}

bool Renderer::is_enabled(const gl::Capability cap) {
	return glIsEnabled((unsigned) cap);
}

bool Renderer::is_enabled(const gl::Capability cap, const int index) {
	return glIsEnabledi((unsigned) cap, index);
}

void Renderer::set_blend_function(const gl::BlendFunc s, const gl::BlendFunc b) {
	glBlendFunc((unsigned) s, (unsigned) b);
}

double Renderer::get_time() {
	return glfwGetTime();
}

float Renderer::get_scroll() {
	return scroll_accumulator;
}

void Renderer::set_max_drawn_quads(const unsigned quantity) {
	auto indices = std::vector<unsigned>(quantity * num_indices_per_face);
	
	static const std::array<unsigned, num_indices_per_face> indices_base = {{
		2, 1, 0,
		2, 3, 1
	}};

	for(unsigned i = 0; i < quantity; i++)
		for(unsigned j = 0; j < num_indices_per_face; j++)
			indices[i * num_indices_per_face + j] = i * num_vertices_per_face + indices_base[j];
	
	
	this->face_indices = IndexBuffer<unsigned>(indices);
}

void Renderer::draw_quads(const unsigned quantity, const gl::DrawMode mode) const {
	this->face_indices.draw(quantity * num_indices_per_face, mode);
}

Renderer::Initializer::Initializer() {
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

Renderer::Initializer::~Initializer() {
	glfwTerminate();
}

float Renderer::scroll_accumulator = 0;
