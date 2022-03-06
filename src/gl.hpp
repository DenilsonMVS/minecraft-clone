
#ifndef GL_LAYER_HPP
#define GL_LAYER_HPP

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <cassert>


namespace gl {

	enum class Type : unsigned {
		BYTE = GL_BYTE,
		CHAR = GL_BYTE,
		INT8 = GL_BYTE,
		UNSIGNED_BYTE = GL_UNSIGNED_BYTE,
		UNSIGNED_CHAR = GL_UNSIGNED_BYTE,
		UINT8 = GL_UNSIGNED_BYTE,
		SHORT = GL_SHORT,
		INT16 = GL_SHORT,
		UNSIGNED_SHORT = GL_UNSIGNED_SHORT,
		UINT16 = GL_UNSIGNED_SHORT,
		INT = GL_INT,
		INT32 = GL_INT,
		UNSIGNED_INT = GL_UNSIGNED_INT,
		UINT32 = GL_UNSIGNED_INT,
		FLOAT = GL_FLOAT,
		DOUBLE = GL_DOUBLE
	};

	template<typename T>
	constexpr Type get_enum();

	template<>
	constexpr Type get_enum<char>() {
		return Type::BYTE;
	}

	template<>
	constexpr Type get_enum<unsigned char>() {
		return Type::UNSIGNED_CHAR;
	}

	template<>
	constexpr Type get_enum<short>() {
		return Type::SHORT;
	}

	template<>
	constexpr Type get_enum<unsigned short>() {
		return Type::UNSIGNED_SHORT;
	}

	template<>
	constexpr Type get_enum<int>() {
		return Type::INT;
	}

	template<>
	constexpr Type get_enum<unsigned int>() {
		return Type::UNSIGNED_INT;
	}

	template<>
	constexpr Type get_enum<float>() {
		return Type::FLOAT;
	}

	template<>
	constexpr Type get_enum<double>() {
		return Type::DOUBLE;
	}


	constexpr unsigned get_size(const unsigned type) {
		switch(type) {
		case GL_BYTE:
		case GL_UNSIGNED_BYTE:
			return 1;

		case GL_SHORT:
		case GL_UNSIGNED_SHORT:
		case GL_HALF_FLOAT:
			return 2;

		case GL_INT:
		case GL_UNSIGNED_INT:
		case GL_FIXED:
		case GL_FLOAT:
			return 4;

		case GL_DOUBLE:
			return 8;

		default:
			assert(false);
			return 0;
		}
	}

	enum class Usage : unsigned {
		STREAM_DRAW = GL_STREAM_DRAW,
		STREAM_READ = GL_STREAM_READ,
		STREAM_COPY = GL_STREAM_COPY,
		STATIC_DRAW = GL_STATIC_DRAW,
		STATIC_READ = GL_STATIC_READ,
		STATIC_COPY = GL_STATIC_COPY,
		DYNAMIC_DRAW = GL_DYNAMIC_DRAW,
		DYNAMIC_READ = GL_DYNAMIC_READ,
		DYNAMIC_COPY = GL_DYNAMIC_COPY,
	};

	enum class BitField : unsigned {
		COLOR_BUFFER = GL_COLOR_BUFFER_BIT,
		DEPTH_BUFFER = GL_DEPTH_BUFFER_BIT,
		STENCIL_BUFFER = GL_STENCIL_BUFFER_BIT
	};

	BitField operator|(const BitField f1, const BitField f2);
	

	enum class Key : int {
		UNKNOWN =           -1,
		SPACE =             32,
		APOSTROPHE =        39,
		COMMA =             44,
		MINUS =             45,
		PERIOD =            46,
		SLASH =             47,
		N_0 =               48,
		N_1 =               49,
		N_2 =               50,
		N_3 =               51,
		N_4 =               52,
		N_5 =               53,
		N_6 =               54,
		N_7 =               55,
		N_8 =               56,
		N_9 =               57,
		SEMICOLON =         59,
		EQUAL =             61,
		A =                 65,
		B =                 66,
		C =                 67,
		D =                 68,
		E =                 69,
		F =                 70,
		G =                 71,
		H =                 72,
		I =                 73,
		J =                 74,
		K =                 75,
		L =                 76,
		M =                 77,
		N =                 78,
		O =                 79,
		P =                 80,
		Q =                 81,
		R =                 82,
		S =                 83,
		T =                 84,
		U =                 85,
		V =                 86,
		W =                 87,
		X =                 88,
		Y =                 89,
		Z =                 90,
		LEFT_BRACKET =      91,
		BACKSLASH =         92,
		RIGHT_BRACKET =     93,
		GRAVE_ACCENT =      96,
		WORLD_1 =           161,
		WORLD_2 =           162,
		ESCAPE =            256,
		ENTER =             257,
		TAB =               258,
		BACKSPACE =         259,
		INSERT =            260,
		DELETE =            261,
		RIGHT =             262,
		LEFT =              263,
		DOWN =              264,
		UP =                265,
		PAGE_UP =           266,
		PAGE_DOWN =         267,
		HOME =              268,
		END =               269,
		CAPS_LOCK =         280,
		SCROLL_LOCK =       281,
		NUM_LOCK =          282,
		PRINT_SCREEN =      283,
		PAUSE =             284,
		F1 =                290,
		F2 =                291,
		F3 =                292,
		F4 =                293,
		F5 =                294,
		F6 =                295,
		F7 =                296,
		F8 =                297,
		F9 =                298,
		F10 =               299,
		F11 =               300,
		F12 =               301,
		F13 =               302,
		F14 =               303,
		F15 =               304,
		F16 =               305,
		F17 =               306,
		F18 =               307,
		F19 =               308,
		F20 =               309,
		F21 =               310,
		F22 =               311,
		F23 =               312,
		F24 =               313,
		F25 =               314,
		KP_0 =              320,
		KP_1 =              321,
		KP_2 =              322,
		KP_3 =              323,
		KP_4 =              324,
		KP_5 =              325,
		KP_6 =              326,
		KP_7 =              327,
		KP_8 =              328,
		KP_9 =              329,
		KP_DECIMAL =        330,
		KP_DIVIDE =         331,
		KP_MULTIPLY =       332,
		KP_SUBTRACT =       333,
		KP_ADD =            334,
		KP_ENTER =          335,
		KP_EQUAL =          336,
		LEFT_SHIFT =        340,
		LEFT_CONTROL =      341,
		LEFT_ALT =          342,
		LEFT_SUPER =        343,
		RIGHT_SHIFT =       344,
		RIGHT_CONTROL =     345,
		RIGHT_ALT =         346,
		RIGHT_SUPER =       347,
		MENU =              348,
	};

	enum class KeyStatus : int {
		RELEASE,
		PRESS,
		REPEAT
	};

	enum class CursorMode : int {
		NORMAL = GLFW_CURSOR_NORMAL,
		HIDDEN = GLFW_CURSOR_HIDDEN,
		DISABLED = GLFW_CURSOR_DISABLED
	};

	enum class Capability : unsigned {
		BLEND = GL_BLEND,
		COLOR_LOGIC_OP = GL_COLOR_LOGIC_OP,
		CULL_FACE = GL_CULL_FACE,
		DEBUG_OUTPT = GL_DEBUG_OUTPUT,
		DEBUG_OUTPUT_SYNCHRONOUS = GL_DEBUG_OUTPUT_SYNCHRONOUS,
		DEPTH_CLAMP = GL_DEPTH_CLAMP,
		DEPTH_TEST = GL_DEPTH_TEST,
		DITHER = GL_DITHER,
		FRAMEBUFFER_SRGB = GL_FRAMEBUFFER_SRGB,
		LINE_SMOOTH = GL_LINE_SMOOTH,
		MULTISAMPLE = GL_MULTISAMPLE,
		POLYGON_OFFSET_FILL = GL_POLYGON_OFFSET_FILL,
		POLYGON_OFFSET_LINE = GL_POLYGON_OFFSET_LINE,
		POLYGON_OFFSET_POINT = GL_POLYGON_OFFSET_POINT,
		POLYGON_SMOOTH = GL_POLYGON_SMOOTH,
		PRIMITIVE_RESTART = GL_PRIMITIVE_RESTART,
		PRIMITIVE_RESTART_FIXED_INDEX = GL_PRIMITIVE_RESTART_FIXED_INDEX,
		RASTERIZER_DISCARD = GL_RASTERIZER_DISCARD,
		SAMPLE_ALPHA_TO_COVERAGE = GL_SAMPLE_ALPHA_TO_COVERAGE,
		SAMPLE_ALPHA_TO_ONE = GL_SAMPLE_ALPHA_TO_ONE,
		SAMPLE_COVERAGE = GL_SAMPLE_COVERAGE,
		SAMPLE_SHADING = GL_SAMPLE_SHADING,
		SAMPLE_MASK = GL_SAMPLE_MASK,
		SCISSOR_TEST = GL_SCISSOR_TEST,
		STENCIL_TEST = GL_STENCIL_TEST,
		TEXTURE_CUBE_MAP_SEAMLESS = GL_TEXTURE_CUBE_MAP_SEAMLESS,
		PROGRAM_POINT_SIZE = GL_PROGRAM_POINT_SIZE
	};
}

#endif
