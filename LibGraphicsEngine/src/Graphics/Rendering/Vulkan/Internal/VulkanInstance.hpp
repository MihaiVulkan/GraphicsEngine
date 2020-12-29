#ifndef GRAPHICS_RENDERING_VULKAN_INTERNAL_VULKAN_INSTANCE_HPP
#define GRAPHICS_RENDERING_VULKAN_INTERNAL_VULKAN_INSTANCE_HPP

#include "Graphics/Rendering/Vulkan/Common/VulkanObject.hpp"
#include <vector>

namespace GraphicsEngine
{
	namespace Graphics
	{
		/*
			*Wrapper for VkInstance object*

			There is no global state in Vulkan and all per-application state is stored in a VkInstance object.
			Creating a VkInstance object initializes the Vulkan library and allows the application to pass information
			about itself to the implementation.

			When creating the VkInstance we also provide:
			- what layers we want to use (e.g. validation layers for debugging & others)
			- what extensions we want to enable (e.g. surface/platform specific surface extension support)

			*Layers - When a layer is enabled, it inserts itself into the call chain for Vulkan commands the layer is interested in. 
			Layers can be used for a variety of tasks that extend the base behavior of Vulkan beyond what is required by the specification -
			such as call logging, tracing, validation, or providing additional extensions.

			*Vulkan 1.0 supports: instance layers and device layers.
			*Starting with VulkanVersion 1.1 - both instance and device layers were grouped udner instance layers!

			*Extensions - Extensions may define new Vulkan commands, structures, and enumerants. For compilation purposes, the interfaces
			defined by registered extensions, including new structures and enumerants as well as function pointer types for new commands,
			are defined in the Khronos-supplied vulkan_core.h together with the core API. However, commands defined by extensions may not be 
			available for static linking - in which case function pointers to these commands should be queried at runtime as described in
			Command Function Pointers. Extensions may be provided by layers as well as by a Vulkan implementation.

			*Instance extensions add new instance-level functionality to the API, outside of the core specification.
			See Device extensions in the VulkanPhysicalDevice wrapper.

			*This wrapper also selects a targgeted Vulakn Version!
		*/
		class VulkanInstance : public VulkanObject
		{
			GE_RTTI(GraphicsEngine::Graphics::VulkanInstance)

		public:
			VulkanInstance();
			explicit VulkanInstance(const char_t* pTitle, bool_t enableValidation);
			virtual ~VulkanInstance();

			const VkInstance& GetHandle() const;
			const std::vector<const char_t*>& GetEnabledInstanceExtensions() const;

			void  SetEnabledInstanceExtensions(const std::vector<const char_t*>& enabledExtensions);

		private:
			void Create(const char_t* pTitle);
			void Destroy();

			VkInstance mHandle;
			std::vector<const char_t*> mEnabledInstanceExtensions;
			bool_t mEnableValidation;
		};
	}
}

#endif // GRAPHICS_RENDERING_VULKAN_INTERNAL_VULKAN_INSTANCE_HPP