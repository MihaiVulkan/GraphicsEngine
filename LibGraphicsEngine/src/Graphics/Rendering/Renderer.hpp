#ifndef GRAPHICS_RENDERING_RENDERER_HPP
#define GRAPHICS_RENDERING_RENDERER_HPP

#include "AppConfig.hpp"
#include "Foundation/TypeDefs.hpp"
#include "Foundation/RTTI.hpp"
#include "Foundation/NoCopyNoMove.hpp"
#include "Foundation/HashUtils.hpp"
#include "Graphics/Rendering/RenderQueue.hpp"
#include <string>
#include <unordered_map>


namespace GraphicsEngine
{
	namespace Platform
	{
		class GE_Window;
	}


	namespace Graphics
	{
		// Graphics API Independent Resources
		class VertexFormat;
		class VertexBuffer;
		class IndexBuffer;
		class UniformBuffer;
		class Texture;
		class RenderTarget;
		class RenderFrameBuffer;
		class Shader;
		class Material;

		// Graphics API Dependent Resources
		class GADRVertexFormat;
		class GADRVertexBuffer;
		class GADRIndexBuffer;
		class GADRUniformBuffer;
		class GADRTexture;
		class GADRRenderTarget;
		class GADRRenderFrameBuffer;
		class GADRShader;
		class GADRMaterial;
		
		class RenderPass;

		class Camera;

		class GeometricPrimitive;
		class GeometryNode;

		/* Rederer is the interface used to access all that is related to rendering
			base renderer class
		*/
		class Renderer : public RTTI
		{
			GE_RTTI(GraphicsEngine::Graphics::Renderer)

		public:
			enum class RendererType : uint8_t
			{
				FORWARD = 0, //TODO - for now we support only forward renderer
				DEFERRED,
				COUNT
			};

			// Resource Types
			typedef std::unordered_map<VertexFormat*, GADRVertexFormat*, HashUtils::PointerHash<VertexFormat>> VertexFormatMap;
			typedef std::unordered_map<VertexBuffer*, GADRVertexBuffer*, HashUtils::PointerHash<VertexBuffer>> VertexBufferMap;
			typedef std::unordered_map<IndexBuffer*, GADRIndexBuffer*, HashUtils::PointerHash<IndexBuffer>> IndexBufferMap;
			typedef std::unordered_map<UniformBuffer*, GADRUniformBuffer*, HashUtils::PointerHash<UniformBuffer>> UniformBufferMap;
			typedef std::unordered_map<Texture*, GADRTexture*, HashUtils::PointerHash<Texture>> TextureMap;
			typedef std::unordered_map<RenderTarget*, GADRRenderTarget*, HashUtils::PointerHash<RenderTarget>> RenderTargetMap;
			typedef std::unordered_map<RenderFrameBuffer*, GADRRenderFrameBuffer*, HashUtils::PointerHash<RenderFrameBuffer>> RenderFrameBufferMap;
			typedef std::unordered_map<Shader*, GADRShader*, HashUtils::PointerHash<Shader>> ShaderMap;
			typedef std::unordered_map<Material*, GADRMaterial*, HashUtils::PointerHash<Material>> MaterialMap;

			Renderer();
			explicit Renderer(Renderer::RendererType type);
			virtual ~Renderer();

			virtual void Init(Platform::GE_Window* pWindow);
			virtual void Terminate();

			virtual void RenderFrame() {};
			virtual void UpdateFrame(bfloat32_t deltaTime) {};
			virtual void SubmitFrame() {};
	

			virtual void WindowResize(uint32_t width = 0, uint32_t height = 0) {};

			void SetCamera(Camera* pCamera) { mpCamera = pCamera; }
			Camera* GetCamera(void) { return mpCamera; }

			Renderer::RendererType GetType() const { return mType; };

			bool_t IsPrepared() { return mIsPrepared; }


			////////////////////

			GADRVertexFormat* Bind(VertexFormat* pVertexFormat);
			void UnBind(VertexFormat* pVertexFormat);

			GADRVertexBuffer* Bind(VertexBuffer* pVertexBuffer);
			void UnBind(VertexBuffer* pVertexBuffer);

			GADRIndexBuffer* Bind(IndexBuffer* pIndexBuffer);
			void UnBind(IndexBuffer* pIndexBuffer);

			GADRUniformBuffer* Bind(UniformBuffer* pUniformBuffer);
			void UnBind(UniformBuffer* pUniformBuffer);

			GADRTexture* Bind(Texture* pTexture);
			void UnBind(Texture* pTexture);

			GADRRenderTarget* Bind(RenderTarget* pRenderTarget);
			void UnBind(RenderTarget* pRenderTarget);

			GADRRenderFrameBuffer* Bind(RenderFrameBuffer* pRenderFrameBuffer);
			void UnBind(RenderFrameBuffer* pRenderFrameBuffer);

			GADRShader* Bind(Shader* pShader);
			void UnBind(Shader* pShader);

			GADRMaterial* Bind(Material* pMaterial, GeometricPrimitive* pGeoPrimitive, uint32_t currentBufferIdx); //TODO
			void UnBind(Material* pMaterial);


			void OnBindMaterial(GADRMaterial* pGADRMaterial, uint32_t currentBufferIdx);

			////////////////////

			//VertexInputState ?
			//InputAssemblyState ?

			//AlphaState* GetAlphaState(); //blend
			//ColorMaskState* GetColorMaskState(); //color
			//DepthStencilState* GetDepthState(); //depth & stencil
			//RasterizerState* GetRasterizerState(); // cull mode & poligon fill mode
			//ViewportState* ViewportState(); // viewport & scissors
			//RenderState* GetRenderState(); //enabled?

			//Pipeline?

			////////////////////

			virtual void Render(RenderQueue* pRenderQueue, RenderPass* pRenderPass) {};
			virtual void Update(Camera* pCamera, bfloat32_t deltaTime) {};

			virtual void UpdateUniformBuffers(RenderQueue::Renderable* pRenderable, Camera* pCamera) {}

			virtual void BindShaderBinds(uint32_t currentBufferIdx) {}

			virtual void DrawObject(RenderQueue::Renderable* pRenderable, uint32_t currentBufferIdx) {}
			virtual void DrawGeometry(GeometryNode* pGeometryNode, uint32_t currentBufferIdx) {};
			virtual void DrawGemetricPrimitve() {};

			////////////////////



		protected:
			virtual void BeginFrame() {};
			virtual void EndFrame() {};

			bool_t mIsPrepared;
			uint32_t mWindowWidth;
			uint32_t mWindowHeight;

			// Resource Management - TODO - to move to a ResourceManager
			// Graphics API Dependent Resource Maps
			VertexFormatMap mVertexFormatMap;
			VertexBufferMap mVertexBufferMap;
			IndexBufferMap mIndexBufferMap;
			UniformBufferMap mUniformBufferMap;
			TextureMap mTextureMap;
			RenderTargetMap mRenderTargetMap;
			RenderFrameBufferMap mRenderFrameBufferMap;
			ShaderMap mShaderMap;
			MaterialMap mMaterialMap;

			RenderQueue* mpRenderQueue;
			RenderPass* mpRenderPass;
			Camera* mpCamera;

		private:
			NO_COPY_NO_MOVE(Renderer)

			void CleanUpResources();

			RendererType mType;
		};
	}
}
#endif /* GRAPHICS_RENDERING_RENDERER_HPP */