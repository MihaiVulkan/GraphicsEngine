#ifndef GRAPHICS_RENDERING_VULKAN_INTERNAL_VULKAN_SURFACE_HPP
#define GRAPHICS_RENDERING_VULKAN_INTERNAL_VULKAN_SURFACE_HPP

#include "Graphics/Rendering/Vulkan/Common/VulkanObject.hpp"
#include <vector>

namespace GraphicsEngine
{
	namespace Graphics
	{
		class VulkanDevice;

		/*
			*Wrapper for VkSurfaceKHR object*

			Basically, encapsulates the presentation surface.
			This surface is platform dependent.Currently the Vulkan AI integrates with: Win32, Wayland(linux), MacOS, Android and some other.
			The VUlkan API offers support for this via extensions.

		*/

		class VulkanSurface : public VulkanObject
		{
			GE_RTTI(GraphicsEngine::Graphics::VulkanSurface)

		public:
			VulkanSurface();
			explicit VulkanSurface(VulkanDevice* pDevice);
			virtual ~VulkanSurface();

			const VkSurfaceKHR& GetHandle() const;

		private:
			void Create();
			void Destroy();

			VulkanDevice* mpDevice;

			VkSurfaceKHR mHandle;
		};
	}
}

#endif // GRAPHICS_RENDERING_VULKAN_INTERNAL_VULKAN_SURFACE_HPP