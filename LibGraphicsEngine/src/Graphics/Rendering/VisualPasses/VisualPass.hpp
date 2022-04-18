#ifndef GRAPHICS_RENDERING_VISUAL_PASSES_VISUAL_PASS_HPP
#define GRAPHICS_RENDERING_VISUAL_PASSES_VISUAL_PASS_HPP

#include "Foundation/Object.hpp"
#include "Graphics/Rendering/Resources/Shader.hpp"
#include "Graphics/Rendering/Resources/RenderTarget.hpp"
#include "Graphics/GeometricPrimitives/GeometricPrimitive.hpp"
#include "Graphics/Rendering/PipelineStates/CullFaceState.hpp"
#include "Graphics/Rendering/PipelineStates/DepthStencilState.hpp"
#include "Graphics/Rendering/PipelineStates/ColorBlendState.hpp"
#include "Graphics/Rendering/PipelineStates/DynamicState.hpp"
#include "Foundation/MemoryManagement/MemoryOperations.hpp"
#include "glm/mat4x4.hpp"
#include <vector>
#include <unordered_map>
#include <functional>

namespace GraphicsEngine
{
	namespace Graphics
	{
		class Renderer;
		class Camera;

		class Texture;
		class UniformBuffer;
		class GeometryNode;

		class VisualEffect;

		class GADVisualPass;

		/* Base class for Visual Passes */
		class VisualPass : public Object
		{
			GE_RTTI(GraphicsEngine::Graphics::VisualPass)

		public:
			enum class PassType : uint8_t
			{
				// ordered by the render order
				GE_PT_OFFSCREEN = 1, // render to an offscreen framebuffer
				GE_PT_SHADOWS = 2, // shadows, a variant of offscreen rendering
				GE_PT_STANDARD = 3, // render to screen - last pass, rename to GE_PT_PRESENT ???
				GE_PT_COUNT
			};

			//TODO - move these some other place
			////////
			enum class PrimitiveTopology : uint8_t
			{
				GE_PT_POINT_LIST = 0,
				GE_PT_LINE_LIST,
				GE_PT_LINE_LOOP,
				GE_PT_LINE_STRIP,
				GE_PT_TRIANGLE_LIST,
				GE_PT_TRIANGLE_STRIP,
				GE_PT_TRIANGLE_FAN,
				GE_PT_PATCH_LIST,
				GE_PT_COUNT
			};

			// front face winding
			enum class FaceWinding : uint8_t
			{
				GE_FW_CLOCKWISE = 0,
				GE_FW_COUNTER_CLOCKWISE,
				GE_FW_COUNT
			};

			enum class PolygonMode : uint8_t
			{
				GE_PM_FILL = 0,
				GE_PM_LINE,
				GE_PM_POINT,
				GE_PM_COUNT
			};
			////////

			struct RenderData
			{
				RenderData()
					: width(0), height(0)
					, clearColor(0.0f)
					, depth(0.0f), stencil(0)
				{}

				uint32_t width, height;
				Color4f clearColor;
				float32_t depth;
				uint32_t stencil;
			};

			typedef std::unordered_map<Shader::ShaderStage, Shader*> ShaderMap;
			typedef std::unordered_map<Shader::ShaderStage, std::vector<Texture*>> TextureMap;
			// NOTE! For now we use only 1 UBO per shader stage
			typedef std::unordered_map<Shader::ShaderStage, UniformBuffer*> UniformBufferMap;

			typedef std::unordered_map<RenderTarget::TargetType, RenderTarget*> RenderTargetMap;

			typedef std::function<void(VisualPass*, uint32_t)> OnBeginFN;
			typedef std::function<void(VisualPass*, uint32_t)> OnEndFN;

			VisualPass();
			explicit VisualPass(VisualPass::PassType type);
			~VisualPass();

			virtual void Init(Renderer* pRenderer, VisualEffect* pVisualEffect);

			virtual void RenderNode(uint32_t currentBufferIdx);
			virtual void UpdateNode(Camera* pCamera, float32_t crrTime);

			void AddShader(Shader* pShader);
			Shader* GetShader(Shader::ShaderStage shaderStage);
			const VisualPass::ShaderMap& GetShaders() const;
			bool_t HasShaders() const;

			void AddTexture(Shader::ShaderStage shaderStage, Texture* pTexture);
			Texture* GetTexture(Shader::ShaderStage shaderStage, uint8_t id) const;
			const VisualPass::TextureMap& GetTextures() const;
			bool_t HasTextures() const;

			void AddRenderTarget(RenderTarget* pRenderTarget);
			RenderTarget* GetRenderTarget(RenderTarget::TargetType targetType);
			const VisualPass::RenderTargetMap& GetRenderTargets() const;
			bool_t HasRenderTargets() const;

			void AddUniformBuffer(Shader::ShaderStage shaderStage, UniformBuffer* pUniformBuffer);
			UniformBuffer* GetUniformBuffer(Shader::ShaderStage shaderStage) const;
			const VisualPass::UniformBufferMap& GetUniformBuffers() const;
			bool_t HasUniformBuffers() const;

			// pipeline states
			const CullFaceState& GetCullFaceState() const;
			void SetCullFaceState(const CullFaceState& cullFaceState);

			const DepthStencilState& GetDepthStencilState() const;
			void SetDepthStencilState(const DepthStencilState& depthStencilState);

			const ColorBlendState& GetColorBlendState() const;
			void SetColorBlendState(const ColorBlendState& colorBlendState);

			const DynamicState& GetDynamicState() const;
			void SetDynamicState(const DynamicState& dynamicState);
			//

			void SetTransform(const glm::mat4& transform);
			const glm::mat4& GetTransform();

			VisualPass::PrimitiveTopology GetPrimitiveTopology() const;
			void SetPrimitiveTopology(VisualPass::PrimitiveTopology topology);
			VisualPass::FaceWinding GetFaceWinding() const;
			void SetFaceWinding(VisualPass::FaceWinding winding);
			VisualPass::PolygonMode GetPolygonMode() const;
			void SetPolygonMode(VisualPass::PolygonMode mode);

			const VisualPass::RenderData& GetRenderData() const;
			void SetRenderData(const VisualPass::RenderData& data);

			GeometryNode* GetNode();
			void SetNode(GeometryNode* pGeoNode);

			VisualEffect* GetVisualEffect();

			VisualPass::PassType GetPassType() const;

			static std::string PassTypeToStr(VisualPass::PassType type);

			void SkipUniformsSetup();

			void SetIsDebug(bool_t val);
			bool_t GetIsDebug() const;

		protected:
			void SetupRenderData();
			void SetupUniforms();
			void SetupShadowUniforms();
			void BindLights();

			void SetupDefaultPrimitives();
			void SetupDefaultPipelineStates();

			PassType mPassType;

			RenderData mRenderData;

			ShaderMap mShaderMap;
			TextureMap mTextureMap;
			RenderTargetMap mRenderTargetMap;
			UniformBufferMap mUniformBufferMap;

			// pipeline/render states
			CullFaceState mCullFaceState;
			DepthStencilState mDepthStencilState;
			ColorBlendState mColorBlendState;
			DynamicState mDynamicState;
			//

			// transform
			glm::mat4 mTransform;

			VisualPass::PrimitiveTopology mPrimitiveTopology;
			VisualPass::FaceWinding mFaceWinding;
			VisualPass::PolygonMode mPolygonMode;

			VisualEffect* mpVisualEffect;
			GeometryNode* mpGeoNode;

			GADVisualPass* mpGADVisualPass;

			Renderer* mpRenderer;

			bool_t mSkipUniformsSetup;

			bool_t mIsDebug;

		private:
			NO_COPY_NO_MOVE_CLASS(VisualPass)
		};
	}
}

#endif // GRAPHICS_RENDERING_VISUAL_PASSES_VISUAL_PASS_HPP