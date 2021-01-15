#ifndef GRAPHICS_RENDERING_PIPELINE_STATES_CULL_FACE_STATE_HPP
#define GRAPHICS_RENDERING_PIPELINE_STATES_CULL_FACE_STATE_HPP

#include "Graphics/Rendering/PipelineStates/PipelineState.hpp"

namespace GraphicsEngine
{
	namespace Graphics
	{
		/* Pipline state encapsulating cull face state of geometry */
		class CullFaceState : public PipelineState
		{
			GE_RTTI(GraphicsEngine::Graphics::CullFaceState)

		public:
			enum class CullMode : uint8_t
			{
				GE_CM_NONE = 0,
				GE_CM_FRONT,
				GE_CM_BACK,
				GE_CM_FRONT_AND_BACK,
				GE_CM_COUNT
			};

			CullFaceState();
			explicit CullFaceState(bool_t isEnabled);
			virtual ~CullFaceState();

			void SetCullMode(CullFaceState::CullMode cullMode);
			const CullFaceState::CullMode& GetCullMode() const;

		private:
			CullMode mCullMode;
		};
	}
}

#endif // GRAPHICS_RENDERING_PIPELINE_STATES_CULL_FACE_STATE_HPP