#ifndef GRAPHICS_RENDERING_PIPELINE_STATES_DYNAMIC_STATE_HPP
#define GRAPHICS_RENDERING_PIPELINE_STATES_DYNAMIC_STATE_HPP

#include "Graphics/Rendering/PipelineStates/PipelineState.hpp"
#include <vector>

namespace GraphicsEngine
{
	namespace Graphics
	{
		/* Pipline state encapsulating Vulkan dynamic states, TODO - changes this as it is API specific */
		class DynamicState : public PipelineState
		{
			GE_RTTI(GraphicsEngine::Graphics::DynamicState)

		public:
			enum class State : uint8_t
			{
				GE_DS_VIEWPORT = 0,
				GE_DS_SCISSOR,
				GE_DS_LINE_WIDTH,
				GE_DS_DEPTH_BIAS,
				GE_DS_BLEND_CONSTANTS,
				GE_DS_DEPTH_BOUNDS,
				GE_DS_STENCIL_COMPARE_MASK,
				GE_DS_STENCIL_WRITE_MASK,
				GE_DS_STENCIL_REFERENCE,
				GE_DS_COUNT
			};

			DynamicState();
			explicit DynamicState(bool_t isEnabled);
			virtual ~DynamicState();

			void Add(DynamicState::State state);

			bool_t HasStates() const;

			const std::vector<DynamicState::State>& GetStates() const;

		private:
			std::vector<DynamicState::State> mStates;
		};
	}
}

#endif // GRAPHICS_RENDERING_PIPELINE_STATES_DYNAMIC_STATE_HPP