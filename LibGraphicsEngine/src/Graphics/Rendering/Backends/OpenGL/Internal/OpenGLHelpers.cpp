#include "Graphics/Rendering/Backends/OpenGL/Internal/OpenGLHelpers.hpp"
#include "Foundation/Logger.hpp"
#include <sstream>

namespace GraphicsEngine
{
	namespace Graphics
	{
		namespace OpenGLHelpers
		{
			std::string ErrorString(GLenum errorCode)
			{
				switch (errorCode)
				{
#define STR(r) case GL_ ##r: return #r
					STR(NO_ERROR);
					STR(INVALID_ENUM);
					STR(INVALID_VALUE);
					STR(INVALID_OPERATION);
					STR(INVALID_FRAMEBUFFER_OPERATION);
					STR(STACK_OVERFLOW);
					STR(STACK_UNDERFLOW);
					STR(OUT_OF_MEMORY);
#undef STR
				default:
					return "UNKNOWN_ERROR";
				}
			}

			void ListRendererInfo()
			{
				LOG_INFO("///////////////// OpenGL Renderer Info ////////////////");
				LOG_INFO("Vendor: %s", glGetString(GL_VENDOR));
				LOG_INFO("Renderer: %s", glGetString(GL_RENDERER));
				LOG_INFO("Version: %s", glGetString(GL_VERSION));
				LOG_INFO("GLSL: %s", glGetString(GL_SHADING_LANGUAGE_VERSION));

				//LOG_INFO("GL Context:  %s", glGetIntegerv(GL_CONTEXT_PROFILE_MASK, *));

				GLint mask;
				glGetIntegerv(GL_CONTEXT_PROFILE_MASK, &mask);
				std::string glProfile("Unknown");

				if (mask & GL_CONTEXT_CORE_PROFILE_BIT)
					glProfile = "Core";
				if (mask & GL_CONTEXT_COMPATIBILITY_PROFILE_BIT)
					glProfile = "Compatibility";

				LOG_INFO("GL profile: %s", glProfile.c_str());

				LOG_INFO("");
			}

			void ListSupportedExtensions()
			{
				GLint numExtensions = 0;
				glGetIntegerv(GL_NUM_EXTENSIONS, &numExtensions);

				if (numExtensions == 0)
				{
					LOG_INFO("No GL extensions supported! You will not be able to use some featuresaof the app!");
				}

				LOG_INFO("///////////////////// Supported OpenGL Extensions ///////////////////");
				for (short i = 0; i < numExtensions; ++i)
				{
					const char_t* extension = (const char_t*)glGetStringi(GL_EXTENSIONS, i);
					LOG_INFO(extension);
				}
				LOG_INFO("");
			}

			void ListCapabilities()
			{
				LOG_INFO("///////////////////// GPU OpenGL Capabilities //////////////////////");

				GLint maxTexUnits = 0;
				//glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &maxTexUnits);
				glGetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, &maxTexUnits);
				LOG_INFO("Max Texture Units: %d", maxTexUnits);

				//// http://stackoverflow.com/questions/29707968/get-maximum-number-of-framebuffer-color-attachments
				GLint maxColorAtts = 0;
				glGetIntegerv(GL_MAX_COLOR_ATTACHMENTS, &maxColorAtts);
				LOG_INFO("Max Color Attachments: %d", maxColorAtts);

				GLint maxTextureArrayLayers = 0;
				glGetIntegerv(GL_MAX_ARRAY_TEXTURE_LAYERS, &maxTextureArrayLayers);
				LOG_INFO("Max Texture Array Layers: %d", maxTextureArrayLayers);

				GLint maxDrawBuf = 0;
				glGetIntegerv(GL_MAX_DRAW_BUFFERS, &maxDrawBuf);
				LOG_INFO("Max Draw Buffers: %d", maxDrawBuf);

				GLint maxVertexAttribs = 0;
				glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &maxVertexAttribs);
				LOG_INFO("Max Vertex Attributes: %d", maxVertexAttribs);

				GLint maxVertexOutputs = 0;
				glGetIntegerv(GL_MAX_VERTEX_OUTPUT_COMPONENTS, &maxVertexOutputs);
				LOG_INFO("Max Vertex Outputs: %d", maxVertexOutputs);

				GLint maxVertexUniforms = 0;
				glGetIntegerv(GL_MAX_VERTEX_UNIFORM_COMPONENTS, &maxVertexUniforms);
				LOG_INFO("Max Vertex Uniforms: %d", maxVertexUniforms);

				GLint maxFragmentInputs = 0;
				glGetIntegerv(GL_MAX_FRAGMENT_INPUT_COMPONENTS, &maxFragmentInputs);
				LOG_INFO("Max Fragment Inputs: %d", maxFragmentInputs);

				GLint maxFragmentUniforms = 0;
				glGetIntegerv(GL_MAX_FRAGMENT_UNIFORM_COMPONENTS, &maxFragmentUniforms);
				LOG_INFO("Max Fragment Uniforms: %d", maxFragmentUniforms);

				GLint maxFloatVaryings = 0;
				glGetIntegerv(GL_MAX_VARYING_FLOATS, &maxFloatVaryings);
				LOG_INFO("Max Float Varyings: %d", maxFloatVaryings);

				LOG_INFO("");
			}
		}
	}
}