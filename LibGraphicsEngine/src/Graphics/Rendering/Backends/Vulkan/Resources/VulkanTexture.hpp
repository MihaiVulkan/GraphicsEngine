#ifndef GRAPHICS_RENDERING_BACKENDS_VULKAN_RESOURCES_VULKAN_TEXTURE_HPP
#define GRAPHICS_RENDERING_BACKENDS_VULKAN_RESOURCES_VULKAN_TEXTURE_HPP

#if defined(VULKAN_RENDERER)
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

			Texture* GetTexture();
			VulkanImage* GetVkImage();
			VulkanImageView* GetVkImageView();
			VulkanSampler* GetVkSampler();

			const VkDescriptorImageInfo& GetVkDescriptorInfo() const;

			//TODO add generate mipmaps functionality, see vkloader.c (ktx lib source code)

		private:
			void Create(Renderer* pRenderer);
			void Destroy();

			void UpdateDecriptorInfo();

			VulkanImage* mpVulkanImage;
			VulkanImageView* mpVulkanImageView;
			VulkanSampler* mpVulkanSampler;
			VkDescriptorImageInfo mVulkanDescriptorInfo;
			Texture* mpTexture;
		};
	}
}
#endif // VULKAN_RENDERER

#endif // GRAPHICS_RENDERING_BACKENDS_VULKAN_RESOURCES_VULKAN_TEXTURE_HPP