#ifndef GRAPHICS_RENDERING_BACKENDS_VULKAN_RESOURCES_VULKAN_VERTEX_FORMAT_HPP
#define GRAPHICS_RENDERING_BACKENDS_VULKAN_RESOURCES_VULKAN_VERTEX_FORMAT_HPP

#if defined(VULKAN_RENDERER)
#include "Graphics/Rendering/Backends/Vulkan/Resources/VulkanResource.hpp"
#include "Graphics/Rendering/Resources/VertexFormat.hpp"
#include <map>

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
			// NOTE! ordered_map as we want to maintain the order of attributes!
			typedef std::map<VertexFormat::VertexAttribute, VkVertexInputAttributeDescription> InputAttributeMap;

			GADRVertexFormat();
			explicit GADRVertexFormat(Renderer* pRenderer, VertexFormat* pVertexFormat);
			virtual ~GADRVertexFormat();

			const VertexFormat::VertexInputRate& GetVertexInputRate() const;
			const GADRVertexFormat::InputAttributeMap& GetVkInputAttributes() const;
			const VkVertexInputBindingDescription& GetVkInputBinding() const;

		private:
			void Create(Renderer* pRenderer);
			void Destroy();


			VertexFormat* mpVertexFormat;

			InputAttributeMap mInputAttributeMap;

			VkVertexInputBindingDescription mInputBinding;
		};
	}
}
#endif // VULKAN_RENDERER

#endif // GRAPHICS_RENDERING_BACKENDS_VULKAN_RESOURCES_VULKAN_VERTEX_FORMAT_HPP