#ifndef GRAPHICS_COMPONENTS_VISUAL_COMPONENT_HPP
#define GRAPHICS_COMPONENTS_VISUAL_COMPONENT_HPP

#include "Graphics/Components/NodeComponent.hpp"
#include "Graphics/Rendering/Resources/Shader.hpp"
#include "Graphics/Rendering/PipelineStates/CullFaceState.hpp"
#include "Graphics/Rendering/PipelineStates/DepthStencilState.hpp"
#include "Graphics/Rendering/PipelineStates/ColorBlendState.hpp"
#include "Graphics/Rendering/PipelineStates/DynamicState.hpp"
#include <unordered_map>

namespace GraphicsEngine
{
	namespace Graphics
	{
		class UniformBuffer;
		
		/* VisualComponent is the base class for all visual node components 
		   All redenderable/visual elements must have this component present. */
		class VisualComponent : public NodeComponent
		{
			GE_RTTI(GraphicsEngine::Graphics::VisualComponent)

		public:
			typedef std::unordered_map<Shader::ShaderStage, Shader*> ShaderMap;
			typedef std::unordered_map<Shader::ShaderStage, UniformBuffer*> UniformBufferMap;

			VisualComponent();
			explicit VisualComponent(const std::string& name);
			virtual ~VisualComponent();

			virtual void Start() override;
			virtual void Update(float32_t deltaTime) override;

			virtual void OnAttach() override;
			virtual void OnDettach() override;

			/////////////////////

			void AddShader(Shader* pShader);

			Shader* GetShader(Shader::ShaderStage stage) const;

			void SetShaders(const VisualComponent::ShaderMap& shaders);
			const VisualComponent::ShaderMap& GetShaders() const;

			void AddUniformBuffer(Shader::ShaderStage stage, UniformBuffer* pUniformBuffer);
			UniformBuffer* GetUniformBuffer(Shader::ShaderStage stage) const;
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


		private:
			void Create();
			void Destroy();

			ShaderMap mShaderMap;
			UniformBufferMap mUniformBufferMap;

			// pipeline/render states
			CullFaceState mCullFaceState;
			DepthStencilState mDepthStencilState;
			ColorBlendState mColorBlendState;
			DynamicState mDynamicState;
		};
	}
}


#endif // GRAPHICS_COMPONENTS_VISUAL_COMPONENT_HPP