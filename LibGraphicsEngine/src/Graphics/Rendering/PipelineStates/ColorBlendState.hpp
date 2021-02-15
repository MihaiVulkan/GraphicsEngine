#ifndef GRAPHICS_RENDERING_PIPELINE_STATES_COLOR_BLEND_STATE_HPP
#define GRAPHICS_RENDERING_PIPELINE_STATES_COLOR_BLEND_STATE_HPP

#include "Graphics/Rendering/PipelineStates/PipelineState.hpp"

namespace GraphicsEngine
{
	namespace Graphics
	{
        /* Pipline state encapsulating alpha/color blending */
		class ColorBlendState : public PipelineState
		{
            GE_RTTI(GraphicsEngine::Graphics::ColorBlendState)

		public:
            enum class BlendFactor : uint8_t
            {
                GE_BF_ZERO = 0,
                GE_BF_ONE,
                GE_BF_SRC_COLOR,
                GE_BF_ONE_MINUS_SRC_COLOR,
                GE_BF_DST_COLOR,
                GE_BF_ONE_MINUS_DST_COLOR,
                GE_BF_SRC_ALPHA,
                GE_BF_ONE_MINUS_SRC_ALPHA,
                GE_BF_DST_ALPHA,
                GE_BF_ONE_MINUS_DST_ALPHA,
                GE_BF_CONSTANT_COLOR,
                GE_BF_ONE_MINUS_CONSTANT_COLOR,
                GE_BF_CONSTANT_ALPHA,
                GE_BF_ONE_MINUS_CONSTANT_ALPHA,
            //    GE_BF_SRC_ALPHA_SATURATE,
            //    GE_BF_SRC1_COLOR,
            //    GE_BF_ONE_MINUS_SRC1_COLOR,
            //    GE_BF_SRC1_ALPHA,
            //    GE_BF_ONE_MINUS_SRC1_ALPHA,
                GE_BF_COUNT
            };

            enum class BlendOp : uint8_t
            {
                GE_BO_ADD = 0,
                GE_BO_SUBTRAC,
                GE_BO_REVERSE_SUBTRACT,
                GE_BO_MIN,
                GE_BO_COUNT
            };

            ColorBlendState();
            explicit ColorBlendState(bool_t isEnabled);
            virtual ~ColorBlendState();

            const bool_t& GetIsBlendEnabled() const;
            void SetIsBlendEnabled(bool_t value);

            const ColorBlendState::BlendFactor& GetSrcColorBlendFactor() const;
            void SetSrcColorBlendFactor(const ColorBlendState::BlendFactor& factor);

            const ColorBlendState::BlendFactor& GetDstColorBlendFactor() const;
            void SetDstColorBlendFactor(const ColorBlendState::BlendFactor& factor);

            const ColorBlendState::BlendOp& GetColorBlendOp() const;
            void SetColorBlendOp(const ColorBlendState::BlendOp& blendOp);

            const ColorBlendState::BlendFactor& GetSrcAlphaBlendFactor() const;
            void SetSrcAlphaBlendFactor(const ColorBlendState::BlendFactor& factor);

            const ColorBlendState::BlendFactor& GetDstAlphaBlendFactor() const;
            void SetDstAlphaBlendFactor(const ColorBlendState::BlendFactor& factor);

            const ColorBlendState::BlendOp& GetAlphaBlendOp() const;
            void SetAlphaBlendOp(const ColorBlendState::BlendOp& blendOp);

            const uint32_t& GetColorWriteMask() const;
            void SetColorWriteMask(uint32_t mask);

            const Color4f& GetConstantColor() const;
            void SetConstantColor(const Color4f& color);
    
        private:
            //NO_COPY_NO_MOVE_CLASS(ColorBlendState)

            bool_t mIsBlendEnabled;
            BlendFactor mSrcColorBlendFactor;
            BlendFactor mDstColorBlendFactor;
            BlendOp mColorBlendOp;
            BlendFactor mSrcAlphaBlendFactor;
            BlendFactor mDstAlphaBlendFactor;
            BlendOp mAlphaBlendOp;
            uint32_t mColorWriteMask;
            Color4f mConstantColor; 

		};
	}
}

#endif // GRAPHICS_RENDERING_PIPELINE_STATES_COLOR_BLEND_STATE_HPP