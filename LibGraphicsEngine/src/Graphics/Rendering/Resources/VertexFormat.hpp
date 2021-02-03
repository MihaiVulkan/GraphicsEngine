#ifndef GRAPHICS_RENDERING_RESOURCES_VERTEX_FORMAT_HPP
#define GRAPHICS_RENDERING_RESOURCES_VERTEX_FORMAT_HPP

#include "Graphics/Rendering/Resources/Resource.hpp"
#include <map>

namespace GraphicsEngine
{
	namespace Graphics
	{
		// VertexFormat - used to store vertex format data to pass to a specific Graphics API
		// TODO - by default we work with float data
		class VertexFormat : public Resource
		{
			GE_RTTI(GraphicsEngine::Graphics::VertexFormat)

		public:
			// VF presets
			static const VertexFormat VF_P3;
			static const VertexFormat VF_P3_N3;
			static const VertexFormat VF_P3_C3;
			static const VertexFormat VF_P3_UV2;
			//static const VertexFormat VF_P3_N3_UV2;
			//static const VertexFormat VF_P3_C3_UV2;
			//static const VertexFormat VF_P3_N3_C3_UV2;
			//static const VertexFormat VF_P3_N3_TG3;
			//static const VertexFormat VF_P3_N3_TG3_C3;
			//static const VertexFormat VF_P3_N3_TG3_UV2;
			//static const VertexFormat VF_P3_N3_TG3_C3_UV2;

			// TODO - Graphics API check for maximum number of attributes
			enum class VertexAttribute : uint8_t
			{
				GE_VA_POSITION = 0,
				GE_VA_NORMAL,
				GE_VA_TANGENT,
				GE_VA_COLOR,
				GE_VA_TEXTURE_COORD,
				GE_VA_COUNT
			};

			// NOTE! ordered_map as we want to maintain the order of attributes!
			typedef std::map<VertexFormat::VertexAttribute, uint8_t> VertexAttributeMap;

			VertexFormat();
			// TODO - maybe make the constructor variadic ?
			explicit VertexFormat(uint8_t position, uint8_t normal, uint8_t tangent, uint8_t color, uint8_t texCoord);
			explicit VertexFormat(const VertexFormat& format);
			explicit VertexFormat(VertexFormat&& format);
			virtual ~VertexFormat();

			VertexFormat& operator =(const VertexFormat& format);
			VertexFormat& operator =(VertexFormat&& format);

			uint32_t GetVertexAttributeStride(const VertexFormat::VertexAttribute& att) const;
			bool_t HasVertexAttribute(const VertexFormat::VertexAttribute& att) const;

			uint32_t GetVertexAttributeOffset(const VertexFormat::VertexAttribute& att) const;
			uint32_t GetVertexTotalStride() const;

			const VertexFormat::VertexAttributeMap& GetVertexAttributes() const;

		private:
			void Create(uint8_t position, uint8_t normal, uint8_t tangent, uint8_t color, uint8_t texCoord);
			void Copy(const VertexFormat& format);
			void Move(VertexFormat&& format);
			void Destroy();

			VertexAttributeMap mVertexAttributes;
		};
	}
}

#endif // GRAPHICS_RENDERING_RESOURCES_VERTEX_FORMAT_HPP