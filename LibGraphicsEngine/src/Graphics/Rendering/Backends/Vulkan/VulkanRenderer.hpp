#ifndef GRAPHICS_RENDERING_BACKENDS_VULKAN_VULKAN_RENDERER_HPP
#define GRAPHICS_RENDERING_BACKENDS_VULKAN_VULKAN_RENDERER_HPP

//#ifdef _WIN32
//#ifndef VK_USE_PLATFORM_WIN32_KHR
//#define VK_USE_PLATFORM_WIN32_KHR
//#endif // VK_USE_PLATFORM_WIN32_KHR
//#endif // _WIN32

#include "Core/AppConfig.hpp"

#include "Graphics/Rendering/Backends/Vulkan/Common/VulkanObject.hpp"
#include "Graphics/Rendering/Renderer.hpp"
#include "Graphics/Rendering/VisualPasses/VisualPass.hpp"
#include <vector>
#include <map>

namespace GraphicsEngine
{
	namespace Platform
	{
		class Window;
	}

	namespace Graphics
	{
		class Camera;

		class VulkanDevice;

		class VulkanCommandPool;
		class VulkanCommandBuffer;

		class VulkanSemaphore;
		class VulkanFence;

		class VulkanShaderModule;
		class VulkanDescriptorPool;
		class VulkanDescriptorSetLayout;
		class VulkanDescriptorSet;
		class VulkanPipelineCache;
		class VulkanPipelineLayout;
		class VulkanGraphicsPipeline;
		class VulkanFrameBuffer;
		class VulkanRenderPass;

		class VulkanQueryPool;

		class VisualComponent;

		class VulkanRenderer : public VulkanObject, public Renderer
		{
			GE_RTTI(GraphicsEngine::Graphics::VulkanRenderer)

		public:
			VulkanRenderer();
			explicit VulkanRenderer(Platform::Window* pWindow);
			virtual ~VulkanRenderer();

			virtual void RenderFrame(RenderQueue* pRenderQueue) override;
			virtual void UpdateFrame(Camera* pCamera, float32_t crrTime) override;
			virtual void SubmitFrame() override;

			virtual void OnWindowResize(uint32_t width = 0, uint32_t height = 0) override;

			/////////////////////////////////

			virtual void ComputeGraphicsResources(RenderQueue* pRenderQueue) override;

			virtual void BindLight(VisualPass* pVisualPass, const LightNode* pLightNode, GeometryNode* pGeoNode) override;

			virtual void DrawNode(VisualPass* pVisualPass, GeometryNode* pGeoNode, uint32_t currentBufferIdx) override;
			virtual void UpdateNode(VisualPass* pVisualPass, GeometryNode* pGeoNode, Camera* pCamera, float32_t crrTime) override;

			//////////////////////////////

			VulkanDevice* GetDevice() const;
			VulkanPipelineCache* GetPipelineCache() const;
			VulkanCommandPool* GetCommandPool() const;

			VulkanCommandBuffer* GetCommandBuffer(uint32_t currentBufferIdx) const;

			VulkanRenderPass* GetRenderPass(VisualPass* pVisualPass);

		private:
			NO_COPY_NO_MOVE_CLASS(VulkanRenderer)

			struct VisualPassBeginData
			{
				VisualPassBeginData()
					: width(0), height(0)
					, clearColor(0.0f)
					, depth(0.0f), stencil(0)
				{}

				uint32_t width, height;
				Color4f clearColor;
				float32_t depth;
				uint32_t stencil;
			};

			struct VisualPassData
			{
				VisualPassBeginData passBeginData;

				VulkanRenderPass* pRenderPass;
				std::vector<VulkanFrameBuffer*> frameBuffers;
				std::vector<VisualPass*> passes;
			};

			virtual void Init(Platform::Window* pWindow) override;
			virtual void Terminate() override;

			void Prepare();

			void SetupDrawCommandPool();
			void SetupDrawCommandBuffers();

			VulkanRenderPass* SetupRenderPass(VisualPass* pVisualPass);
			void SetupFrameBuffers(VisualPass* pVisualPass, VulkanRenderPass* pRenderPass, std::vector<VulkanFrameBuffer*>& frameBuffersOut,
				VisualPassBeginData& visualPassBeginDataOut);

			void SetupSynchronizationPrimitives();
			void SetupPipelineCache();
			void SetupSubmitInfo();

			void SetupPipelineStats();
			void GetQueryResults();

			void ResetQuery(VisualPass::PassType passType, uint32_t currentBufferIdx);
			void BeginQuery(VisualPass::PassType passType, uint32_t currentBufferIdx);
			void EndQuery(VisualPass::PassType passType, uint32_t currentBufferIdx);

			void DrawSceneToCommandBuffer();
			void DrawNodes(uint32_t currentBufferIdx);

			void UpdateNodes(Camera* pCamera, float32_t crrTime);

			virtual void BeginFrame() override;
			virtual void EndFrame() override;

			void PrepareFrame();
			void PresentFrame();

			void UpdateDynamicStates(VisualPass* pVisualPass, uint32_t currentBufferIdx);
			void UpdateUniformBuffers(VisualPass* pVisualPass, GeometryNode* pGeoNode, Camera* pCamera, float32_t crrTime);

			void DrawDirect(uint32_t indexCount, uint32_t firstIndex, uint32_t instanceCount, uint32_t currentBufferIdx, bool_t isIndexedDrawing = false);
			void DrawIndirect(uint32_t indexCount, uint32_t firstIndex, uint32_t instanceCount, uint32_t currentBufferIdx, bool_t isIndexedDrawing = false);


			void BeginRenderPass(const VisualPassData& visualPassData, uint32_t currentBufferIdx);
			void EndRenderPass(const VisualPassData& visualPassData, uint32_t currentBufferIdx);

			void AddVisualPass(VisualPass* pVisualPass);

			// VULKAN RESOURCES

			// Device
			VulkanDevice* mpDevice;

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

			// map must be ordered as the passes must be processed in the pass type order
			std::map<VisualPass::PassType, VisualPassData> mVisualPassMap;


			//  pipeline statistics results
			struct PipelineStatsData
			{
				VulkanQueryPool* pQueryPool;
				std::vector<uint64_t> pipelineStats;
				std::vector<std::string> pipelineStatNames;
			};

			std::map<VisualPass::PassType, PipelineStatsData> mPipelineStatsMap;

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
		};
	}
}

#endif /* GRAPHICS_RENDERING_BACKENDS_VULKAN_VULKAN_RENDERER_HPP */