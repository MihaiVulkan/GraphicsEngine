#include "OpenGLDebug.hpp"
#include "Foundation/Logger.hpp"
#include <string>

namespace GraphicsEngine
{
	namespace Graphics
	{
		/*
			Debug wrapper when working with OpenGL errors
		*/
		namespace OpenGLDebug
		{

			void CALLBACK DebugCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam)
			{
                std::string sourceStr;
                switch (source)
                {
                case GL_DEBUG_SOURCE_API:
                    sourceStr = "OpenGL API";
                    break;
                case GL_DEBUG_SOURCE_WINDOW_SYSTEM:
                    sourceStr = "Window System";
                    break;
                case GL_DEBUG_SOURCE_SHADER_COMPILER:
                    sourceStr = "Shader Compiler";
                    break;
                case GL_DEBUG_SOURCE_THIRD_PARTY:
                    sourceStr = "Third Party";
                    break;
                case GL_DEBUG_SOURCE_APPLICATION:
                    sourceStr = "Application";
                    break;
                case GL_DEBUG_SOURCE_OTHER:
                    sourceStr = "Other";
                    break;
                }

                std::string typeStr;
                switch (type)
                {
                case GL_DEBUG_TYPE_ERROR:
                    typeStr = "Error";
                    break;
                case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
                    typeStr = "Deprecated behavior";
                    break;
                case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
                    typeStr = "Undefined behavior";
                    break;
                case GL_DEBUG_TYPE_PORTABILITY:
                    typeStr = "Portability";
                    break;
                case GL_DEBUG_TYPE_PERFORMANCE:
                    typeStr = "Performance";
                    break;
                case GL_DEBUG_TYPE_OTHER:
                    typeStr = "Other";
                    break;
                }

                std::string severityStr;
                switch (severity)
                {
                case GL_DEBUG_SEVERITY_HIGH:
                    severityStr = "High";
                    break;
                case GL_DEBUG_SEVERITY_MEDIUM:
                    severityStr = "Medium";
                    break;
                case GL_DEBUG_SEVERITY_LOW:
                    severityStr = "Low";
                    break;
                }

                if (!severityStr.empty())
                {
                    LOG_DEBUG("Source: %s, Type: %s, Id: %d, Severity: %s, Message: %s", sourceStr.c_str(), typeStr.c_str(), id, severityStr.c_str(), message);
                }
			}
		}
	}
}