#ifndef GRAPHICS_COMPONENTS_VISUAL_COMPONENT_HPP
#define GRAPHICS_COMPONENTS_VISUAL_COMPONENT_HPP

#include "Graphics/Components/NodeComponent.hpp"
#include "Graphics/Rendering/Resources/Shader.hpp"
#include "Graphics/Rendering/Resources/Texture.hpp"
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
			typedef std::unordered_map<Shader::ShaderStage, Texture*> TextureMap;
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
			Shader* GetShader(Shader::ShaderStage stage);
			void SetShaders(const VisualComponent::ShaderMap& shaders);
			const VisualComponent::ShaderMap& GetShaders() const;

			void AddTexture(Texture* pTexture, Shader::ShaderStage shaderStage);
			bool_t HasTextures() const;
			const VisualComponent::TextureMap& GetTextures() const;

			void AddUniformBuffer(Shader::ShaderStage stage, UniformBuffer* pUniformBuffer);
			UniformBuffer* GetUniformBuffer(Shader::ShaderStage stage) const;
			bool_t HasUniformBuffers() const;

			// pipeline states
			CullFaceState& GetCullFaceState();
			void SetCullFaceState(const CullFaceState& cullFaceState);

			DepthStencilState& GetDepthStencilState();
			void SetDepthStencilState(const DepthStencilState& depthStencilState);

			ColorBlendState& GetColorBlendState();
			void SetColorBlendState(const ColorBlendState& colorBlendState);

			DynamicState& GetDynamicState();
			void SetDynamicState(const DynamicState& dynamicState);


		private:
			NO_COPY_NO_MOVE_CLASS(VisualComponent)

			void Create();
			void Destroy();

			ShaderMap mShaderMap;
			TextureMap mTextureMap;
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