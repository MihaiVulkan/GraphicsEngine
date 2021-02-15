#ifndef GRAPHICS_RENDERING_PIPELINE_STATES_PIPELINE_STATE_HPP
#define GRAPHICS_RENDERING_PIPELINE_STATES_PIPELINE_STATE_HPP

#include "Foundation/Object.hpp"

namespace GraphicsEngine
{
	namespace Graphics
	{
		/* Base Pipline state class */
		class PipelineState : public Object
		{
			GE_RTTI(GraphicsEngine::Graphics::PipelineState)

		public:
			PipelineState();
			explicit PipelineState(bool_t isEnabled);
			virtual ~PipelineState();

			void SetIsEnabled(bool_t isEnabled);
			bool_t GetIsEnabled() const;

		protected:
			//NO_COPY_NO_MOVE_CLASS(PipelineState)

			bool_t mIsEnabled;
		};
	}
}

#endif // GRAPHICS_RENDERING_PIPELINE_STATES_PIPELINE_STATE_HPP