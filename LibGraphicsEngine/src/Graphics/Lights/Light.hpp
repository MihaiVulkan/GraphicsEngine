#ifndef GRAPHICS_LIGHTS_LIGHT_HPP
#define GRAPHICS_LIGHTS_LIGHT_HPP

#include "Foundation/Object.hpp"
#include "glm/mat4x4.hpp"

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

			virtual const glm::vec3& GetPosition() const { return glm::vec3(0.0f); };
			virtual void SetPosition(const glm::vec3& pos) {};

			virtual const Color3f& GetColor() const;
			virtual void SetColor(const Color3f& color);

			const glm::mat4& GetLightPVM() const;
			virtual void ComputeLightPVM() {}

		protected:
			LightType mLightType;

			Color3f mColor;

			glm::mat4 mLightPVM;

		private:
			NO_COPY_NO_MOVE_CLASS(Light)
		};
	}
}
#endif // GRAPHICS_LIGHTS_LIGHT_HPP