#ifndef GRAPHICS_RENDERING_BACKENDS_OPENGL_COMMON_OPENGL_COMMON_HPP
#define GRAPHICS_RENDERING_BACKENDS_OPENGL_COMMON_OPENGL_COMMON_HPP

#if defined(OPENGL_RENDERER)
#define VERTEX_BUFFER_BIND_ID 0 //NOTE! Not reflected in shader
#define INSTANCE_BUFFER_BIND_ID 1

#define UNIFORM_BINDING_ID 0

//#define SHADER_ENTRY_POINT "main"

/* Number of samples needs to be the same at image creation,      */
/* renderpass creation and pipeline creation.                     */
#define GL_SAMPLE_COUNT_1 1
#define GL_SAMPLE_COUNT_2 2
#define GL_SAMPLE_COUNT_4 4
#define GL_SAMPLE_COUNT_8 8
#define GL_SAMPLE_COUNT_16 16
#define GL_SAMPLE_COUNT_32 32

#define MIN_NUM_SAMPLES GL_SAMPLE_COUNT_1

/* Number of viewports and number of scissors have to be the same */
/* at pipeline creation and in any call to set them dynamically   */
/* They also have to be the same as each other                    */
#define MIN_NUM_VIEWPORTS 1
#define MIN_NUM_SCISSORS MIN_NUM_VIEWPORTS


#define COLOR_ATT 0
#define DEPTH_ATT 1
#define ATT_COUNT 2

#define CLEARVALUES_COUNT 2
#endif // OPENGL_RENDERER

#endif // GRAPHICS_RENDERING_BACKENDS_OPENGL_COMMON_OPENGL_COMMON_HPP