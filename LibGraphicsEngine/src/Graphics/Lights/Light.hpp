#ifndef GRAPHICS_LIGHTS_LIGHT_HPP
#define GRAPHICS_LIGHTS_LIGHT_HPP

#include "Foundation/Object.hpp"

namespace GraphicsEngine
{
	namespace Graphics
	{
		/* Light class */
		class Light : public Object
		{
			GE_RTTI(GraphicsEngine::Graphics::Light)

		public:
			enum class LightType : uint8_t
			{
				GE_LT_DIRECTIONAL = 0,
				GE_LT_POINT,
				GE_LT_SPOT,
				GE_PT_COUNT
			};

			Light();
			explicit Light(LightType type);
			virtual ~Light();

			const Light::LightType& GetLightType() const;
			void SetLightType(const Light::LightType& type);

			virtual const glm::vec3& GetDirection() const { return glm::vec3(0.0f); };
			virtual void SetDirection(const glm::vec3& dir) {};

			virtual const glm::vec3& GetColor() const { return glm::vec3(0.0f); };
			virtual void SetColor(const glm::vec3& color) {};

		protected:
			LightType mLightType;

		private:
			NO_COPY_NO_MOVE_CLASS(Light)
		};
	}
}
#endif // GRAPHICS_LIGHTS_LIGHT_HPP