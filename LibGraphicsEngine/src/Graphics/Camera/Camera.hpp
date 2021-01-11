/* Author: BAIRAC MIHAI */

#ifndef GRAPHICS_CAMERA_HPP
#define GRAPHICS_CAMERA_HPP

#include "AppConfig.hpp"
#include "Foundation/Object.hpp"
#include "glm/common.hpp"
#include "glm/vec3.hpp"
#include "glm/mat4x4.hpp"

#include <string>

//#define USE_INVERSE_MATRICES


//class GlobalConfig;


/*

Transformation pipeline/ Graphics spaces:
Model space -> Model matrix -> World space -> View matrix -> Camera space ->
Projection Matrix -> Homogeneos Clip space -> Perspective division -> NDC/Clip space -> View port transform -> Screen/Window space

* Right-hand coordinate system *
Positive Z axis points to the user (away from the screen)
Positive X azis points to right
Positive Y axis points up

OpenGL usually uses a right handed coordiante system (world)space)

* Left-hand coordinate system *
Positive Z axis points into the screen (away from the user)
Positive X azis points to right
Positive Y axis points up

Vulkan, Direct3D, Metal usually use left handed coordiante system (world space)

** OpenGL **

OpenGL is a right-handed coordinate system in local, world and camera space, but left-handed in NDC/clip space.
The NDC/clip space is left-handed contrary to right-hand, because the handness is changed during the transformation of
the projection matrix. There is a -z multiplication which in turn changes the handedness of the coordinate system.

In Homogeneos Clip space the range is [-w, w] (right before perspective division where we devide by w).

NDC coordinates range in [-1.0f, 1.0f] and so does the depth in NDC.
The final depth ranges in [0.0f, 1.0f] after we get into screen space.

In OpenGL one can use left-handed coordinate system for local/world spaces.

More info: https://stackoverflow.com/questions/4124041/is-opengl-coordinate-system-left-handed-or-right-handed


Default camera frame: camera looks into the negative Z axis, positive right axis and positive up axis.

** Vulkan **

Vulkan is a left-handed coordinate system in local, world and camera spaces, but NDC/clip spaces is a right-hand one.
The same reason why in NDC/Clip space the handedness changes to right (as in OpenGL case).

The perspective projection matrix is different compared to OpenGL.
In Homogeneos Clip space the range is [0, w] (right before perspective division where we devide by w).

NDC coordinates range in [0f, 1.0f] and so does the depth in NDC.
The final depth ranges in [0.0f, 1.0f] after we get into screen space.

Default camera frame: camera looks into the positive Z axis, positive right axis and positive up axis.

More info: https://matthewwellings.com/blog/the-new-vulkan-coordinate-system/

*/

/*
 FPS Free Camera implementation
 Used on several purposes, not only as a viewing camera,
 but also for observation, debugging and projection of the ocean grid
 based on : http://www.opengl-tutorial.org/beginners-tutorials/tutorial-6-keyboard-and-mouse/

 9.005 Are OpenGL matrices column-major or row-major?
 For programming purposes, OpenGL matrices are 16-value arrays with base vectors laid out contiguously in memory.
 The translation components occupy the 13th, 14th, and 15th elements of the 16-element matrix, where indices are
 numbered from 1 to 16 as described in section 2.11.2 of the OpenGL 2.1 Specification.
 Column-major versus row-major is purely a notational convention. Note that post-multiplying with column-major
 matrices produces the same result as pre-multiplying with row-major matrices. The OpenGL Specification and the OpenGL
 Reference Manual both use column-major notation. You can use any notation, as long as it's clearly stated.
 Sadly, the use of column-major format in the spec and blue book has resulted in endless confusion in the OpenGL
 programming community. Column-major notation suggests that matrices are not laid out in memory as a programmer would expect.
 More info here:  http://stackoverflow.com/questions/17717600/confusion-between-c-and-opengl-matrix-order-row-major-vs-column-major
  !!! ORDERING CONVENTION
  GLM uses column major ordering by default, and all vectors are represented as column vectors.
  !!!

  Check this article: https://seanmiddleditch.com/matrices-handedness-pre-and-post-multiplication-row-vs-column-major-and-notations/

  We use the convention GLM library is using:
  - default coordiante system (world) - right handed
  - default matrix, vector memory layout: column major matrices and column vectors
  - default matrix-vector multiplication: post-multiplication v' = M * v;
*/

namespace GraphicsEngine
{
	namespace Graphics
	{
		/* Camera base class */
		class Camera : public Object
		{
			GE_RTTI(GraphicsEngine::Graphics::Camera)

		public:
			//// Methods ////
			Camera();
			explicit Camera(const std::string& name);
			//explicit Camera(const std::string& name, const GlobalConfig& config);
			virtual ~Camera();

			virtual void UpdateViewMatrix();
			virtual void UpdatePerspectiveProjectionMatrix();
			//virtual void UpdatePerspectiveProjectionMatrix(uint32_t windowWidth, uint32_t windowHeight);
			virtual void UpdatePerspectiveProjectionMatrix(int32_t fovy, float32_t aspect, float32_t zNear, float32_t zFar);
			virtual void UpdateOrthographicProjectionMatrix(float32_t left, float32_t right, float32_t bottom, float32_t top, float32_t zNear, float32_t zFar);

			//// Getters ////
			const std::string& GetName() const;

			const glm::vec3& GetPosition() const;
			float32_t GetAltitude() const;
			const glm::vec3& GetForward() const;
			const glm::vec3& GetRight() const;
			const glm::vec3& GetUp() const;

			int32_t GetFOV() const;

			float32_t GetAspectRatio() const;

			float32_t GetZNear() const;
			float32_t GetZFar() const;


			const glm::mat4& GetViewMatrix() const;
			const glm::mat4& GetProjectionMatrix() const;
			const glm::mat4& GetProjectionViewMatrix() const;
#ifdef USE_INVERSE_MATRICES
			const glm::mat4& GetInverseViewMatrix() const;
			const glm::mat4& GetInverseProjectionMatrix() const;
			const glm::mat4& GetInverseProjectionViewMatrix() const;
#endif // USE_INVERSE_MATRICES
			//// Setters ////
			void SetPosition(const glm::vec3& position);
			void SetAltitude(float altitude);
			void SetForward(const glm::vec3& forward);
			void SetRight(const glm::vec3& right);
			void SetUp(const glm::vec3& up);

			void SetFOV(int32_t fov);
			void ResetFOV();

			void SetAspectRatio(float32_t aspectRatio);

			void SetZNear(float32_t zNear);
			void SetZFar(float32_t zFar);

			void SetViewMatrix(const glm::mat4& view);
			void SetProjectionMatrix(const glm::mat4& proj);

			////////////// FPS Camera Interface  ////////////////
			enum class CAMERA_DIRECTIONS : uint8_t
			{
				GE_CD_FORWARD = 0,
				GE_CD_BACKWARD,
				GE_CD_RIGHT,
				GE_CD_LEFT,
				GE_CD_UP,
				GE_CD_DOWN,
				GE_CD_COUNT
			};

			virtual void UpdateOrientationWithMouse(float32_t dx, float32_t dy) {};
			virtual void UpdatePositionWithKeyboard(float32_t value, const Camera::CAMERA_DIRECTIONS& dir) {};

			virtual float32_t GetPitch() const { return 0.0f; };
			virtual float32_t GetYaw() const { return 0.0f; };

		protected:
			float32_t ComputePerspectiveProjectionCorrectionFactor() const;

			//// Variables ////
			std::string mName;

			// POSITION, TARGET
			// glm types self init !!!
			glm::vec3 mPosition; // position in world space
			glm::vec3 mForward;
			glm::vec3 mRight;
			glm::vec3 mUp;

			// MATRICES
			// glm types self init !!!
			glm::mat4 mView; // world -> camera
			glm::mat4 mProjection; // camera -> clip
			glm::mat4 mProjectionView; // world -> clip
#if USE_INVERSE_MATRICES
			glm::mat4 mInverseView; // camera -> world
			glm::mat4 mInverseProjection; // clip -> camera
			glm::mat4 mInverseProjectionView; // clip -> world
#endif // USE_INVERSE_MATRICES

	// Projection
			int32_t mFOVy, mInitialFOVy;
			float32_t mAspectRatio;
			float32_t mZNear;
			float32_t mZFar;

			//TODO - add frustum data, cull methods, etc.
			// or add a CullCmera which culls scene graph nodes based on calculated planes and geometric intersection

		private:
			NO_COPY_NO_MOVE(Camera);
		};
	}
}

#endif /* GRAPHICS_CAMERA_HPP */