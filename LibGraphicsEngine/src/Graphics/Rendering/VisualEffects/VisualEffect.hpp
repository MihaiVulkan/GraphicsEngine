#ifndef GRAPHICS_RENDERING_VISUAL_EFFECTS_VISUAL_EFFECT_HPP
#define GRAPHICS_RENDERING_VISUAL_EFFECTS_VISUAL_EFFECT_HPP

#include "Foundation/Object.hpp"
#include "Foundation/HashUtils.hpp"
#include "Graphics/Rendering/VisualPasses/VisualPass.hpp"
#include <string>
#include <vector>
#include <map>
namespace GraphicsEngine
{
	namespace Graphics
	{
		class Renderer;
		class Camera;

		class Texture;
		class UniformBuffer;

		class GeometryNode;
		class VertexFormat;

		/* Base class for Visual Effects */
		class VisualEffect : public Object
		{
			GE_RTTI(GraphicsEngine::Graphics::VisualEffect)

		public:
			enum class EffectType : uint8_t
			{
				GE_ET_UNLIT,
				GE_ET_LIT, // lit, but no shadows
				GE_ET_LIT_SHADOWS,
				GE_ET_CUSTOM,
				GE_PT_COUNT
			};

			typedef std::map<VisualPass::PassType, std::vector<VisualPass*>> PassMap;

			VisualEffect();
			explicit VisualEffect(VisualEffect::EffectType effectType);
			~VisualEffect();

			virtual void Init();
			virtual void Reset(); // screen resize


			void InitPasses(Renderer* pRenderer);

			const VisualEffect::PassMap& GetPasses() const;

			void SetTargetNode(GeometryNode* pGeoNode);
			GeometryNode* GetTargetNode();

			void AddDependencyNode(GeometryNode* pGeoNode);
			std::vector<GeometryNode*> GetDependencyNodes();

			void SetPrimitiveTopology(VisualPass::PrimitiveTopology topology);
			void SetFaceWinding(VisualPass::FaceWinding winding);
			void SetPolygonMode(VisualPass::PolygonMode mode);

			VisualEffect::EffectType GetEffectType() const;

			static std::string EffectTypeToStr(VisualEffect::EffectType type);

		protected:
			virtual void InitCustomEffect() {};

			EffectType mEffectType;
			std::string mEffectName;

			PassMap mPassMap;

			GeometryNode* mpTargetNode; // node the effect shall be applied on
			std::vector<GeometryNode*> mDependencyNodes; // nodes the effect may depend upon (offscreen effects, etc.)

			VisualPass::PrimitiveTopology mPrimitiveTopology;
			VisualPass::FaceWinding mFaceWinding;
			VisualPass::PolygonMode mPolygonMode;

		private:
			NO_COPY_NO_MOVE_CLASS(VisualEffect)

		};
	}
}

#endif // GRAPHICS_RENDERING_VISUAL_EFFECTS_VISUAL_EFFECT_HPP