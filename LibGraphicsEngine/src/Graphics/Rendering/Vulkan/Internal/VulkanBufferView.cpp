#include "VulkanBufferView.hpp"
#include "VulkanDevice.hpp"
#include "VulkanInitializers.hpp"
#include "VulkanHelpers.hpp"
#include <cassert>

using namespace GraphicsEngine;
using namespace GraphicsEngine::Graphics;

VulkanBufferView::VulkanBufferView()
	: mpDevice(nullptr)
	, mHandle(VK_NULL_HANDLE)
{}

VulkanBufferView::VulkanBufferView(VulkanDevice* pDevice, VkBuffer bufferHandle, VkFormat format, VkDeviceSize offset, VkDeviceSize range,
									VkBufferViewCreateFlags flags)
	: mpDevice(pDevice)
	, mHandle(VK_NULL_HANDLE)
{
	VkBufferViewCreateInfo bufferViewCreateInfo = VulkanInitializers::BufferViewCreateInfo(bufferHandle, format, offset, range, flags);

	Create(bufferViewCreateInfo);
}

VulkanBufferView::~VulkanBufferView()
{
	Destroy();
}

void VulkanBufferView::Create(const VkBufferViewCreateInfo& bufferViewCreateInfo)
{
	assert(mpDevice != nullptr);

	VK_CHECK_RESULT(vkCreateBufferView(mpDevice->GetDeviceHandle(), &bufferViewCreateInfo, nullptr, &mHandle));
}

void VulkanBufferView::Destroy()
{
	assert(mpDevice != nullptr);

	if (mHandle)
	{
		vkDestroyBufferView(mpDevice->GetDeviceHandle(), mHandle, nullptr);
		mHandle = VK_NULL_HANDLE;
	}

	if (mpDevice)
	{
		mpDevice = nullptr;
	}
}


const VkBufferView& VulkanBufferView::GetHandle() const
{
	return mHandle;
}