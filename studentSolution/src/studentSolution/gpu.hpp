/*!
 * @file gpu.hpp
 * @brief This file contains class that represents graphic card.
 *
 * @author Tomáš Milet, imilet@fit.vutbr.cz
 * @author Jan kalina, xkalinj00@stud.fit.vutbr.cz
 */
#pragma once

#include <solutionInterface/gpu.hpp>

/**
 * @brief Function that executes work stored in command buffer on the gpu memory.
 * @details This function represents the functionality of GPU. It can render
 *          stuff, it can clear framebuffer. It can process work stored in
 *          command buffer
 *
 * @param mem GPU memory.
 * @param cb Command buffer - packaged of work sent to the GPU.
 */
void student_GPU_run(GPUMemory &mem, const CommandBuffer &cb);

/**
 * @brief Processes a single GPU command.
 *
 * @details This function handles execution of a specific GPU command based
 *          on its type. It interprets the command data and performs the
 *          corresponding operation on GPU memory.
 *
 * @param memory Reference to the GPU memory where operations will be performed.
 * @param type The type of command to be executed.
 * @param data The data associated with the command to be processed.
 */
void handleCommand(GPUMemory &memory, CommandType type, const CommandData &data);


/******************************************************************************/
/*                                                                            */
/*                              COMMAND HANDLERS                              */
/*                                                                            */
/******************************************************************************/

/**
 * @brief Handles binding a framebuffer to GPU memory.
 *
 * @param memory Reference to GPU memory where the framebuffer will be bound.
 * @param data Command data containing the framebuffer ID to bind.
 */
void handleBindFramebufferCommand(GPUMemory &memory, const CommandData &data);

/**
 * @brief Handles binding a shader program to GPU memory.
 *
 * @param memory Reference to GPU memory where the program will be bound.
 * @param data Command data containing the program ID to bind.
 */
void handleBindProgramCommand(GPUMemory &memory, const CommandData &data);

/**
 * @brief Handles binding a vertex array to GPU memory.
 *
 * @param memory Reference to GPU memory where the vertex array will be bound.
 * @param data Command data containing the vertex array ID to bind.
 */
void handleBindVertexArrayCommand(GPUMemory &memory, const CommandData &data);

/**
 * @brief Handles enabling/disabling write operations to framebuffer.
 *
 * @param memory Reference to GPU memory where write blocking will be configured.
 * @param data Command data containing block writes flag.
 */
void handleBlockWritesCommand(GPUMemory &memory, const CommandData &data);

/**
 * @brief Handles enabling/disabling backface culling.
 *
 * @param memory Reference to GPU memory where backface culling will be configured.
 * @param data Command data containing backface culling parameters.
 */
void handleSetBackfaceCullingCommand(GPUMemory &memory, const CommandData &data);

/**
 * @brief Handles setting front face winding direction.
 *
 * @param memory Reference to GPU memory where front face direction will be set.
 * @param data Command data containing front face winding direction.
 */
void handleSetFrontFaceCommand(GPUMemory &memory, const CommandData &data);

/**
 * @brief Handles configuring stencil test parameters.
 *
 * @param memory Reference to GPU memory where stencil settings will be updated.
 * @param data Command data containing stencil test parameters.
 */
void handleSetStencilCommand(GPUMemory &memory, const CommandData &data);

/**
 * @brief Handles setting the draw ID for instanced rendering.
 *
 * @param memory Reference to GPU memory where draw ID will be set.
 * @param data Command data containing draw ID value.
 */
void handleSetDrawIdCommand(GPUMemory &memory, const CommandData &data);

/**
 * @brief Handles clearing the color buffer with specified color.
 *
 * @param memory Reference to GPU memory containing the framebuffer.
 * @param clearCommand Command data containing the clear color value.
 */
void handleClearColorCommand(const GPUMemory &memory, const ClearColorCommand &clearCommand);

/**
 * @brief Handles clearing the depth buffer with specified value.
 *
 * @param memory Reference to GPU memory containing the framebuffer.
 * @param clearCommand Command data containing the clear depth value.
 */
void handleClearDepthCommand(const GPUMemory &memory, const ClearDepthCommand &clearCommand);

/**
 * @brief Handles clearing the stencil buffer with specified value.
 *
 * @param memory Reference to GPU memory containing the framebuffer.
 * @param clearCommand Command data containing the clear stencil value.
 */
void handleClearStencilCommand(const GPUMemory &memory, const ClearStencilCommand &clearCommand);


/******************************************************************************/
/*                                                                            */
/*                              HELPER FUNCTIONS                              */
/*                                                                            */
/******************************************************************************/

/**
 * @brief Gets a pointer to a pixel in an image, handling y-coordinate
 *        reversal if needed.
 *
 * @param img Reference to the image.
 * @param x X-coordinate of the pixel.
 * @param y Y-coordinate of the pixel.
 * @param height Height of the image.
 * @param yReversed Flag indicating if y-coordinates are stored in reverse order.
 *
 * @return `uint8_t*` Pointer to the specified pixel data.
 */
uint8_t *getPixelMaybeReversed(const Image &img, uint32_t x, uint32_t y,
                               uint32_t height, bool yReversed);

/**
 * @brief Converts a normalized float value [0.0, 1.0] to an unsigned 8-bit
 *        integer [0, 255].
 *
 * @param value Float value to convert (expected to be in range [0.0, 1.0]).
 *
 * @return `uint8_t` Converted 8-bit unsigned integer value.
 */
uint8_t castNormalizedFloatToUnsignedInt8(float value);

/**
 * @brief Extracts a specific color channel from a color vector.
 *
 * @param color RGBA color vector.
 * @param channel The color channel to extract (R, G, B, or A).
 *
 * @return `float` The value of the specified color channel.
 */
float pickChannel(const glm::vec4 &color, Image::Channel channel);

/*** end of file gpu.hpp ***/
