#ifndef GRAPHICS_RENDERING_BACKENDS_VULKAN_RESOURCES_VULKAN_TEXTURE_HPP
#define GRAPHICS_RENDERING_BACKENDS_VULKAN_RESOURCES_VULKAN_TEXTURE_HPP

#include "Graphics/Rendering/Backends/Vulkan/Resources/VulkanResource.hpp"
#include "Graphics/Rendering/Resources/Texture.hpp"

namespace GraphicsEngine
{
	namespace Graphics
	{
		class Renderer;
		class VulkanDevice;
		class VulkanImage;
		class VulkanImageView;
		class VulkanSampler;
		class Texture;

		// Vulkan implementation of the Graphics API Dependent Resource
		class GADRTexture : public GADRResource
		{
			GE_RTTI(GraphicsEngine::Graphics::GADRTexture)

		public:
			GADRTexture();
			explicit GADRTexture(Renderer* pRenderer, Texture* pTexture);
			virtual ~GADRTexture();

			const VkDescriptorImageInfo& GetDescriptorInfo() const;

			//TODO add generate mipmaps functionality, see vkloader.c (ktx lib source code)

		private:
			void Create(Renderer* pRenderer);
			void Destroy();

			void UpdateDecriptorInfo();

			VulkanImage* mpVulkanImage;
			VulkanImageView* mpVulkanImageView;
			VulkanSampler* mpVulkanSampler;
			Texture* mpTexture;
			VkDescriptorImageInfo mDefaultDescriptorInfo;
		};
	}
}

#endif // GRAPHICS_RENDERING_BACKENDS_VULKAN_RESOURCES_VULKAN_TEXTURE_HPP