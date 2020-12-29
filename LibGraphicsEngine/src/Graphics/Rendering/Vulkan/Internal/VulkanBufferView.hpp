#ifndef GRAPHICS_RENDERING_VULKAN_INTERNAL_VULKAN_BUFFER_VIEW_HPP
#define GRAPHICS_RENDERING_VULKAN_INTERNAL_VULKAN_BUFFER_VIEW_HPP

#include "Graphics/Rendering/Vulkan/Common/VulkanObject.hpp"

namespace GraphicsEngine
{
	namespace Graphics
	{
		class VulkanDevice;

		/* 
			*Wrapper for VkBufferView object*

			A buffer view represents a contiguous range of a buffer and a specific format to be used to interpret the data.
			Buffer views are used to enable shaders to access buffer contents interpreted as formatted data. In order to create
			a valid buffer view, the buffer must have been created with at least one of the following usage flags:
			VK_BUFFER_USAGE_UNIFORM_TEXEL_BUFFER_BIT, VK_BUFFER_USAGE_STORAGE_TEXEL_BUFFER_BIT 
		*/
		class VulkanBufferView : public VulkanObject
		{
			GE_RTTI(GraphicsEngine::Graphics::VulkanBufferView)

		public:
			VulkanBufferView();
			explicit VulkanBufferView(VulkanDevice* pDevice, VkBuffer bufferHandle, VkFormat format, VkDeviceSize offset, VkDeviceSize range,
				VkBufferViewCreateFlags flags = 0);
			virtual ~VulkanBufferView();

			const VkBufferView& GetHandle() const;

		private:
			void Create(const VkBufferViewCreateInfo& bufferViewCreateInfo);
			void Destroy();


			VulkanDevice* mpDevice;

			VkBufferView mHandle;
		};
	}
}

#endif // GRAPHICS_RENDERING_VULKAN_INTERNAL_VULKAN_BUFFER_VIEW_HPP