#ifndef CORE_GRAPHICS_ENGINE_HPP
#define CORE_GRAPHICS_ENGINE_HPP

#include "Applications/WindowApplication.hpp"

#include "Core/Engine.hpp"
#include "Graphics/GraphicsSystem.hpp"
#include "Input/InputSystem.hpp"

#include "Graphics/Rendering/Resources/VertexFormat.hpp"
#include "Graphics/Rendering/Resources/VertexBuffer.hpp"
#include "Graphics/Rendering/Resources/IndexBuffer.hpp"
#include "Graphics/Rendering/Resources/UniformBuffer.hpp"
#include "Graphics/Rendering/Resources/Shader.hpp"
#include "Graphics/Rendering/Resources/Texture.hpp"
#include "Graphics/Rendering/Resources/RenderTarget.hpp"
#include "Graphics/Rendering/Resources/Material.hpp"
#include "Graphics/Rendering/Resources/Model.hpp"

#include "Graphics/ShaderTools/GLSL/GLSLShaderTypes.hpp"

#include "Graphics/Rendering/PipelineStates/CullFaceState.hpp"
#include "Graphics/Rendering/PipelineStates/DepthStencilState.hpp"
#include "Graphics/Rendering/PipelineStates/ColorBlendState.hpp"
#include "Graphics/Rendering/PipelineStates/DynamicState.hpp"

#include "Graphics/Rendering/VisualEffects/VisualEffect.hpp"
// Unlit effects
#include "Graphics/Rendering/VisualEffects/UnlitEffects/UnlitColorAttributeVisualEffect.hpp"
#include "Graphics/Rendering/VisualEffects/UnlitEffects/UnlitColorVisualEffect.hpp"
#include "Graphics/Rendering/VisualEffects/UnlitEffects/Unlit2DTextureVisualEffect.hpp"
#include "Graphics/Rendering/VisualEffects/UnlitEffects/Unlit2DTextureArrayVisualEffect.hpp"
#include "Graphics/Rendering/VisualEffects/UnlitEffects/UnlitCubemapTextureVisualEffect.hpp"
// Lit effects
#include "Graphics/Rendering/VisualEffects/LitEffects/LitColorAttributeVisualEffect.hpp"
#include "Graphics/Rendering/VisualEffects/LitEffects/LitColorVisualEffect.hpp"
#include "Graphics/Rendering/VisualEffects/LitEffects/Lit2DTextureVisualEffect.hpp"
#include "Graphics/Rendering/VisualEffects/LitEffects/LitEnvironmentMappingTextureVisualEffect.hpp"
#include "Graphics/Rendering/VisualEffects/LitEffects/LitMirrorTextureVisualEffect.hpp"
#include "Graphics/Rendering/VisualEffects/LitEffects/LitShadowColorAttributeVisualEffect.hpp"

#include "Graphics/Loaders/KTX2Loader.hpp"
#include "Graphics/Loaders/glTF2Loader.hpp"

#include "Graphics/GeometricPrimitives/GeometricPrimitive.hpp"
#include "Graphics/Lights/DirectionalLight.hpp"
#include "Graphics/Lights/PointLight.hpp"

#include "Graphics/Components/NodeComponent.hpp"
#include "Graphics/Components/VisualComponent.hpp"

#include "Graphics/SceneGraph/Node.hpp"
#include "Graphics/SceneGraph/GroupNode.hpp"
#include "Graphics/SceneGraph/GeometryNode.hpp"
#include "Graphics/SceneGraph/LightNode.hpp"
#include "Graphics/SceneGraph/CameraNode.hpp"

#include "Graphics/Cameras/Camera.hpp"
#include "Foundation/Logger.hpp"

//TODO - to be replaced when I add my own Math lib
//Math stuff
#include "glm/common.hpp"
#include "glm/vec3.hpp"
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"

#endif // CORE_GRAPHICS_ENGINE_HPP