#ifndef GRAPHICS_RENDERING_BACKENDS_OPENGL_VISUAL_PASSES_OPENGL_VISUAL_PASS_HPP
#define GRAPHICS_RENDERING_BACKENDS_OPENGL_VISUAL_PASSES_OPENGL_VISUAL_PASS_HPP

#if defined(OPENGL_RENDERER)
#include "Graphics/Rendering/Backends/OpenGL/Common/OpenGLObject.hpp"
#include "Graphics/Rendering/VisualPasses/VisualPass.hpp"
#include <vector>
#include <unordered_map>

namespace GraphicsEngine
{
	namespace Graphics
	{
		class Renderer;
		class RenderQueue;
		class Camera;

		class OpenGLRenderer;
		class OpenGLVertexArrayObject;
		class OpenGLShaderProgram;

		/* Base class for Graphics API Dependent Visual Pass */
		class GADVisualPass : public VisualPass, public OpenGLObject
		{
			GE_RTTI(GraphicsEngine::Graphics::GADVisualPass)

		public:
			GADVisualPass();
			GADVisualPass(Renderer* pRenderer, VisualPass* pVisualPass);
			~GADVisualPass();

			virtual void RenderNode(uint32_t currentBufferIdx) override;
			virtual void UpdateNode(Camera* pCamera, float32_t crrTime) override;

		protected:
			void Init(Renderer* pRenderer, VisualPass* pVisualPass);

			void SetupPipeline();
			void SetupShaderStage();
			void SetupVertexInputState();
			void SetupPrimitiveAssemblyState();
			void SetupViewportScissorState();
			void SetupRasterizationState();
			void SetupMultisampleState();
			void SetupDepthStencilState();
			void SetupColorBlendState();
			void SetupDynamicState();

			void BindUniforms();

			OpenGLRenderer* mpOpenGLRenderer;

			OpenGLVertexArrayObject* mpOpenGLVertexArrayObject;
			OpenGLShaderProgram* mpOpenGLShaderProgram;

			VisualPass* mpVisualPass;
			bool_t mIsPresentPass; // default pass - to present to screen

			static int msGlobalTexUnit;
			uint8_t mStartTextUnit;

		private:
			NO_COPY_NO_MOVE_CLASS(GADVisualPass)

		};
	}
}
#endif // OPENGL_RENDERER

#endif // GRAPHICS_RENDERING_BACKENDS_OPENGL_VISUAL_PASSES_OPENGL_VISUAL_PASS_HPP