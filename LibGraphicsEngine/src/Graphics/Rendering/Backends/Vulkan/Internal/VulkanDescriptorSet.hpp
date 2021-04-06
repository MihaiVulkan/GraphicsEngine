#ifndef GRAPHICS_RENDERING_BACKENDS_VULKAN_INTERNAL_VULKAN_DESCRIPTOR_SET_HPP
#define GRAPHICS_RENDERING_BACKENDS_VULKAN_INTERNAL_VULKAN_DESCRIPTOR_SET_HPP

#include "Graphics/Rendering/Backends/Vulkan/Common/VulkanObject.hpp"
#include <vector>

namespace GraphicsEngine
{
	namespace Graphics
	{
		class VulkanDevice;
		class VulkanDescriptorPool;
		class VulkanDescriptorSetLayout;

		/*
			*Wrapper for VkDescriptorSet object*

			A descriptor is an opaque data structure representing a shader resource such as a buffer, buffer view, image view,
			sampler, or combined image sampler. Descriptors are organised into descriptor sets, which are bound during command 
			recording for use in subsequent draw commands. The arrangement of content in each descriptor set is determined by a
			descriptor set layout, which determines what descriptors can be stored within it. The sequence of descriptor set
			layouts that can be used by a pipeline is specified in a pipeline layout. Each pipeline object can use up to 
			maxBoundDescriptorSets (see Limits) descriptor sets.

			Shaders access resources via variables decorated with a descriptor set and binding number that link them to a descriptor 
			in a descriptor set.

			Descriptors are grouped together into descriptor set objects. A descriptor set object is an opaque object containing storage
			for a set of descriptors, where the types and number of descriptors is defined by a descriptor set layout. The layout object
			may be used to define the association of each descriptor binding with memory or other implementation resources. The layout 
			is used both for determining the resources that need to be associated with the descriptor set, and determining the interface 
			between shader stages and shader resources.
		*/
		class VulkanDescriptorSet : public VulkanObject
		{
			GE_RTTI(GraphicsEngine::Graphics::VulkanDescriptorSet)

		public:
			VulkanDescriptorSet();
			explicit VulkanDescriptorSet(VulkanDevice* pDevice, VulkanDescriptorPool* pDescriptorPool, uint32_t id, const std::vector<VulkanDescriptorSetLayout*>& layouts);
			virtual ~VulkanDescriptorSet();

			void Update(const std::vector<VkWriteDescriptorSet>& writeSet, const std::vector<VkCopyDescriptorSet>& copySet);

			const uint32_t& GetId() const;
			const VkDescriptorSet& GetHandle() const;
			const std::vector<VulkanDescriptorSetLayout*>& GetLayouts() const;

		private:
			void Create(const std::vector<VulkanDescriptorSetLayout*>& setLayouts);
			void Destroy();

			VulkanDevice* mpDevice;
			VulkanDescriptorPool* mpDescriptorPool;

			uint32_t mId;
			VkDescriptorSet mHandle;
			std::vector<VulkanDescriptorSetLayout*> mLayouts;
		};
	}
}

#endif // GRAPHICS_RENDERING_BACKENDS_VULKAN_INTERNAL_VULKAN_DESCRIPTOR_SET_HPP