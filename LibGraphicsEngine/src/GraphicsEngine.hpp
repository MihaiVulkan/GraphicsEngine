#ifndef GRAPHICS_ENGINE_HPP
#define GRAPHICS_ENGINE_HPP

#include <cstdint> // primary types

#include "Applications/WindowApplication.hpp"

#include "Engine.hpp"
#include "Graphics/GraphicsSystem.hpp"
#include "Input/InputSystem.hpp"

#include "Graphics/Rendering/Resources/VertexFormat.hpp"
#include "Graphics/Rendering/Resources/VertexBuffer.hpp"
#include "Graphics/Rendering/Resources/IndexBuffer.hpp"
#include "Graphics/Rendering/Resources/Shader.hpp"
#include "Graphics/Rendering/Resources/Texture.hpp"
#include "Graphics/Rendering/Resources/Material.hpp"
#include "Graphics/Rendering/Resources/UniformBuffer.hpp"

#include "Graphics/GeometricPrimitives/GeometricPrimitive.hpp"

#include "Graphics/Components/NodeComponent.hpp"
#include "Graphics/Components/MaterialComponent.hpp"

#include "Graphics/SceneGraph/Node.hpp"
#include "Graphics/SceneGraph/GroupNode.hpp"
#include "Graphics/SceneGraph/GeometryNode.hpp"
#include "Graphics/SceneGraph/CameraNode.hpp"

#include "Graphics/Rendering/Vulkan/VulkanRenderer.hpp"

#include "Graphics/Camera/Camera.hpp"

#include "Foundation/Logger.hpp"




namespace GraphicsEngine
{
	void Init();

}


#endif // GRAPHICS_ENGINE_HPP