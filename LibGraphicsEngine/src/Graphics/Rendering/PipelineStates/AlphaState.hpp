#ifndef GRAPHICS_RENDERING_PIPELINE_STATES_ALPHA_STATE_HPP
#define GRAPHICS_RENDERING_PIPELINE_STATES_ALPHA_STATE_HPP

#include "Foundation/TypeDefs.hpp"
#include "PipelineState.hpp"

namespace GraphicsEngine
{
	namespace Graphics
	{
		class AlphaState : public PipelineState
		{
		public:
            enum class SrcBlendMode : uint8_t
            {
                SBM_ZERO = 0,
                SBM_ONE,
                SBM_DST_COLOR,
                SBM_ONE_MINUS_DST_COLOR,
                SBM_SRC_ALPHA,
                SBM_ONE_MINUS_SRC_ALPHA,
                SBM_DST_ALPHA,
                SBM_ONE_MINUS_DST_ALPHA,
                SBM_SRC_ALPHA_SATURATE,
                SBM_CONSTANT_COLOR,
                SBM_ONE_MINUS_CONSTANT_COLOR,
                SBM_CONSTANT_ALPHA,
                SBM_ONE_MINUS_CONSTANT_ALPHA,
                SBM_COUNT
            };

            enum class DstBlendMode : uint8_t
            {
                DBM_ZERO = 0,
                DBM_ONE,
                DBM_SRC_COLOR,
                DBM_ONE_MINUS_SRC_COLOR,
                DBM_SRC_ALPHA,
                DBM_ONE_MINUS_SRC_ALPHA,
                DBM_DST_ALPHA,
                DBM_ONE_MINUS_DST_ALPHA,
                DBM_CONSTANT_COLOR,
                DBM_ONE_MINUS_CONSTANT_COLOR,
                DBM_CONSTANT_ALPHA,
                DBM_ONE_MINUS_CONSTANT_ALPHA,
                DBM_COUNT
            };

            enum class CompareMode : uint8_t
            {
                CM_NEVER = 0,
                CM_LESS,
                CM_EQUAL,
                CM_LEQUAL,
                CM_GREATER,
                CM_NOTEQUAL,
                CM_GEQUAL,
                CM_ALWAYS,
                CM_COUNT
            };

            AlphaState();
            virtual ~AlphaState();

            const SrcBlendMode& GetSrcBlendMode() const;
            void SetSrcBlendMode(const SrcBlendMode& srcBlendMode);

            const DstBlendMode& GetDstBlendMode() const;
            void SetDstBlendMode(const DstBlendMode& dstBlendMode);

            bool_t IsComparedEnabled() const;
            void SetIsComparedEnabled(bool_t value);

            const CompareMode& GetCompareMode() const;
            void SetCompareMode(const CompareMode& compareMode);

            bfloat32_t GetReference() const;
            void SetReference(bfloat32_t ref);

            const Color4f& GetConstantColor() const;
            void SetConstantColor(const Color4f& color);
    
        private:
            //bool_t mIsBlendEnabled;      // default: false
            SrcBlendMode mSrcBlendMode;  // default: SBM_SRC_ALPHA
            DstBlendMode mDstBlendMode;  // default: DBM_ONE_MINUS_SRC_ALPHA
            bool_t mIsCompareEnabled;    // default: false
            CompareMode mCompareMode;    // default: CM_ALWAYS
            bfloat32_t mReference;        // default: 0, always in [0,1]
            Color4f mConstantColor;   // default: (0,0,0,0)

		};
	}
}

#endif // GRAPHICS_RENDERING_PIPELINE_STATES_ALPHA_STATE_HPP