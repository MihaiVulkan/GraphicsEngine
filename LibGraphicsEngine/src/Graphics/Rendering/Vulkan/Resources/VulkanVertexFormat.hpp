#ifndef GRAPHICS_RENDERING_VULKAN_RESOURCES_VERTEX_FORMAT_HPP
#define GRAPHICS_RENDERING_VULKAN_RESOURCES_VERTEX_FORMAT_HPP

#include "VulkanResource.hpp"
#include "Graphics/Rendering/Resources/VertexFormat.hpp"
#include <vector>

namespace GraphicsEngine
{
	namespace Graphics
	{
		class Renderer;

		// Vulkan implementation of the Graphics API Dependent Resource
		class GADRVertexFormat : public GADRResource
		{
			GE_RTTI(GraphicsEngine::Graphics::GADRVertexFormat)

		public:
			GADRVertexFormat();
			explicit GADRVertexFormat(Renderer* pRenderer, VertexFormat* pVertexFormat);
			virtual ~GADRVertexFormat();

			//const VertexFormat& GetFormat() const;
			const std::vector<VkVertexInputAttributeDescription>& GetVkInputAttributes() const;

			static VkFormat VertexFormatToVulkanVertexFormat(const VertexFormat::Attribute& att);

		private:
			void Create(Renderer* pRenderer);
			void Destroy();


			VertexFormat* mpVertexFormat;

			std::vector<VkVertexInputAttributeDescription> mInputAttributes;
		};
	}
}

#endif // GRAPHICS_RENDERING_VULKAN_RESOURCES_VERTEX_FORMAT_HPP