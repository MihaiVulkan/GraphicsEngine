#ifndef GRAPHICS_RENDERING_PIPELINE_STATES_PIPELINE_STATE_HPP
#define GRAPHICS_RENDERING_PIPELINE_STATES_PIPELINE_STATE_HPP

#include "Foundation/TypeDefs.hpp"

namespace GraphicsEngine
{
	namespace Graphics
	{
		class PipelineState
		{
		public:
			virtual ~PipelineState();

			void SetIsEnabled(bool_t isEnabled);
			bool_t IsEnabled() const;

		protected:
			explicit PipelineState(bool_t isEnabled);

		private:
			bool_t mIsEnabled;
		};
	}
}

#endif // GRAPHICS_RENDERING_PIPELINE_STATES_PIPELINE_STATE_HPP