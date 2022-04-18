#ifndef GRAPHICS_RENDERING_BACKENDS_VULKAN_COMMON_VULKAN_COMMON_HPP
#define GRAPHICS_RENDERING_BACKENDS_VULKAN_COMMON_VULKAN_COMMON_HPP

#define VERTEX_BUFFER_BIND_ID 0 //NOTE! Not reflected in shader
#define INSTANCE_BUFFER_BIND_ID 1

#define NUM_DESCRIPTOR_SETS 1
#define UNIFORM_BINDING_ID 0

#define SHADER_ENTRY_POINT "main"
#define VERTEX_INPUT_BIDING_ID 0

/* Number of samples needs to be the same at image creation,      */
/* renderpass creation and pipeline creation.                     */
#define MIN_NUM_SAMPLES VkSampleCountFlagBits::VK_SAMPLE_COUNT_1_BIT

/* Number of viewports and number of scissors have to be the same */
/* at pipeline creation and in any call to set them dynamically   */
/* They also have to be the same as each other                    */
#define MIN_NUM_VIEWPORTS 1
#define MIN_NUM_SCISSORS MIN_NUM_VIEWPORTS

#define RASTER_MIN_LINE_WIDTH 1.0f


#define SUBPASS_ID 0

#define COLOR_ATT 0
#define DEPTH_ATT 1
#define ATT_COUNT 2

#define CLEARVALUES_COUNT 2

#define DEPTH_BIAS_CONSTANT 1.25f
#define DEPTH_CLAMP 0.0f
#define DEPTH_BIAS_SLOPE 1.75f

#endif // GRAPHICS_RENDERING_BACKENDS_VULKAN_COMMON_VULKAN_COMMON_HPP