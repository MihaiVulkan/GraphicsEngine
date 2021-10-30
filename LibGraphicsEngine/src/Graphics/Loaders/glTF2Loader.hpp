#ifndef GRAPHICS_LOADERS_GLTF2_LOADER_HPP
#define GRAPHICS_LOADERS_GLTF2_LOADER_HPP

#include "Foundation/Object.hpp"
#include "glm/vec2.hpp"
#include "glm/vec3.hpp"
#include "glm/vec4.hpp"
#include <string>
#include <vector>
#include <functional>

namespace GraphicsEngine
{
	namespace Graphics
	{
		/*
			A simple glTF 2.0 model/scene loader.
			No support yet for: skins, animations
			glTF 2.0 spec here: https://github.com/KhronosGroup/glTF/blob/master/specification/2.0/README.md
		*/
		class glTF2Loader : public Object
		{
			GE_RTTI(GraphicsEngine::Graphics::glTF2Loader)

		public:
			enum FileLoadingFlags {
				None = 0x00000000,
				PreTransformVertices = 0x00000001,
				PreMultiplyVertexColors = 0x00000002
			};

			/*
			Attribute order convention:
			POSITION
			NORMAL
			TANGENT
			COLOR
			UV
			...
			*/

			struct VertexAttributes
			{
				VertexAttributes()
					: pos(0), normal(0), tangent(0), color(0), uv(0)
				{}

				uint32_t size()
				{
					return (pos + normal + tangent + color + uv);
				}

				uint32_t posOffset()
				{
					return 0;
				}

				uint32_t normalOffset()
				{
					return pos;
				}

				uint32_t tangentOffset()
				{
					return pos + normal;
				}

				uint32_t colorOffset()
				{
					return pos + normal + tangent;
				}


				uint32_t uvOffset()
				{
					return pos + normal + tangent + color;
				}

				uint32_t pos;
				uint32_t normal;
				uint32_t tangent;
				uint32_t color;
				uint32_t uv;
			};

			glTF2Loader();
			explicit glTF2Loader(const std::string& filePath, uint32_t loadingFlags = glTF2Loader::FileLoadingFlags::None);
			virtual ~glTF2Loader();

			void Draw(std::function<void(uint32_t indexCount, uint32_t firstIndex)> onDrawCB);

			const glTF2Loader::VertexAttributes& GetVertexAttributes() const;
			const std::vector<float32_t>& GetVertexBuffer() const;
			const std::vector<uint32_t>& GetIndexBuffer() const;

		private:
			NO_COPY_NO_MOVE_CLASS(glTF2Loader)

			struct Impl;
			Impl* mpImpl;
		};
	}
}

#endif // GRAPHICS_LOADERS_GLTF2_LOADER_HPP