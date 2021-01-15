#ifndef GRAPHICS_RENDERING_BACKENDS_VULKAN_INTERNAL_VULKAN_DESCRIPTOR_POOL_HPP
#define GRAPHICS_RENDERING_BACKENDS_VULKAN_INTERNAL_VULKAN_DESCRIPTOR_POOL_HPP

#include "Graphics/Rendering/Backends/Vulkan/Common/VulkanObject.hpp"
#include <vector>

namespace GraphicsEngine
{
	namespace Graphics
	{
		class VulkanDevice;

		/*
			*Wrapper for VkDescriptorPool object*

			A descriptor pool maintains a pool of descriptors, from which descriptor sets are allocated. Descriptor pools are 
			externally synchronized, meaning that the application must not allocate and/or free descriptor sets from the same 
			pool in multiple threads simultaneously.

			When a descriptor set is allocated, the initial state is largely uninitialized and all descriptors are undefined.
			Descriptors also become undefined if the underlying resource is destroyed.

			When a pool is destroyed, all descriptor sets allocated from the pool are implicitly freed and become invalid. 
			Descriptor sets allocated from a given pool do not need to be freed before destroying that descriptor pool.
		*/
		class VulkanDescriptorPool : public VulkanObject
		{
			GE_RTTI(GraphicsEngine::Graphics::VulkanDescriptorPool)

		public:
			// Note! VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT allows for each descriptor set to be freed individually
			VulkanDescriptorPool();
			explicit VulkanDescriptorPool(VulkanDevice* pDevice, uint32_t maxSets, const std::vector<VkDescriptorPoolSize>& poolSizes, 
									VkDescriptorPoolCreateFlags flags = VkDescriptorPoolCreateFlagBits::VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT);
			virtual ~VulkanDescriptorPool();

			VkResult Reset(VkDescriptorPoolResetFlags flags);

			const VkDescriptorPool& GetHandle() const;
			const std::vector<VkDescriptorPoolSize>& GetPoolSizes() const;

		private:
			void Create(const VkDescriptorPoolCreateInfo& descriptorPoolCreateInfo);
			void Destroy();

			VulkanDevice* mpDevice;

			VkDescriptorPool mHandle;
			std::vector<VkDescriptorPoolSize> mPoolSizes;
		};
	}
}

#endif // GRAPHICS_RENDERING_BACKENDS_VULKAN_INTERNAL_VULKAN_DESCRIPTOR_POOL_HPP