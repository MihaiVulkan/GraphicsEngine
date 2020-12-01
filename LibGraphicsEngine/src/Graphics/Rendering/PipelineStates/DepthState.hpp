#ifndef GRAPHICS_RENDERING_PIPELINE_STATES_DEPTH_STATE_HPP
#define GRAPHICS_RENDERING_PIPELINE_STATES_DEPTH_STATE_HPP

#include "Foundation/TypeDefs.hpp"
#include "PipelineState.hpp"

namespace GraphicsEngine
{
    namespace Graphics
    {
        class DepthState : public PipelineState
        {
        public:
            enum class CompareMode : uint8_t
            {
                CM_NEVER,
                CM_LESS,
                CM_EQUAL,
                CM_LEQUAL,
                CM_GREATER,
                CM_NOTEQUAL,
                CM_GEQUAL,
                CM_ALWAYS,
                CM_QUANTITY
            };

            DepthState();
            virtual ~DepthState();

            bool_t GetIsWritable() const;
            void SetIsWritable(bool_t value);

            const DepthState::CompareMode& GetCompareMode() const;
            void SetCompareMode(const DepthState::CompareMode& compareMode);

            //bool mIsEnabled;         // default: true
            bool_t mIsWritable;        // default: true
            CompareMode mCompare;  // default: CM_LEQUAL
        };
    }
}

#endif // GRAPHICS_RENDERING_PIPELINE_STATES_DEPTH_STATE_HPP