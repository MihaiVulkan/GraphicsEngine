#ifndef GRAPHICS_RENDERING_RESOURCES_VERTEX_FORMAT_HPP
#define GRAPHICS_RENDERING_RESOURCES_VERTEX_FORMAT_HPP

#include "Resource.hpp"
#include <unordered_map>

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
			/*static const VertexFormat VF_P3;
			static const VertexFormat VF_P3_N3;
			static const VertexFormat VF_P3_C3;
			static const VertexFormat VF_P3_UV2;
			static const VertexFormat VF_P3_N3_UV2;
			static const VertexFormat VF_P3_C3_UV2;
			static const VertexFormat VF_P3_N3_C3_UV2;
			static const VertexFormat VF_P3_N3_TG3;
			static const VertexFormat VF_P3_N3_TG3_C3;
			static const VertexFormat VF_P3_N3_TG3_UV2;
			static const VertexFormat VF_P3_N3_TG3_C3_UV2;*/

			// TODO - Graphics API check for maximum number of attributes
			enum class Attribute : uint8_t
			{
				POSITION = 0,
				NORMAL,
				TANGENT,
				COLOR,
				TEXTURE_COORD,
				COUNT
			};

			typedef std::unordered_map<VertexFormat::Attribute, uint8_t, std::hash<VertexFormat::Attribute>> AttributeMap;

			VertexFormat();
			// TODO - maybe make the constructor variadic ?
			explicit VertexFormat(uint8_t position, uint8_t normal, uint8_t tangent, uint8_t color, uint8_t texCoord);
			explicit VertexFormat(const VertexFormat& format);
			explicit VertexFormat(VertexFormat&& format);
			virtual ~VertexFormat();

			VertexFormat& operator =(const VertexFormat& format);
			VertexFormat& operator =(VertexFormat&& format);

			const uint8_t& GetAttributeStride(const VertexFormat::Attribute& att) const;
			bool_t HasAttribute(const VertexFormat::Attribute& att) const;

			uint32_t GetAttributeOffset(const VertexFormat::Attribute& att) const;
			uint32_t GetTotalStride() const;

			const VertexFormat::AttributeMap& GetAttributes() const;

		private:
			void Create(uint8_t position, uint8_t normal, uint8_t tangent, uint8_t color, uint8_t texCoord);
			void Copy(const VertexFormat& format);
			void Move(VertexFormat&& format);
			void Destroy();

			AttributeMap mAttributes;
		};
	}
}

#endif // GRAPHICS_RENDERING_RESOURCES_VERTEX_FORMAT_HPP