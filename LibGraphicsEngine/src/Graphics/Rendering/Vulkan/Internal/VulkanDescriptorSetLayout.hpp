#ifndef GRAPHICS_RENDERING_VULKAN_INTERNAL_VULKAN_DESCRIPTOR_SET_LAYOUT_HPP
#define GRAPHICS_RENDERING_VULKAN_INTERNAL_VULKAN_DESCRIPTOR_SET_LAYOUT_HPP

#include "Graphics/Rendering/Vulkan/Common/VulkanObject.hpp"
#include <vector>

namespace GraphicsEngine
{
	namespace Graphics
	{
		class VulkanDevice;

		/*
			*Wrapper for VkDescriptorSetLayout object*

			A descriptor set layout object is defined by an array of zero or more descriptor bindings. Each individual descriptor 
			binding is specified by a descriptor type, a count (array size) of the number of descriptors in the binding, a set of 
			shader stages that can access the binding, and (if using immutable samplers) an array of sampler descriptors.
		*/
		class VulkanDescriptorSetLayout : public VulkanObject
		{
			GE_RTTI(GraphicsEngine::Graphics::VulkanDescriptorSetLayout)

		public:
			VulkanDescriptorSetLayout();
			explicit VulkanDescriptorSetLayout(VulkanDevice* pDevice, const std::vector<VkDescriptorSetLayoutBinding> bindings, VkDescriptorSetLayoutCreateFlags flags = 0);
			virtual ~VulkanDescriptorSetLayout();


			const VkDescriptorSetLayout& GetHandle() const;
			const std::vector<VkDescriptorSetLayoutBinding>& GetBindings() const;

		private:
			void Create(const VkDescriptorSetLayoutCreateInfo& descriptorPoolCreateInfo);
			void Destroy();

			VulkanDevice* mpDevice;

			VkDescriptorSetLayout mHandle;
			std::vector<VkDescriptorSetLayoutBinding> mBindings;
		};
	}
}

#endif // GRAPHICS_RENDERING_VULKAN_INTERNAL_VULKAN_DESCRIPTOR_SET_LAYOUT_HPP