#ifndef GRAPHICS_RENDERING_BACKENDS_OPENGL_OPENGL_RENDERER_HPP
#define GRAPHICS_RENDERING_BACKENDS_OPENGL_OPENGL_RENDERER_HPP

#if defined(OPENGL_RENDERER)
#include "Graphics/Rendering/Backends/OpenGL/Common/OpenGLObject.hpp"
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

		class OpenGLShaderObject;
		class OpenGLFrameBuffer;

		class VisualComponent;
		class IndexBuffer;

		class OpenGLRenderer : public OpenGLObject, public Renderer
		{
			GE_RTTI(GraphicsEngine::Graphics::OpenGLRenderer)

		public:
			OpenGLRenderer();
			explicit OpenGLRenderer(Platform::Window* pWindow);
			virtual ~OpenGLRenderer();

			virtual void RenderFrame(RenderQueue* pRenderQueue) override;
			virtual void UpdateFrame(Camera* pCamera, float32_t crrTime) override;
			virtual void SubmitFrame() override;

			virtual void OnWindowResize(uint32_t width = 0, uint32_t height = 0) override;

			/////////////////////////////////

			virtual void ComputeGraphicsResources(RenderQueue* pRenderQueue) override;

			virtual void BindLight(VisualPass* pVisualPass, const LightNode* pLightNode, GeometryNode* pGeoNode) override;

			virtual void DrawNode(VisualPass* pVisualPass, GeometryNode* pGeoNode, uint32_t currentBufferIdx) override;
			virtual void UpdateNode(VisualPass* pVisualPass, GeometryNode* pGeoNode, Camera* pCamera, float32_t crrTime) override;

		private:
			NO_COPY_NO_MOVE_CLASS(OpenGLRenderer)

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
				VisualPassData()
					: pFrameBuffer(nullptr)
				{}

				VisualPassBeginData passBeginData;

				OpenGLFrameBuffer* pFrameBuffer;
				std::vector<VisualPass*> passes;
			};

			virtual void Init(Platform::Window* pWindow) override;
			virtual void Terminate() override;

			void Prepare();

			void SetupPipelineStats();
			void GetQueryResults();

			void ResetQuery(VisualPass::PassType passType, uint32_t currentBufferIdx);
			void BeginQuery(VisualPass::PassType passType, uint32_t currentBufferIdx);
			void EndQuery(VisualPass::PassType passType, uint32_t currentBufferIdx);

			void DrawSceneToBackBuffer();
			void DrawNodes(uint32_t currentBufferIdx);

			void UpdateNodes(Camera* pCamera, float32_t crrTime);

			virtual void BeginFrame() override;
			virtual void EndFrame() override;

			void PrepareFrame();
			void PresentFrame();

			void UpdateUniformBuffers(VisualPass* pVisualPass, GeometryNode* pGeoNode, Camera* pCamera, float32_t crrTime);

			void DrawDirect(uint32_t count, uint32_t first, IndexBuffer* pIndexBuffer = nullptr);
		//	void DrawIndirect(uint32_t indexCount, uint32_t firstIndex, uint32_t instanceCount, uint32_t currentBufferIdx, bool_t isIndexedDrawing = false);


			void BeginRenderPass(const VisualPassData& visualPassData, uint32_t currentBufferIdx);
			void EndRenderPass(const VisualPassData& visualPassData, uint32_t currentBufferIdx);

			void SetupFrameBuffers(VisualPass* pVisualPass, OpenGLFrameBuffer*& pFrameBufferOut, VisualPassBeginData& visualPassBeginDataOut);

			void AddVisualPass(VisualPass* pVisualPass);

			Platform::Window* mpWindow;

			// OPENGL RESOURCES

			// Active frame buffer index
			uint32_t mCurrentBufferIdx;

			// map must be ordered as the passes must be processed in the pass type order
			std::map<VisualPass::PassType, VisualPassData> mVisualPassMap;

			//  pipeline statistics results
			struct PipelineStatsData
			{
				std::vector<std::string> queryNames;
				std::vector<GLenum> queryTargets;
				std::vector<GLuint> queryIds;
			};

			std::map<VisualPass::PassType, PipelineStatsData> mPipelineStatsMap;

			//////////////////////////////////////////
		};
	}
}
#endif // OPENGL_RENDERER

#endif /* GRAPHICS_RENDERING_BACKENDS_OPENGL_OPENGL_RENDERER_HPP */