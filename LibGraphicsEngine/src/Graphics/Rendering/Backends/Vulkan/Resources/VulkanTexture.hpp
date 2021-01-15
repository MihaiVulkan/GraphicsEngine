#ifndef GRAPHICS_RENDERING_BACKENDS_VULKAN_RESOURCES_VULKAN_TEXTURE_HPP
#define GRAPHICS_RENDERING_BACKENDS_VULKAN_RESOURCES_VULKAN_TEXTURE_HPP

#include "Graphics/Rendering/Backends/Vulkan/Resources/VulkanResource.hpp"
#include "Graphics/Rendering/Resources/Texture.hpp"

namespace GraphicsEngine
{
	namespace Graphics
	{
		class Renderer;
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


		private:
			void Create(Renderer* pRenderer);
			void Destroy();

			VkImageType TypeToVulkanType(const Texture::TextureType& type) const;
			VkFormat FormatToVulkanFormat(const Texture::Format& format) const;
			VkSamplerAddressMode WrapModeToVulkanWrapMode(const Texture::WrapMode& wrapMode) const;
			VkFilter FilterModeToVulkanFilterMode(const Texture::FilterMode& filterMode) const;
			VkSamplerMipmapMode MipMapModeToVulkanMipMapMode(const Texture::MipMapMode& mipMapMode) const;

			VulkanImage* mpVulkanImage;
			VulkanImageView* mpVulkanImageView;
			VulkanSampler* mpVulkanSampler;
			Texture* mpTexture;
		};
	}
}

#endif // GRAPHICS_RENDERING_BACKENDS_VULKAN_RESOURCES_VULKAN_TEXTURE_HPP