#ifndef GRAPHICS_RENDERING_PIPELINE_STATES_DEPTH_STATE_HPP
#define GRAPHICS_RENDERING_PIPELINE_STATES_DEPTH_STATE_HPP

#include "PipelineState.hpp"

namespace GraphicsEngine
{
    namespace Graphics
    {
        /* Pipline state encapsulating depth/stencil tests */
        class DepthStencilState : public PipelineState
        {
            GE_RTTI(GraphicsEngine::Graphics::DepthStencilState)

        public:
            enum class CompareOp : uint8_t
            {
                GE_CO_NEVER = 0,
                GE_CO_LESS,
                GE_CO_EQUAL,
                GE_CO_LESS_OR_EQUAL,
                GE_CO_GREATER,
                GE_CO_NOT_EQUAL,
                GE_CO_GREATER_OR_EQUAL,
                GE_CO_ALWAYS,
                GE_CO_COUNT
            };

            enum class StencilOp : uint8_t
            {
                GE_SO_KEEP = 0,
                GE_SO_ZERO,
                GE_SO_REPLACE,
                GE_SO_INCREMENT_AND_CLAMP,
                GE_SO_DECREMENT_AND_CLAMP,
                GE_SO_INVERT,
                GE_SO_INCREMENT_AND_WRAP,
                GE_SO_DECREMENT_AND_WRAP,
                GE_SO_COUNT
            };

            DepthStencilState();
            explicit DepthStencilState(bool_t isEnabled);
            virtual ~DepthStencilState();

            const bool_t& GetIsDepthEnabled() const;
            void SetIsDepthEnabled(bool_t value);

            const bool_t& GetIsDepthWritable() const;
            void SetIsDepthWritable(bool_t value);

            const DepthStencilState::CompareOp& GetDepthCompareOp() const;
            void SetDepthCompareOp(DepthStencilState::CompareOp compOp);

            const bool_t& GetIsStencilEnabled() const;
            void SetIsStencilEnabled(bool_t value);

            const DepthStencilState::StencilOp& GetStencilFailOp() const;
            void SetStencilFailOp(DepthStencilState::StencilOp stencilOp);

            const DepthStencilState::StencilOp& GetStencilPassDepthPassOp() const;
            void SetStencilPassDepthPassOp(DepthStencilState::StencilOp stencilOp);

            const DepthStencilState::StencilOp& GetStencilPassDepthFailOp() const;
            void SetStencilPassDepthFailOp(DepthStencilState::StencilOp stencilOp);

            const DepthStencilState::CompareOp& GetStencilCompareOp() const;
            void SetStencilCompareOp(DepthStencilState::CompareOp compOp);

            const uint32_t& GetStencilCompareMask() const;
            void SetStencilCompareMask(uint32_t mask);

            const uint32_t& GetStencilWriteMask() const;
            void SetStencilWriteMask(uint32_t mask);

            const uint32_t& GetStencilReference() const;
            void SetStencilReference(uint32_t ref);

        private:
            bool_t      mIsDepthEnabled;
            bool_t      mIsDepthWritable;
            CompareOp   mDepthCompareOp;

            bool_t       mIsStencilEnabled;
            StencilOp    mStencilFailOp;
            StencilOp    mStencilPassDepthPassOp;
            StencilOp    mStencilPassDepthFailOp;
            CompareOp    mStencilCompareOp;
            uint32_t     mStencilCompareMask;
            uint32_t     mStencilWriteMask;
            uint32_t     mStencilReference;
        };
    }
}

#endif // GRAPHICS_RENDERING_PIPELINE_STATES_DEPTH_STATE_HPP