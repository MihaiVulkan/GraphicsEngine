#include "AppConfig.hpp"
#include "VulkanSurface.hpp"
#include "VulkanDevice.hpp"
#include "VulkanLogicalDevice.hpp"
#include "VulkanInitializers.hpp"
#include "VulkanHelpers.hpp"
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
	assert(mpDevice->GetWindow() != nullptr);

	// TODO - fix this
#ifdef _WIN32
	VkWin32SurfaceCreateInfoKHR win32SurfaceCreateInfo = {};
	win32SurfaceCreateInfo.sType = VkStructureType::VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
	win32SurfaceCreateInfo.pNext = nullptr;
	win32SurfaceCreateInfo.flags = 0;
	win32SurfaceCreateInfo.hinstance = mpDevice->GetWindow()->win32.instance;
	win32SurfaceCreateInfo.hwnd = mpDevice->GetWindow()->win32.handle;

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