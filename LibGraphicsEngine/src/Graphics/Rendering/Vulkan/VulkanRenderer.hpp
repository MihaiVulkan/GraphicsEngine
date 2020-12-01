#ifndef GRAPHICS_RENDERING_VULKAN_RENDERER_HPP
#define GRAPHICS_RENDERING_VULKAN_RENDERER_HPP

#ifdef _WIN32
#ifndef VK_USE_PLATFORM_WIN32_KHR
#define VK_USE_PLATFORM_WIN32_KHR
#endif // VK_USE_PLATFORM_WIN32_KHR
#endif // _WIN32

#define NEW_GRAPHICS

#include "AppConfig.hpp"

#include "Graphics/Camera/Camera.hpp"
#include "Graphics/Rendering/Renderer.hpp"

#include "vulkan/vulkan.h"

#ifndef NEW_GRAPHICS
#include "glm/common.hpp"
#include "glm/vec2.hpp"
#include "glm/vec3.hpp"
#include "glm/mat4x4.hpp"
#include <vector>
#include <cstdint>

//// GLSLS Debug ////
#include "glslang/glslang/Include/ResourceLimits.h"
#include "glslang/glslang/Public/ShaderLang.h"
////
#endif

namespace GraphicsEngine
{
	namespace Platform
	{
		class GE_Window;
	}

	namespace Graphics
	{
		class VulkanDevice;
		class VulkanBuffer;
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

		class VulkanRenderer : public Renderer
		{
			GE_RTTI(GraphicsEngine::Graphics::VulkanRenderer)

		public:
			VulkanRenderer();
			virtual ~VulkanRenderer();

			virtual void Init(Platform::GE_Window* pWindow) override;
			virtual void Terminate() override;

			virtual void RenderFrame() override;
			virtual void UpdateFrame(bfloat32_t deltaTime) override;
			virtual void SubmitFrame() override;


			virtual void WindowResize(uint32_t width = 0, uint32_t height = 0) override;


			VulkanDevice* GetDevice() const;

			/////////////////////////////////

			virtual void Render(RenderQueue* pRenderQueue, RenderPass* pRenderPass) override;
			virtual void Update(Camera* pCamera, bfloat32_t deltaTime) override;

			virtual void UpdateUniformBuffers(RenderQueue::Renderable* pRenderable, Camera* pCamera) override;

			// this method differs from the Renderer interface!
			virtual void BindShaderBindings(uint32_t currentBufferIdx, VulkanDescriptorSet* pDescriptorSet,
				VulkanPipelineLayout* pPipelineLayout, VulkanGraphicsPipeline* pGraphicsPipeline);

			void DrawSceneToCommandBuffer();

			virtual void DrawObject(RenderQueue::Renderable* pRenderable, uint32_t currentBufferIdx) override;
			virtual void DrawGeometry(GeometryNode* pGeometryNode, uint32_t currentBufferIdx) override;

			//////////////////////////////

			VulkanPipelineCache* GetVkPipelineCache() const;
			VulkanRenderPass* GetVkDefaultRenderPass() const;

		private:
			void Prepare();

			void setupDefaultDepthStencil();
			void setupDefaultRenderPass();
			void setupDefaultFrameBuffer();

			void setupDrawCommandBuffers();
			void setupSynchronizationPrimitives();
			void setupPipelineCache();
			void setupSubmitInfo();

			void setupPipelineStats();
			void getQueryResults();

			void resetQuery(uint32_t currentBufferId);
			void beginQuery(uint32_t currentBufferId);
			void endQuery(uint32_t currentBufferId);

			virtual void BeginFrame() override;
			virtual void EndFrame() override;

			////////////// Particular functions dependent on scene content
#ifndef NEW_GRAPHICS
			void setupScene();
			void setupVertexBuffers();
			void setupUniformBuffers();
			void updateUBO();
			void setupShaders();
			void setupTextures();
			void setupDescriptorPool();
			void setupDescriptorSet();
			void setupPipeline();
			void buildCommandBuffers();
#endif
			//////////////

			void PrepareFrame();
			void PresentFrame();

#ifndef NEW_GRAPHICS
			void Draw(); //TODO - remove
			virtual void viewChanged();
#endif

		private:
			NO_COPY_NO_MOVE(VulkanRenderer)

			// VULKAN RESOURCES

			// Device
			VulkanDevice* mpDevice;

			// DepthStencil Buffer attachment
			VulkanFrameBufferAttachment* mpDepthStencil;

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

#ifndef NEW_GRAPHICS
			// Shader modules
			std::vector<VulkanShaderModule*> mShaderModules;

			// Descriptor set pool
			VulkanDescriptorPool* mpDescriptorPool;

			// The descriptor set stores the resources bound to the binding points in a shader
			// It connects the binding points of the different shaders with the buffers and images used for those bindings
			VulkanDescriptorSet* mpDescriptorSet;

			// The descriptor set layout describes the shader binding layout (without actually referencing descriptor)
			// Like the pipeline layout it's pretty much a blueprint and can be used with different descriptor sets as long as their layout matches
			VulkanDescriptorSetLayout* mpDescriptorSetLayout;


			///////////// SPECIFICS APP DEPENDENT
				// Vertex layout used in this example
			struct Vertex {
				float position[3];
				float color[3];
			};

			// Vertex buffer
			VulkanBuffer* mpVertices;

			// Index buffer
			VulkanBuffer* mpIndices;
			uint32_t mIndicesCount;

			// Uniform buffer block object/
			VulkanBuffer* mpUniformBufferVS;

			// For simplicity we use the same uniform block layout as in the shader:
			//
			//	layout(set = 0, binding = 0) uniform UBO
			//	{
			//		mat4 projectionMatrix;
			//		mat4 modelMatrix;
			//		mat4 viewMatrix;
			//	} ubo;
			//
			// This way we can just memcopy the ubo data to the ubo
			// Note: You should use data types that align with the GPU in order to avoid manual padding (vec4, mat4)
			struct {
				glm::mat4 projectionMatrix;
				glm::mat4 modelMatrix;
				glm::mat4 viewMatrix;
			} mUboVS;


			// The pipeline layout is used by a pipline to access the descriptor sets 
			// It defines interface (without binding any actual data) between the shader stages used by the pipeline and the shader resources
			// A pipeline layout can be shared among multiple pipelines as long as their interfaces match
			VulkanPipelineLayout* mpPipelineLayout;

			// Pipelines (often called "pipeline state objects") are used to bake all states that affect a pipeline
			// While in OpenGL every state can be changed at (almost) any time, Vulkan requires to layout the graphics (and compute) pipeline states upfront
			// So for each combination of non-dynamic pipeline states you need a new pipeline (there are a few exceptions to this not discussed here)
			// Even though this adds a new dimension of planing ahead, it's a great opportunity for performance optimizations by the driver
			VulkanGraphicsPipeline* mpGraphicsPipeline;
#endif
		};
	}
}

#endif /* GRAPHICS_RENDERING_VULKAN_RENDERER_HPP */