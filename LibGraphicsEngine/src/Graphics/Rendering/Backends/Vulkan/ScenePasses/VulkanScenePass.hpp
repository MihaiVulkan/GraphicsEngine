#ifndef GRAPHICS_RENDERING_BACKEND_VULKAN_SCENE_PASSES_VULKAN_SCENE_PASS_HPP
#define GRAPHICS_RENDERING_BACKEND_VULKAN_SCENE_PASSES_VULKAN_SCENE_PASS_HPP

#include "Graphics/Rendering/Backends/Vulkan/Common/VulkanObject.hpp"
#include "Graphics/Rendering/Backends/Vulkan/Resources/VulkanTexture.hpp"
#include "Graphics/Rendering/ScenePasses/ScenePass.hpp"
#include <vector>
#include <unordered_map>
#include "glm/vec2.hpp"
#include "glm/vec4.hpp"

namespace GraphicsEngine
{
	namespace Graphics
	{
		class Renderer;
		class RenderQueue;
		class Camera;
		class ScenePass;

		class VulkanRenderer;
		class VulkanRenderTarget;
		class VulkanFrameBuffer;
		class VulkanRenderPass;

		// Vulkan implementation of the ScenePass class
		class VulkanScenePass : public VulkanObject
		{
			GE_RTTI(GraphicsEngine::Graphics::VulkanScenePass)

		public:
			VulkanScenePass();
			explicit VulkanScenePass(VulkanRenderer* pVulkanRenderer);
			virtual ~VulkanScenePass();

			void Init(ScenePass* pScenePass);
			// we need to be able to reset the framebuffers on window resize
			void Reset();

			void Render(Renderer* pRenderer, RenderQueue* pRenderQueue, uint32_t currentBufferIdx);
			void Update(Renderer* pRenderer, RenderQueue* pRenderQueue, Camera* pCamera, float32_t crrTime);

			VulkanRenderPass* GetRenderPass(ScenePass::PassType passType);
			ScenePass* GetScenePass();
			const std::vector<GADRTexture*>& GetRenderTargets(ScenePass::PassType passType) const;

		private:
			NO_COPY_NO_MOVE_CLASS(VulkanScenePass)

			void InitHelper(ScenePass* pScenePass);
			void ResetHelper(ScenePass* pScenePass);

			void SetupStandardRenderPass();
			void SetupStandardFrameBuffers();

			void SetupOffscreenRenderTargets();
			void SetupOffscreenRenderPass();
			void SetupOffscreenFrameBuffers();


			void Begin(ScenePass* pScenePass, uint32_t currentBufferIdx);
			void End(ScenePass* pScenePass, uint32_t currentBufferIdx);

			VulkanRenderer* mpVulkanRenderer;

			struct PassBuffers
			{
				PassBuffers()
					: pRenderPass(nullptr)
				{}

				ScenePass* pPass;
				
				std::vector<GADRTexture*> renderTargets;
				VulkanRenderPass* pRenderPass;
				std::vector<VulkanFrameBuffer*> frameBuffers;
			};
			std::unordered_map<ScenePass::PassType, PassBuffers> mVulkanPassBuffersMap;

			uint32_t m_FrameBufferCount;

			ScenePass* mpScenePass;
		};
	}
}

#endif // GRAPHICS_RENDERING_BACKEND_VULKAN_SCENE_PASSES_VULKAN_SCENE_PASS_HPP