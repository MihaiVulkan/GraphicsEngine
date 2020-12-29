#ifndef GRAPHICS_RENDERING_VULKAN_RESOURCES_VULKAN_VERTEX_FORMAT_HPP
#define GRAPHICS_RENDERING_VULKAN_RESOURCES_VULKAN_VERTEX_FORMAT_HPP

#include "VulkanResource.hpp"
#include "Graphics/Rendering/Resources/VertexFormat.hpp"
#include <unordered_map>

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
			typedef std::unordered_map<VertexFormat::VertexAttribute, VkVertexInputAttributeDescription> InputAttributeMap;

			GADRVertexFormat();
			explicit GADRVertexFormat(Renderer* pRenderer, VertexFormat* pVertexFormat);
			virtual ~GADRVertexFormat();

			const GADRVertexFormat::InputAttributeMap& GetVkInputAttributes() const;

		private:
			void Create(Renderer* pRenderer);
			void Destroy();


			VertexFormat* mpVertexFormat;

			InputAttributeMap mInputAttributeMap;
		};
	}
}

#endif // GRAPHICS_RENDERING_VULKAN_RESOURCES_VULKAN_VERTEX_FORMAT_HPP