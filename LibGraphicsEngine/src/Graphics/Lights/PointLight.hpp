#ifndef GRAPHICS_LIGHTS_POINT_LIGHT_HPP
#define GRAPHICS_LIGHTS_POINT_LIGHT_HPP

#include "Graphics/Lights/Light.hpp"
#include "glm/vec3.hpp"

namespace GraphicsEngine
{
	namespace Graphics
	{
		/* Point Light class */
		class PointLight : public Light
		{
			GE_RTTI(GraphicsEngine::Graphics::PointLight)

		public:
			PointLight();
			explicit PointLight(const glm::vec3& pos, const Color3f& color);
			virtual ~PointLight();

			virtual const glm::vec3& GetPosition() const override;
			virtual void SetPosition(const glm::vec3& pos) override;

		private:
			NO_COPY_NO_MOVE_CLASS(PointLight)

			virtual void ComputeLightPVM() override;

			glm::vec3 mPosition;
		};
	}
}
#endif // GRAPHICS_LIGHTS_POINT_LIGHT_HPP