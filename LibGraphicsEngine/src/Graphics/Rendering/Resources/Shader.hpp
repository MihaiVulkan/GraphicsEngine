#ifndef GRAPHICS_RENDERING_RESOURCES_SHADER_HPP
#define GRAPHICS_RENDERING_RESOURCES_SHADER_HPP

#include "Resource.hpp"
#include <string>

namespace GraphicsEngine
{
	namespace Graphics
	{
		// Shader - used to store shader data to pass to a specific Graphics API
		class Shader : public Resource
		{
			GE_RTTI(GraphicsEngine::Graphics::Shader)

		public:
			enum class Type : uint8_t
			{
				T_VERTEX = 0,
				T_TESSELATION_CONTROL,
				T_TESSELATION_EVALUATION,
				T_GEOMETRY,
				T_FRAGMENT,
				T_ALL_GRAPHICS, // all the above shader types combined (no compute type and no extension types)
				T_COMPUTE,
				T_COUNT
			};

			Shader();
			explicit Shader(Shader::Type type, const char_t* pPath);
			virtual ~Shader();

			const Shader::Type& GetType() const;
			const std::string& GetPath() const;


		private:
			void Create(const char_t* pPath);
			void Destroy();

			std::string mPath;

			Shader::Type mType;
		};
	}
}

#endif // GRAPHICS_RENDERING_RESOURCES_SHADER_HPP