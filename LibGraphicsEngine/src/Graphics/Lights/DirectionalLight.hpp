#ifndef GRAPHICS_LIGHTS_DIRECTIONAL_LIGHT_HPP
#define GRAPHICS_LIGHTS_DIRECTIONAL_LIGHT_HPP

#include "Graphics/Lights/Light.hpp"
#include "glm/vec3.hpp"

namespace GraphicsEngine
{
	namespace Graphics
	{
		/* Directional Light class */
		class DirectionalLight : public Light
		{
			GE_RTTI(GraphicsEngine::Graphics::DirectionalLight)

		public:
			DirectionalLight();
			explicit DirectionalLight(const glm::vec3& dir, const glm::vec3& color);
			virtual ~DirectionalLight();

			virtual const glm::vec3& GetDirection() const override;
			virtual void SetDirection(const glm::vec3& dir) override;

			virtual const glm::vec3& GetColor() const override;
			virtual void SetColor(const glm::vec3& color) override;

		private:
			NO_COPY_NO_MOVE_CLASS(DirectionalLight)

			glm::vec3 mDirection;
			glm::vec3 mColor;
		};
	}
}
#endif // GRAPHICS_LIGHTS_DIRECTIONAL_LIGHT_HPP