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
 * @brief Converts a normalized float value <0.0, 1.0> to an unsigned 8-bit
 *        integer <0, 255>.
 *
 * @param value Float value to convert (expected to be in range <0.0, 1.0>).
 *
 * @return `uint8_t` Converted 8-bit unsigned integer value.
 */
uint8_t castNormalizedFloatToUnsignedInt8(float value);

/**
 * @brief Converts an 8-bit unsigned integer to a normalized float value.
 *
 * @details Transforms an 8-bit unsigned integer (0-255) to a float value
 *          in the normalized range <0.0, 1.0>. This is the inverse operation
 *          of castNormalizedFloatToUnsignedInt8.
 *
 * @param value The 8-bit unsigned integer value to convert.
 * @return `float` Normalized float value in range <0.0, 1.0>.
 */
float castUnsignedInt8ToNormalizedFloat(uint8_t value);

/**
 * @brief Extracts a specific color channel from a color vector.
 *
 * @param color RGBA color vector.
 * @param channel The color channel to extract (R, G, B, or A).
 *
 * @return `float` The value of the specified color channel.
 */
float getColorChannel(const glm::vec4 &color, Image::Channel channel);


/******************************************************************************/
/*                                                                            */
/*                           VERTEX SHADER HELPERS                            */
/*                                                                            */
/******************************************************************************/

/**
 * @brief Gets the vertex index for use in vertex assembly.
 *
 * @details Retrieves the vertex index either from an index buffer (for indexed
 *          drawing) or directly uses the vertex number (for non-indexed drawing).
 *          For indexed drawing, handles different index formats (U8, U16, U32).
 *
 * @param memory Reference to GPU memory containing buffers and vertex arrays.
 * @param vertexNumber Sequence number of the vertex in the drawing command.
 *
 * @return `uint32_t` The vertex index value (`gl_VertexID`) for the given vertex.
 */
uint32_t getVertexIndex(const GPUMemory &memory, uint32_t vertexNumber);

/**
 * @brief Assembles vertex data into an input vertex structure
 *
 * @details Collects all vertex attributes (position, normal, texcoords, etc.)
 *          from vertex buffers in GPU memory according to the current vertex
 *          array configuration, and fills the provided InVertex structure.
 *
 * @param memory Reference to GPU memory containing vertex data and attribute
 *               configurations.
 * @param inVertex Reference to the input vertex structure to be filled with
 *                 assembled data.
 */
void assembleVertex(const GPUMemory &memory, InVertex &inVertex);


/******************************************************************************/
/*                                                                            */
/*                     RASTERIZATION inspired by IZG labs                     */
/*                                                                            */
/******************************************************************************/

/**
 * @brief Rasterizes a triangle using the Pineda algorithm.
 *
 * @details Converts a triangle into fragments (potential pixels) using edge functions.
 *          For each pixel within the triangle's bounding box, determines if it's inside
 *          the triangle, interpolates vertex attributes, and invokes the fragment shader.
 *
 * @param memory GPU memory containing all resources.
 * @param program Active shader program with vertex and fragment shaders.
 * @param frameBuffer Target framebuffer for rendering output.
 * @param shaderInterface Interface for passing uniform data to shaders.
 * @param outTriangle Array of three output vertices from the vertex shader.
 * @param vertices Array of three screen-space vertex positions after viewport transform.
 * @param oneOverW Array of 1/w values for perspective-correct interpolation.
 */
void rasterizeTriangleUsingPineda(const GPUMemory &memory,
                                  const Program &program,
                                  const Framebuffer &frameBuffer,
                                  const ShaderInterface &shaderInterface,
                                  const OutVertex outTriangle[3],
                                  const glm::vec3 vertices[3],
                                  const float oneOverW[3]);


/******************************************************************************/
/*                                                                            */
/*                           RASTERIZATION HELPERS                            */
/*                                                                            */
/******************************************************************************/

/**
 * @brief Transforms clip space position to screen space coordinates.
 *
 * @details Performs perspective division (dividing by w) and viewport transformation
 *          to convert clip space coordinates to screen space. This is part of the
 *          graphics pipeline that maps from normalized device coordinates to window
 *          coordinates.
 *
 * @param clipSpacePosition Position in clip space after vertex shader processing.
 * @param width Width of the viewport in pixels.
 * @param height Height of the viewport in pixels.
 * @param oneOverW Output parameter storing 1/w for perspective correct interpolation.
 *
 * @return `glm::vec3` Screen space position (x, y, z) where x and y are pixel
 *         coordinates and z is the depth value.
 */
glm::vec3 clipSpacePositionToScreenSpace(const glm::vec4 &clipSpacePosition, uint32_t width,
                                         uint32_t height, float &oneOverW);

/**
 * @brief Determines if a triangle should be culled based on its orientation.
 *
 * @details Implements backface culling to eliminate triangles that face away from
 *          the camera. This optimization avoids processing pixels that would be
 *          occluded by front-facing geometry.
 *
 * @param triangleVertex Array of three screen-space vertices forming the triangle.
 * @param backfaceCulling Configuration specifying culling mode (off, cull front,
 *                        cull back).
 *
 * @return `true` if the triangle should be culled (skipped), `false` otherwise.
 */
bool backFaceCulling(const glm::vec3 triangleVertex[3], const BackfaceCulling &backfaceCulling);

/**
 * @brief Interpolates vertex attributes for a fragment using barycentric coordinates.
 *
 * @details Computes attribute values for a fragment by interpolating between the
 *          three vertices of the triangle. Handles different attribute types
 *          according to the program configuration. Integer attributes use flat
 *          shading (values from the provoking vertex), while floating-point
 *          attributes are interpolated.
 *
 * @param inFragment Reference to the fragment where interpolated attributes will be stored.
 * @param program The shader program containing vs2fs configuration.
 * @param outVertices Array of three output vertices containing attribute values.
 * @param lambda0 First barycentric coordinate.
 * @param lambda1 Second barycentric coordinate.
 * @param lambda2 Third barycentric coordinate.
 */
void interpolateFragmentAttributes(InFragment &inFragment, const Program &program,
                                   const OutVertex outVertices[3], float lambda0,
                                   float lambda1, float lambda2);


/******************************************************************************/
/*                                                                            */
/*                          PER FRAGMENT OPERATIONS                           */
/*                                                                            */
/******************************************************************************/

/**
 * @brief Performs early fragment tests (stencil and depth) before fragment
 *        shader execution.
 *
 * @details Implements the fixed-function pipeline stage that tests fragments
 *          against depth and stencil buffers. These tests can discard fragments
 *          early to avoid unnecessary fragment shader executions. Takes into
 *          account whether the fragment comes from a front-facing or back-facing
 *          primitive to apply the appropriate stencil operations.
 *
 * @param memory Reference to GPU memory containing stencil test settings.
 * @param frameBuffer Target framebuffer with depth and stencil buffers.
 * @param inFragment Fragment to be tested with position and depth information.
 * @param isFacingFront Boolean indicating if the fragment is from a front-facing primitive.
 *
 * @return `true` if the fragment passes all tests and should continue processing,
 *         `false` if the fragment should be discarded.
 */
bool executeEarlyPerFragmentOperations(const GPUMemory &memory, const Framebuffer &frameBuffer,
                                       const InFragment &inFragment, bool isFacingFront);

/**
 * @brief Performs late fragment operations after fragment shader execution.
 *
 * @details Implements the fixed-function pipeline stage that processes fragments
 *          after fragment shader execution. This stage occurs after the fragment
 *          shader has run and early fragment operations have been completed.
 *          The operations performed in this stage include:<br>
 *          1. Fragment discard check (fragments marked for discard are rejected).<br>
 *          2. Stencil buffer updates for fragments that passed all tests.<br>
 *          3. Depth buffer updates with fragment depth values.<br>
 *          4. Color buffer updates with alpha blending.
 *
 * @note The alpha blending formula used is:
 *     $$outputColor = existingColor * (1 - alpha) + fragmentColor * alpha$$
 *
 * @param memory Reference to GPU memory containing graphics pipeline state.
 * @param frameBuffer Target framebuffer where color/depth/stencil will be written.
 * @param inFragment Input fragment data including position and interpolated attributes.
 * @param outFragment Output from fragment shader containing color and discard flag.
 * @param isFacingFront Boolean indicating if the fragment is from a front-facing primitive.
 */
void executeLatePerFragmentOperations(const GPUMemory &memory, const Framebuffer &frameBuffer,
                                      const InFragment &inFragment, const OutFragment &outFragment,
                                      bool isFacingFront);

/**
 * @brief Applies a stencil operation to modify a stencil buffer value.
 *
 * @details Implements the different stencil operations that can be applied
 *          when a stencil or depth test yields specific results. Operations
 *          include keeping the value, setting to zero, replacing, incrementing,
 *          decrementing, or inverting the stencil value.
 *
 * @param stencilValue Reference to the stencil buffer value to be modified.
 * @param stencilOperation The operation to perform (KEEP, ZERO, REPLACE, etc.).
 * @param stencilValueReference Reference value used for the REPLACE operation (default: 0).
 */
void executeStencilOperation(uint8_t &stencilValue, StencilOp stencilOperation,
                             uint32_t stencilValueReference = 0);


/********************************************************************************/
/*                                                                              */
/*                  CLIPPING inspired by IZG presentation and:                  */
/* https://www.geeksforgeeks.org/polygon-clipping-sutherland-hodgman-algorithm/ */
/*                                                                              */
/********************************************************************************/

/**
 * @brief Clips a triangle against the near plane using the Sutherland-Hodgman
 *        algorithm.
 *
 * @details This function implements near plane clipping (`z = -w`) in homogeneous
 *          clip space. It processes each edge of the input triangle to determine
 *          if it intersects the near plane, and generates properly clipped geometry.
 *          The result can be 0, 1, or 2 triangles depending on how the original
 *          triangle intersects the near plane.
 *
 * @param program The shader program containing attribute type information for interpolation.
 * @param inputTriangle Array of 3 vertices representing the input triangle in clip space.
 * @param outputTriangles Array to store up to 2 resulting triangles after clipping.
 *
 * @return `uint32_t` Number of triangles produced (0 = fully clipped,
 *          1 or 2 = partially clipped).
 */
uint32_t clippingSutherlandHodgman(const Program &program, const OutVertex inputTriangle[3],
                                   OutVertex outputTriangles[2][3]);

/**
 * @brief Tests if a vertex is inside the near clipping plane.
 *
 * @details In homogeneous clip space, the near clipping plane is defined
 *          by `z = -w`. A vertex is inside the view volume when `z >= -w`,
 *          which can be rewritten as `z + w >= 0`.
 *
 * @param vertex The vertex to test against the near clipping plane.
 *
 * @return `bool` `True` if the vertex is inside the view volume
 *         (`z + w >= 0`), `false` otherwise.
 */
bool isVertexInsideClipPlane(const OutVertex &vertex);

/**
 * @brief Calculates the intersection point between a line segment and the
 *        near clipping plane.
 *
 * @details Computes the exact point where the line segment between `firstVertex`
 *          and `secondVertex` intersects the near plane (`z = -w`). The function
 *          linearly interpolates all vertex attributes at the intersection point.
 *
 * @param program The shader program containing attribute interpolation information.
 * @param startVertex First vertex of the line segment.
 * @param endVertex Second vertex of the line segment.
 *
 * @return `OutVertex` A new vertex at the intersection point with interpolated attributes.
 */
OutVertex calculateClipPlaneIntersection(const Program &program, const OutVertex &startVertex,
                                         const OutVertex &endVertex);

/**
 * @brief Creates a new vertex by linearly interpolating between two vertices.
 *
 * @details Performs linear interpolation of position and all vertex attributes
 *          based on the provided interpolation factor. Different attribute
 *          types (`float`, `vec2`, `vec3`, `vec4`) are handled according to
 *          their type. Integer attributes use flat shading (no interpolation).
 *
 * @param program Program containing attribute type information.
 * @param startVertex First source vertex.
 * @param endVertex Second source vertex.
 * @param interpolationFactor Value between `0.0` and `1.0` controlling the interpolation.
 *
 * @return `OutVertex` A new vertex with interpolated position and attributes.
 */
OutVertex interpolateVertex(const Program &program, const OutVertex &startVertex,
                            const OutVertex &endVertex, float interpolationFactor);

/*** end of file gpu.hpp ***/
