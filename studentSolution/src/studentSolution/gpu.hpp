/*!
 * @file gpu.hpp
 * @brief This file contains class that represents graphic card.
 *
 * @author Tomáš Milet, imilet@fit.vutbr.cz
 * @author Jan kalina, xkalinj00@stud.fit.vutbr.cz
 */
#pragma once

#include <solutionInterface/gpu.hpp>


/******************************************************************************/
/*                                                                            */
/*                               MAIN GPU LOGIC                               */
/*                                                                            */
/******************************************************************************/

/**
 * @brief Main entry point for GPU command execution.
 *
 * @details Initializes the GPU's draw ID counter to zero and processes
 *          all commands from the provided command buffer. This function
 *          serves as the primary interface for executing GPU operations.
 *
 * @param mem Reference to GPU memory containing all resources.
 * @param cb Command buffer containing the commands to be executed.
 */
void student_GPU_run(GPUMemory &mem, const CommandBuffer &cb);

/**
 * @brief Processes all commands in a command buffer sequentially.
 *
 * @details Iterates through each command in the provided command buffer,
 *          extracts its type and data, and dispatches it to the appropriate
 *          handler function. This function is used both for the main command
 *          buffer and for nested sub-command buffers.
 *
 * @param memory Reference to GPU memory where operations will be performed.
 * @param commandBuffer The buffer containing commands to be processed.
 */
void executeCommandBuffer(GPUMemory &memory, const CommandBuffer &commandBuffer);

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

/**
 * @brief Handles custom user-defined commands.
 *
 * @details Processes user-defined commands that extend the basic functionality
 *          of the GPU command system. This allows for custom operations to be
 *          implemented by applications.
 *
 * @param userCommand The user-defined command to be processed.
 */
void handleUserCommand(const UserCommand &userCommand);

/**
 * @brief Handles draw commands for rendering geometry.
 *
 * @details Processes draw commands which trigger the rendering pipeline to draw
 *          primitives using the currently bound program, vertex array, and
 *          framebuffer. This function implements the vertex and fragment
 *          processing stages.
 *
 * @param memory Reference to GPU memory containing resources needed for rendering.
 * @param drawCommand Command data containing draw parameters such as primitive type,
 *                    vertex count, and draw mode.
 */
void handleDrawCommand(GPUMemory &memory, const DrawCommand &drawCommand);

/**
 * @brief Handles execution of nested command buffers.
 *
 * @details Processes a sub-command buffer by recursively executing all commands
 *          contained within it. This allows for hierarchical organization of
 *          rendering commands and reuse of command sequences.
 *
 * @param memory Reference to GPU memory where operations will be performed.
 * @param subCommandBuffer Pointer to the sub-command buffer to be executed.
 */
void handleSubCommand(GPUMemory &memory, const CommandBuffer *subCommandBuffer);


/******************************************************************************/
/*                                                                            */
/*                              HELPER FUNCTIONS                              */
/*                                                                            */
/******************************************************************************/

/**
 * @brief Gets a pointer to a pixel in an image, handling y-coordinate
 *        reversal if needed.
 *
 * @param image Reference to the image.
 * @param x X-coordinate of the pixel.
 * @param y Y-coordinate of the pixel.
 * @param height Height of the image.
 * @param yReversed Flag indicating if y-coordinates are stored in reverse order.
 *
 * @return `uint8_t*` Pointer to the specified pixel data.
 */
uint8_t *getPixelMaybeReversed(const Image &image, uint32_t x, uint32_t y,
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
float getColorChannel(const glm::vec4 &color, Image::Channel channel);

/*** end of file gpu.hpp ***/
