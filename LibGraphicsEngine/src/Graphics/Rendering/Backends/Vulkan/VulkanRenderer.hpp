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
#include "Graphics/Rendering/RenderPasses/RenderPass.hpp"
#include <vector>
#include <unordered_map>

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
			explicit VulkanRenderer(Platform::Window* pWindow);
			virtual ~VulkanRenderer();

			virtual void RenderFrame(RenderQueue* pRenderQueue, RenderPass* pRenderPass) override;
			virtual void UpdateFrame(Camera* pCamera, float32_t crrTime) override;
			virtual void SubmitFrame() override;

			virtual void OnWindowResize(uint32_t width = 0, uint32_t height = 0) override;

			/////////////////////////////////

			virtual void ComputeGraphicsResources(RenderQueue* pRenderQueue, RenderPass* pRenderPass) override;

			virtual void UpdateUniformBuffers(RenderQueue::Renderable* pRenderable, Camera* pCamera, float32_t crrTime) override;
			virtual void UpdateDynamicStates(const DynamicState& dynamicState, uint32_t currentBufferIdx) override;

			virtual void DrawObject(RenderQueue::Renderable* pRenderable, uint32_t currentBufferIdx) override;

			//////////////////////////////

			VulkanDevice* GetDevice() const;
			VulkanPipelineCache* GetPipelineCache() const;
			VulkanRenderPass* GetDefaultRenderPass() const;
			VulkanCommandPool* GetCommandPool() const;

			VulkanCommandBuffer* GetCommandBuffer(uint32_t currentBufferIdx) const;

		private:
			virtual void Init(Platform::Window* pWindow) override;
			virtual void Terminate() override;

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

			void DrawDirect(uint32_t vertexIndexCount, uint32_t instanceCount, uint32_t currentBufferIdx, bool_t isIndexedDrawing = false);
			void DrawIndirect(uint32_t vertexIndexCount, uint32_t instanceCount, uint32_t currentBufferIdx, bool_t isIndexedDrawing = false);

			virtual void BeginFrame() override;
			virtual void EndFrame() override;

			void PrepareFrame();
			void PresentFrame();

		private:
			NO_COPY_NO_MOVE_CLASS(VulkanRenderer)

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
			////////// descriptor/uniform map data
			struct DescriptorSetBindingData
			{
				VkShaderStageFlagBits shaderStage;
				VkDescriptorSetLayoutBinding layoutBinding;
				VkWriteDescriptorSet writeSet;
				VkCopyDescriptorSet copySet;
			};

			typedef std::unordered_map<VkDescriptorType, std::vector<DescriptorSetBindingData>> DescriptorSetBindingMap;
			std::unordered_map<VisualComponent*, DescriptorSetBindingMap, HashUtils::PointerHash<VisualComponent>> mDescriptorSetBindingMapCollection;

			void setupPipeline(GeometricPrimitive* pGeoPrimitive, VisualComponent* pVisComp);
			void AddWriteDescriptorSet(VisualComponent* pVisComp, VkShaderStageFlagBits shaderStage, uint32_t binding, VkDescriptorType descriptorType,
				const VkDescriptorImageInfo* pDescriptorImageInfo, const VkDescriptorBufferInfo* pDescriptorBufferInfo);

			// descriptor pools
			std::unordered_map<VisualComponent*, VulkanDescriptorPool*, HashUtils::PointerHash<VisualComponent>> mpDescriptorPoolMap;

			struct DescriptorSetData
			{
				// 1 descriptor set per material/visual effect
				VulkanDescriptorSetLayout* pDescriptorSetLayout;
				VulkanDescriptorSet* pDescriptorSet;

				DescriptorSetData& operator = (const DescriptorSetData& other)
				{
					pDescriptorSetLayout = other.pDescriptorSetLayout;
					pDescriptorSet = other.pDescriptorSet;

					return *this;
				}
			};

			std::unordered_map<VisualComponent*, DescriptorSetData, HashUtils::PointerHash<VisualComponent>> mDescriptorSetDataCollection;

			struct PipelineData
			{
				// 1 pipeline per material/visual effect
				VulkanPipelineLayout* pPipelineLayout;
				VulkanGraphicsPipeline* pGraphicsPipeline;
			};

			std::unordered_map<VisualComponent*, PipelineData, HashUtils::PointerHash<VisualComponent>> mPipelineDataCollection;
		};
	}
}

#endif /* GRAPHICS_RENDERING_BACKENDS_VULKAN_VULKAN_RENDERER_HPP */