#ifndef GRAPHICS_RENDERING_BACKENDS_VULKAN_INTERNAL_VULKAN_SAMPLER_HPP
#define GRAPHICS_RENDERING_BACKENDS_VULKAN_INTERNAL_VULKAN_SAMPLER_HPP

#include "Graphics/Rendering/Backends/Vulkan/Common/VulkanObject.hpp"

namespace GraphicsEngine
{
	namespace Graphics
	{
		class VulkanDevice;

		class VulkanSampler : public VulkanObject
		{
			GE_RTTI(GraphicsEngine::Graphics::VulkanSampler)

		public:
			typedef struct
			{
				VkFilter magFilter;
				VkFilter minFilter;
				VkSamplerMipmapMode mipmapMode;
				VkSamplerAddressMode addressModeU;
				VkSamplerAddressMode addressModeV;
				VkSamplerAddressMode addressModeW; 
				float32_t mipLodBias;
				VkBool32 anisotropyEnable;
				float32_t maxAnisotropy;
				VkBool32 compareEnable;
				VkCompareOp compareOp;
				float32_t minLod;
				float32_t maxLod;
				VkBorderColor borderColor;
				VkBool32 unnormalizedCoordinates;
			} Data;

			VulkanSampler();
			explicit VulkanSampler(VulkanDevice* pDevice,
				VkFilter magFilter, VkFilter minFilter, VkSamplerMipmapMode mipmapMode, VkSamplerAddressMode addressModeU, VkSamplerAddressMode addressModeV,
				VkSamplerAddressMode addressModeW, float32_t mipLodBias, VkBool32 anisotropyEnable, float32_t maxAnisotropy, VkBool32 compareEnable,
				VkCompareOp compareOp, float32_t minLod, float32_t maxLod, VkBorderColor borderColor, VkBool32 unnormalizedCoordinates,
				VkSamplerCreateFlags flags = 0);
			virtual ~VulkanSampler();


			const VkSampler& GetHandle() const;
			const VulkanSampler::Data& GetData() const;

		private:
			void Create(const VkSamplerCreateInfo& samplerCreateInfo);
			void Destroy();

			VulkanDevice* mpDevice;

			VkSampler mHandle;

			VulkanSampler::Data mSamplerData;


		};
	}
}

#endif // GRAPHICS_RENDERING_BACKENDS_VULKAN_INTERNAL_VULKAN_SAMPLER_HPP