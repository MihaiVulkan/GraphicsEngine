#include "Core/AppConfig.hpp"
#include "Graphics/Rendering/Backends/Vulkan/Internal/VulkanSurface.hpp"
#include "Graphics/Rendering/Backends/Vulkan/Internal/VulkanDevice.hpp"
#include "Graphics/Rendering/Backends/Vulkan/Internal/VulkanLogicalDevice.hpp"
#include "Graphics/Rendering/Backends/Vulkan/Internal/VulkanInitializers.hpp"
#include "Graphics/Rendering/Backends/Vulkan/Internal/VulkanHelpers.hpp"
#include "Foundation/Platform/Platform.hpp"
#include <cassert>

using namespace GraphicsEngine;
using namespace GraphicsEngine::Graphics;

VulkanSurface::VulkanSurface()
	: mpDevice(nullptr)
	, mHandle(VK_NULL_HANDLE)
{}

VulkanSurface::VulkanSurface(VulkanDevice* pDevice)
	: mpDevice(pDevice)
	, mHandle(VK_NULL_HANDLE)
{
	Create();
}

VulkanSurface::~VulkanSurface()
{
	Destroy();
}

void VulkanSurface::Create()
{
	assert(mpDevice != nullptr);

	// TODO - improve this or move to Platform
#if defined(_WIN32) && defined(VULKAN_RENDERER)
	Platform::WindowWin32* pWindow = dynamic_cast<Platform::WindowWin32*>(mpDevice->GetWindow());
	assert(pWindow != nullptr);

	VkWin32SurfaceCreateInfoKHR win32SurfaceCreateInfo = {};
	win32SurfaceCreateInfo.sType = VkStructureType::VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
	win32SurfaceCreateInfo.pNext = nullptr;
	win32SurfaceCreateInfo.flags = 0;
	win32SurfaceCreateInfo.hinstance = pWindow->GetPlatformData().instance;
	win32SurfaceCreateInfo.hwnd = pWindow->GetPlatformData().handle;

	VK_CHECK_RESULT(vkCreateWin32SurfaceKHR(mpDevice->GetInstanceHandle(), &win32SurfaceCreateInfo, nullptr, &mHandle));
#else
	// other platforms
#endif // _WIN32

}

void VulkanSurface::Destroy()
{
	assert(mpDevice != nullptr);

	if (mHandle)
	{
		vkDestroySurfaceKHR(mpDevice->GetInstanceHandle(), mHandle, nullptr);
		mHandle = VK_NULL_HANDLE;
	}

	if (mpDevice)
	{
		mpDevice = nullptr;
	}
}

const VkSurfaceKHR& VulkanSurface::GetHandle() const
{
	return mHandle;
}