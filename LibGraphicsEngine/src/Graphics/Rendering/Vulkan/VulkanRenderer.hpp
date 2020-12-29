#ifndef GRAPHICS_RENDERING_VULKAN_VULKAN_RENDERER_HPP
#define GRAPHICS_RENDERING_VULKAN_VULKAN_RENDERER_HPP

#ifdef _WIN32
#ifndef VK_USE_PLATFORM_WIN32_KHR
#define VK_USE_PLATFORM_WIN32_KHR
#endif // VK_USE_PLATFORM_WIN32_KHR
#endif // _WIN32

#include "AppConfig.hpp"

#include "Graphics/Rendering/Vulkan/Common/VulkanObject.hpp"
#include "Graphics/Rendering/Renderer.hpp"
#include <vector>

namespace GraphicsEngine
{
	namespace Platform
	{
		class GE_Window;
	}

	namespace Graphics
	{
		class Camera;

		class VulkanDevice;
		class VulkanBuffer;
		class VulkanCommandPool;
		class VulkanCommandBuffer;
		class VulkanFrameBuffer;
		class VulkanFrameBufferAttachment;
		class VulkanRenderPass;
		class VulkanSemaphore;
		class VulkanFence;
		class VulkanShaderModule;
		class VulkanDescriptorPool;
		class VulkanDescriptorSetLayout;
		class VulkanDescriptorSet;
		class VulkanPipelineCache;
		class VulkanPipelineLayout;
		class VulkanGraphicsPipeline;
		class VulkanQueryPool;

		class GeometricPrimitive; // TODO
		class VisualComponent;

		class VulkanRenderer : public VulkanObject, public Renderer
		{
			GE_RTTI(GraphicsEngine::Graphics::VulkanRenderer)

		public:
			VulkanRenderer();
			virtual ~VulkanRenderer();

			virtual void Init(Platform::GE_Window* pWindow) override;
			virtual void Terminate() override;

			virtual void RenderFrame(RenderQueue* pRenderQueue, RenderPass* pRenderPass) override;
			virtual void UpdateFrame(Camera* pCamera, float32_t deltaTime) override;
			virtual void SubmitFrame() override;

			virtual void OnWindowResize(uint32_t width = 0, uint32_t height = 0) override;

			/////////////////////////////////

			virtual void ComputeGraphicsResources(RenderQueue* pRenderQueue, RenderPass* pRenderPass) override;

			virtual void UpdateUniformBuffers(RenderQueue::Renderable* pRenderable, Camera* pCamera) override;

			// this method differs from the Renderer interface!
			virtual void BindShaderBindings(uint32_t currentBufferIdx, VulkanDescriptorSet* pDescriptorSet,
				VulkanPipelineLayout* pPipelineLayout, VulkanGraphicsPipeline* pGraphicsPipeline);

			virtual void DrawObject(RenderQueue::Renderable* pRenderable, uint32_t currentBufferIdx) override;

			virtual void UpdateDynamicStates(const DynamicState& dynamicState, uint32_t currentBufferIdx) override;

			//////////////////////////////

			VulkanDevice* GetDevice() const;
			VulkanPipelineCache* GetPipelineCache() const;
			VulkanRenderPass* GetDefaultRenderPass() const;
			VulkanCommandPool* GetCommandPool() const;

		private:
			void Prepare();

			void setupDefaultRenderPass();
			void setupDefaultFrameBuffer();

			void setupDrawCommandPool();
			void setupDrawCommandBuffers();

			void setupSynchronizationPrimitives();
			void setupPipelineCache();
			void setupSubmitInfo();

			void setupPipelineStats();
			void getQueryResults();

			void resetQuery(uint32_t currentBufferIdx);
			void beginQuery(uint32_t currentBufferIdx);
			void endQuery(uint32_t currentBufferIdx);

			void DrawSceneToCommandBuffer();

			virtual void BeginFrame() override;
			virtual void EndFrame() override;

			void PrepareFrame();
			void PresentFrame();

		private:
			NO_COPY_NO_MOVE(VulkanRenderer)

			// VULKAN RESOURCES

			// Device
			VulkanDevice* mpDevice;

			// Default framebuffers
			std::vector<VulkanFrameBuffer*> mDefaultFrameBuffers;
			// Default renderPass
			VulkanRenderPass* mpDefaultRenderPass;

			// Synchronization primitives
			// Synchronization is an important concept of Vulkan that OpenGL mostly hid away. Getting this right is crucial to using Vulkan.

			// Semaphores
			// Used to coordinate operations within the graphics queue and ensure correct command ordering
			VulkanSemaphore* mpRenderCompleteSemaphore;
			VulkanSemaphore* mpPresentCompleteSemaphore;

			// Fences
			// Used to check the completion of queue operations (e.g. command buffer execution)
			std::vector<VulkanFence*> mWaitFences;

			// command pool for rendering commands
			VulkanCommandPool* mpCommandPool;

			// Command buffers used for rendering
			std::vector<VulkanCommandBuffer*> mDrawCommandBuffers;

			// Active frame buffer index
			uint32_t mCurrentBufferIdx;

			// Contains command buffers and semaphores to be presented to the queue
			VkSubmitInfo mSubmitInfo;
			VkPipelineStageFlags mSubmitPipelineStages;

			// Pipeline cache object
			VulkanPipelineCache* mpPipelineCache;


			//  pipeline statistics results
			VulkanQueryPool* mpQueryPool;

			std::vector<uint64_t> mPipelineStats;
			std::vector<std::string> mPipelineStatNames;

			//////////////////////////////////////////

			// Example settings that can be changed e.g. by command line arguments
			struct {
				/** @brief Activates validation layers (and message output) when set to true */
				bool validation = false;
				/** @brief Set to true if fullscreen mode has been requested via command line */
				bool fullscreen = false;
				/** @brief Set to true if v-sync will be forced for the swapchain */
				bool vsync = false;
				/** @brief Enable UI overlay */
				bool overlay = false;
			} mSettings;



			///////////////////////////////////
			void setupPipeline(GeometricPrimitive* pGeoPrimitive, VisualComponent* pVisComp);

			// 1 descriptor pool is enough for all needs
			VulkanDescriptorPool* mpDescriptorPool;

			struct DescriptorSetData
			{
				// 1 per object/effect (render pass)
				VulkanDescriptorSetLayout* pDescriptorSetLayout;
				VulkanDescriptorSet* pDescriptorSet;
			};

			std::vector<DescriptorSetData> mDescriptorSetDataCollection;

			struct PipelineData
			{
				// 1 pipeline per effect (render pass)
				VulkanPipelineLayout* pPipelineLayout;
				VulkanGraphicsPipeline* pGraphicsPipeline;
			};

			std::vector<PipelineData> mPipelineDataCollection;
		};
	}
}

#endif /* GRAPHICS_RENDERING_VULKAN_VULKAN_RENDERER_HPP */