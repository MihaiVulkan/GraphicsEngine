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
#include "Graphics/Rendering/ScenePasses/ScenePass.hpp"
#include "Graphics/Rendering/Backends/Vulkan/ScenePasses/VulkanScenePass.hpp"
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

			virtual void RenderFrame(RenderQueue* pRenderQueue) override;
			virtual void UpdateFrame(Camera* pCamera, float32_t crrTime) override;
			virtual void SubmitFrame() override;

			virtual void OnWindowResize(uint32_t width = 0, uint32_t height = 0) override;

			/////////////////////////////////

			virtual void ComputeGraphicsResources(RenderQueue* pRenderQueue, ScenePass* pScenePass) override;

			virtual void UpdateUniformBuffers(ScenePass* pScenePass, const RenderQueue::Renderable* pRenderable, Camera* pCamera, float32_t crrTime) override;
			virtual void UpdateDynamicStates(ScenePass* pScenePass, const DynamicState& dynamicState, uint32_t currentBufferIdx) override;

			virtual void DrawObject(ScenePass* pScenePass, const RenderQueue::Renderable* pRenderable, uint32_t currentBufferIdx) override;

			virtual void BindLight(ScenePass* pScenePass, const LightNode* pLightNode, GeometryNode* pGeoNode) override;

			void DrawDirect(uint32_t indexCount, uint32_t firstIndex, uint32_t instanceCount, uint32_t currentBufferIdx, bool_t isIndexedDrawing = false);
			void DrawIndirect(uint32_t indexCount, uint32_t firstIndex, uint32_t instanceCount, uint32_t currentBufferIdx, bool_t isIndexedDrawing = false);

			//////////////////////////////

			VulkanDevice* GetDevice() const;
			VulkanPipelineCache* GetPipelineCache() const;
			VulkanCommandPool* GetCommandPool() const;

			VulkanCommandBuffer* GetCommandBuffer(uint32_t currentBufferIdx) const;

		private:
			NO_COPY_NO_MOVE_CLASS(VulkanRenderer)

			virtual void Init(Platform::Window* pWindow) override;
			virtual void Terminate() override;

			void Prepare();

			void SetupDrawCommandPool();
			void SetupDrawCommandBuffers();

			void SetupSynchronizationPrimitives();
			void SetupPipelineCache();
			void SetupSubmitInfo();

			void SetupPipelineStats();
			void GetQueryResults();

			void SetupPipelines(ScenePass* pScenePass, GeometryNode* pGeoNode);
			void SetupStandardPipeline(ScenePass* pScenePass, GeometryNode* pGeoNode);
			void SetupOffscreenPipeline(ScenePass* pScenePass, GeometryNode* pGeoNode);

			void SetupDescriptorSets(ScenePass::PassType passType, VisualComponent* pVisComp);
			void SetupShaderStage(ScenePass* pScenePass, VisualComponent* pVisComp, std::vector<VkPipelineShaderStageCreateInfo>& shaderStagesOut);
			void SetupVertexInputState(VisualComponent* pVisComp, GeometricPrimitive* pGeometry, VkPipelineVertexInputStateCreateInfo& pipelineVertexInputStateCreateInfoOut);
			void SetupPrimitiveAssemblyState(GeometricPrimitive* pGeometry, VkPipelineInputAssemblyStateCreateInfo& pipelineInputAssemblyStateCreateInfoOut);
			void SetupViewportScissorState(VisualComponent* pVisComp, VkPipelineViewportStateCreateInfo& pipelineViewportStateCreateInfoOut);
			void SetupRasterizationState(ScenePass* pScenePass, VisualComponent* pVisComp, GeometricPrimitive* pGeometry, VkPipelineRasterizationStateCreateInfo& pipelineRasterizationStateCreateInfoOut);
			void SetupMultisampleState(VkPipelineMultisampleStateCreateInfo& pipelineMultisampleStateCreateInfoOut);
			void SetupDepthStencilState(VisualComponent* pVisComp, VkPipelineDepthStencilStateCreateInfo& pipelineDepthStencilStateCreateInfoOut);
			void SetupColorBlendState(VisualComponent* pVisComp, VkPipelineColorBlendStateCreateInfo& pipelineColorBlendStateCreateInfoOut);
			void SetupDynamicState(VisualComponent* pVisComp, VkPipelineDynamicStateCreateInfo& pipelineDynamicStateCreateInfoOut);

			void AddWriteDescriptorSet(ScenePass::PassType passType, VisualComponent* pVisComp, VkShaderStageFlagBits shaderStage, uint32_t setId, uint32_t binding, VkDescriptorType descriptorType,
				const VkDescriptorImageInfo* pDescriptorImageInfo, const VkDescriptorBufferInfo* pDescriptorBufferInfo);

			void ResetQuery(uint32_t currentBufferIdx);
			void BeginQuery(uint32_t currentBufferIdx);
			void EndQuery(uint32_t currentBufferIdx);

			void DrawSceneToCommandBuffer();

			virtual void BeginFrame() override;
			virtual void EndFrame() override;

			void PrepareFrame();
			void PresentFrame();


			// VULKAN RESOURCES

			// Device
			VulkanDevice* mpDevice;

			// Scene pass
			VulkanScenePass* mpScenePass;

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
				VisualComponent* pVisualComponent; // reference to the visual component to be able to differentiate

				VkDescriptorSetLayoutBinding layoutBinding;
				VkWriteDescriptorSet writeSet;
			//	VkCopyDescriptorSet copySet; //TODO
			};

			// key - is descriptor setId, value - vector of descriptor binding data
			typedef std::unordered_map<uint32_t, std::vector<DescriptorSetBindingData>> DescriptorSetBindingMap;
			std::unordered_map<ScenePass::PassType, DescriptorSetBindingMap> mDescriptorSetBindingMapCollection;

			// descriptor pools
			struct DescriptorPoolData
			{
				VisualComponent* pVisualComponent; // reference to the visual component to be able to differentiate

				VulkanDescriptorPool* pDescriptorPool;
			};


			std::unordered_map<ScenePass::PassType, std::vector<DescriptorPoolData>> mpDescriptorPoolMap;

			struct DescriptorSetData
			{
				VisualComponent* pVisualComponent; // reference to the visual component to be able to differentiate

				// 1 descriptor set per material/visual effect
				VulkanDescriptorSetLayout* pDescriptorSetLayout;
				VulkanDescriptorSet* pDescriptorSet;
			};

			//TOOD - for now only 1 descriptor set per visual component!
			std::unordered_map<ScenePass::PassType, std::vector<DescriptorSetData>> mDescriptorSetDataCollection;

			struct PipelineData
			{
				// 1 pipeline per material/visual effect
				VisualComponent* pVisualComponent; // reference to the visual component to be able to differentiate

				VulkanPipelineLayout* pPipelineLayout;
				VulkanGraphicsPipeline* pGraphicsPipeline;
			};

			std::unordered_map<ScenePass::PassType, std::vector<PipelineData>> mPipelineDataCollection;

		};
	}
}

#endif /* GRAPHICS_RENDERING_BACKENDS_VULKAN_VULKAN_RENDERER_HPP */