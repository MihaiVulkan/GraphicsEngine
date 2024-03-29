#ifndef GRAPHICS_RENDERING_RENDERER_HPP
#define GRAPHICS_RENDERING_RENDERER_HPP

#include "Core/AppConfig.hpp"
#include "Foundation/Object.hpp"
#include "Foundation/HashUtils.hpp"
#include "Graphics/Rendering/RenderQueue.hpp"
#include <string>
#include <unordered_map>


namespace GraphicsEngine
{
	namespace Platform
	{
		class Window;
	}


	namespace Graphics
	{
		// Graphics API Independent Resources
		// Visual Pass
		class VisualPass;

		class RenderTarget;
		class VertexFormat;
		class VertexBuffer;
		class IndexBuffer;
		class UniformBuffer;
		class Texture;
		class Shader;
		class Material;
		class Model;

		// Pipeline States
		class DynamicState;

		// Graphics API Dependent 
		class GADVisualPass;

		//Resources
		class GADRVertexFormat;
		class GADRVertexBuffer;
		class GADRIndexBuffer;
		class GADRUniformBuffer;
		class GADRTexture;
		class GADRRenderTarget;
		class GADRShader;
		class GADRMaterial;
		class GADRModel;

		class GeometryNode;
		class LightNode;

		/* Rederer is the interface used to access all that is related to rendering
			base renderer class
		*/
		class Renderer : public Object
		{
			GE_RTTI(GraphicsEngine::Graphics::Renderer)

		public:
			enum class RendererType : uint8_t
			{
				GE_RT_FORWARD = 0, //TODO - for now we support only forward renderer
				GE_RT_DEFERRED,
				GE_RT_COUNT
			};

			typedef std::unordered_map<VisualPass*, GADVisualPass*> VisualPassMap;

			// Resource Types
			typedef std::unordered_map<RenderTarget*, GADRTexture*, HashUtils::PointerHash<RenderTarget>> RenderTargetMap;
			typedef std::unordered_map<VertexFormat*, GADRVertexFormat*, HashUtils::PointerHash<VertexFormat>> VertexFormatMap;
			typedef std::unordered_map<VertexBuffer*, GADRVertexBuffer*, HashUtils::PointerHash<VertexBuffer>> VertexBufferMap;
			typedef std::unordered_map<IndexBuffer*, GADRIndexBuffer*, HashUtils::PointerHash<IndexBuffer>> IndexBufferMap;
			typedef std::unordered_map<UniformBuffer*, GADRUniformBuffer*, HashUtils::PointerHash<UniformBuffer>> UniformBufferMap;
			typedef std::unordered_map<Texture*, GADRTexture*, HashUtils::PointerHash<Texture>> TextureMap;
			typedef std::unordered_map<Shader*, GADRShader*, HashUtils::PointerHash<Shader>> ShaderMap;
			typedef std::unordered_map<Material*, GADRMaterial*, HashUtils::PointerHash<Material>> MaterialMap;
			typedef std::unordered_map<Model*, GADRModel*, HashUtils::PointerHash<Model>> ModelMap;


			Renderer();
			explicit Renderer(Platform::Window* pWindow, Renderer::RendererType type = Renderer::RendererType::GE_RT_FORWARD);
			virtual ~Renderer();

			virtual void RenderFrame(RenderQueue* pRenderQueue) {};
			virtual void UpdateFrame(Camera* pCamera, float32_t crrTime) {};
			virtual void SubmitFrame() {};
	
			virtual void OnWindowResize(uint32_t width = 0, uint32_t height = 0) {};

			virtual void ComputeGraphicsResources(RenderQueue* pRenderQueue) {};

			virtual void BindLight(VisualPass* pVisualPass, const LightNode* pLightNode, GeometryNode* pGeoNode) {};

			virtual void DrawNode(VisualPass* pVisualPass, GeometryNode* pGeoNode, uint32_t currentBufferIdx) {}
			virtual void UpdateNode(VisualPass* pVisualPass, GeometryNode* pGeoNode, Camera* pCamera, float32_t crrTime) {}

			// cleanup Graphics API Independed Resources
			virtual void CleanUpGAIR();

			uint32_t GetWindowWidth() const;
			uint32_t GetWindowHeight() const;
	
			Renderer::RendererType GetRendererType() const { return mRendererType; };

			bool_t IsPrepared() { return mIsPrepared; }

			RenderQueue* GetRenderQueue() { return mpRenderQueue; }

			///////////////////////////

			GADVisualPass* Get(VisualPass* pVisualPass);


			GADRVertexFormat* Bind(VertexFormat* pVertexFormat);
			void UnBind(VertexFormat* pVertexFormat);

			GADRVertexFormat* Get(VertexFormat* pVertexFormat);

			GADRVertexBuffer* Bind(VertexBuffer* pVertexBuffer, uint32_t currentBufferIdx);
			void UnBind(VertexBuffer* pVertexBuffer);

			GADRVertexBuffer* Get(VertexBuffer* pVertexBuffer);

			GADRIndexBuffer* Bind(IndexBuffer* pIndexBuffer, uint32_t currentBufferIdx);
			void UnBind(IndexBuffer* pIndexBuffer);

			GADRIndexBuffer* Get(IndexBuffer* pIndexBuffer);

			GADRUniformBuffer* Bind(UniformBuffer* pUniformBuffer);
			void UnBind(UniformBuffer* pUniformBuffer);

			GADRUniformBuffer* Get(UniformBuffer* pUniformBuffer);

			GADRTexture* Bind(Texture* pTexture);
			void UnBind(Texture* pTexture);

			GADRTexture* Get(Texture* pTexture);

			GADRTexture* Bind(RenderTarget* pRenderTarget);
			void UnBind(RenderTarget* pRenderTarget);

			GADRTexture* Get(RenderTarget* pRenderTarget);

			GADRShader* Bind(Shader* pShader);
			void UnBind(Shader* pShader);

			GADRShader* Get(Shader* pShader);

			GADRMaterial* Bind(Material* pMaterial);
			void UnBind(Material* pMaterial);

			GADRMaterial* Get(Material* pMaterial);

			GADRModel* Bind(Model* pModel);
			void UnBind(Model* pModel);

			GADRModel* Get(Model* pModel);


		protected:

			virtual void Init(Platform::Window* pWindow);
			virtual void Terminate();

			virtual void BeginFrame() {};
			virtual void EndFrame() {};

			///////////////////////////////

			bool_t mIsPrepared;
			uint32_t mWindowWidth;
			uint32_t mWindowHeight;

			// Resource Management - TODO - to move to a ResourceManager
			// Graphics API Dependent Resource Maps
			VisualPassMap mVisualPassMap;

			VertexFormatMap mVertexFormatMap;
			VertexBufferMap mVertexBufferMap;
			IndexBufferMap mIndexBufferMap;
			UniformBufferMap mUniformBufferMap;
			TextureMap mTextureMap;
			RenderTargetMap mRenderTargetMap;
			ShaderMap mShaderMap;
			MaterialMap mMaterialMap;
			ModelMap mModelMap;

			RenderQueue* mpRenderQueue;
			Camera* mpCamera;

		private:
			NO_COPY_NO_MOVE_CLASS(Renderer)

			void CleanUpResources();

			RendererType mRendererType;
		};
	}
}
#endif /* GRAPHICS_RENDERING_RENDERER_HPP */