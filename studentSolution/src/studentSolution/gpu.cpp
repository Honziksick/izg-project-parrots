/*!
 * @file gpu.cpp
 * @brief This file contains implementation of GPU functionality.
 *
 * @author Tomáš Milet, imilet@fit.vutbr.cz
 * @author Jan kalina, xkalinj00@stud.fit.vutbr.cz
 */

#include <studentSolution/gpu.hpp>
#include <algorithm>


/******************************************************************************/
/*                                                                            */
/*                               MAIN GPU LOGIC                               */
/*                                                                            */
/******************************************************************************/

//! [student_GPU_run]
void student_GPU_run(GPUMemory &mem, const CommandBuffer &cb) {
    // === TEST 12 ===
    mem.gl_DrawID = 0; // initialize the draw ID to zero before processing commands from main-cb and sub-cbs

    // Main loop is separated into its own function, so the draw ID i correctly
    // incremented when recursively calling main loop for sub-commands
    executeCommandBuffer(mem, cb);
} // student_GPU_run()
//! [student_GPU_run]

static inline void executeCommandBuffer(GPUMemory &memory, const CommandBuffer &commandBuffer) {
    for(uint32_t iCommand = 0; iCommand < commandBuffer.nofCommands; iCommand++) {
        const CommandType type = commandBuffer.commands[iCommand].type;
        CommandData data = commandBuffer.commands[iCommand].data;
        handleCommand(memory, type, data);
    }
} // executeCommandBuffer()

static inline void handleCommand(GPUMemory &memory, const CommandType type, const CommandData &data) {
    switch(type) {
        /**********************************************************************/
        /*                          BINDING commands                          */
        /**********************************************************************/
        case CommandType::BIND_FRAMEBUFFER:
            handleBindFramebufferCommand(memory, data);
            break;

        case CommandType::BIND_PROGRAM:
            handleBindProgramCommand(memory, data);
            break;

        case CommandType::BIND_VERTEXARRAY:
            handleBindVertexArrayCommand(memory, data);
            break;

        /**********************************************************************/
        /*                          SETTING commands                          */
        /**********************************************************************/
        case CommandType::BLOCK_WRITES_COMMAND:
            handleBlockWritesCommand(memory, data);
            break;

        case CommandType::SET_BACKFACE_CULLING_COMMAND:
            handleSetBackfaceCullingCommand(memory, data);
            break;

        case CommandType::SET_FRONT_FACE_COMMAND:
            handleSetFrontFaceCommand(memory, data);
            break;

        case CommandType::SET_STENCIL_COMMAND:
            handleSetStencilCommand(memory, data);
            break;

        case CommandType::SET_DRAW_ID:
            handleSetDrawIdCommand(memory, data);
            break;

        /**********************************************************************/
        /*                           CLEAR commands                           */
        /**********************************************************************/
        case CommandType::CLEAR_COLOR:
            handleClearColorCommand(memory, data.clearColorCommand);
            break;

        case CommandType::CLEAR_DEPTH:
            handleClearDepthCommand(memory, data.clearDepthCommand);
            break;

        case CommandType::CLEAR_STENCIL:
            handleClearStencilCommand(memory, data.clearStencilCommand);
            break;

        /**********************************************************************/
        /*                           OTHER commands                           */
        /**********************************************************************/
        case CommandType::USER_COMMAND:
            handleUserCommand(data.userCommand);
            break;

        case CommandType::DRAW:
            handleDrawCommand(memory, data.drawCommand);
            break;

        case CommandType::SUB_COMMAND:
            handleSubCommand(memory, data.subCommand.commandBuffer);
            break;

        case CommandType::EMPTY:
        default:
            break;
    } // switch(type)
} // handleCommand()


/******************************************************************************/
/*                                                                            */
/*                              COMMAND HANDLERS                              */
/*                                                                            */
/******************************************************************************/

// === TEST 0 ===
static inline void handleBindFramebufferCommand(GPUMemory &memory, const CommandData &data) {
    memory.activatedFramebuffer = data.bindFramebufferCommand.id;
} // handleBindFramebufferCommand()

// === TEST 1 ===
static inline void handleBindProgramCommand(GPUMemory &memory, const CommandData &data) {
    memory.activatedProgram = data.bindProgramCommand.id;
} // handleBindProgramCommand()

// === TEST 2 ===
static inline void handleBindVertexArrayCommand(GPUMemory &memory, const CommandData &data) {
    memory.activatedVertexArray = data.bindVertexArrayCommand.id;
} // handleBindVertexArrayCommand()

// === TEST 3 ===
static inline void handleBlockWritesCommand(GPUMemory &memory, const CommandData &data) {
    memory.blockWrites = data.blockWritesCommand.blockWrites;
} // handleBlockWritesCommand()

// === TEST 4 ===
static inline void handleSetBackfaceCullingCommand(GPUMemory &memory, const CommandData &data) {
    memory.backfaceCulling.enabled = data.setBackfaceCullingCommand.enabled;
} // handleSetBackfaceCullingCommand()

// === TEST 5 ===
static inline void handleSetFrontFaceCommand(GPUMemory &memory, const CommandData &data) {
    memory.backfaceCulling.frontFaceIsCounterClockWise = data.setFrontFaceCommand.frontFaceIsCounterClockWise;
} // handleSetFrontFaceCommand()

// === TEST 6 ===
static inline void handleSetStencilCommand(GPUMemory &memory, const CommandData &data) {
    memory.stencilSettings = data.setStencilCommand.settings;
} // handleSetStencilCommand()

// === TEST 7 ===
static inline void handleSetDrawIdCommand(GPUMemory &memory, const CommandData &data) {
    memory.gl_DrawID = data.setDrawIdCommand.id;
} // handleSetDrawIdCommand()

// === TEST 8-10 ===
static inline void handleClearColorCommand(const GPUMemory &memory, const ClearColorCommand &clearCommand) {
    const Framebuffer &framebuffer = memory.framebuffers[memory.activatedFramebuffer];
    const Image &color = framebuffer.color;

    // Check if the color buffer is valid
    if(!color.data) {
        return;
    }

    // Clear the color buffer for each pixel
    for(uint32_t y = 0; y < framebuffer.height; y++) {
        for(uint32_t x = 0; x < framebuffer.width; x++) {
            uint8_t *pixel = getPixelMaybeReversed(color, x, y, framebuffer.height, framebuffer.yReversed);

            // Clear the pixel
            for(uint32_t iChannel = 0; iChannel < color.channels; iChannel++) {
                const float channelComponent = getColorChannel(clearCommand.value, color.channelTypes[iChannel]);

                // Clear each channel (R, G, B, A) based on the image format
                if(color.format == Image::U8) {
                    pixel[iChannel] = castNormalizedFloatToUnsignedInt8(channelComponent);
                }
                else {
                    reinterpret_cast<float*>(pixel)[iChannel] = channelComponent;
                }
            } // for(c)
        } // for(x)
    } // for(y)
} // handleClearColorCommand()

// === TEST 8-10 ===
static inline void handleClearDepthCommand(const GPUMemory &memory, const ClearDepthCommand &clearCommand) {
    const Framebuffer &framebuffer = memory.framebuffers[memory.activatedFramebuffer];
    const Image &depth = framebuffer.depth;

    // Check if the depth buffer is valid
    if(!depth.data) {
        return;
    }

    // Clear the depth buffer for each pixel
    for(uint32_t y = 0; y < framebuffer.height; y++) {
        for(uint32_t x = 0; x < framebuffer.width; x++) {
            uint8_t *pixel = getPixelMaybeReversed(depth, x, y, framebuffer.height, framebuffer.yReversed);

            // Stencil is always 1 channel (hence the [0]) float value
            reinterpret_cast<float*>(pixel)[0] = clearCommand.value;
        }
    }
} // handleClearDepthCommand()

// === TEST 8-10 ===
static inline void handleClearStencilCommand(const GPUMemory &memory, const ClearStencilCommand &clearCommand) {
    const Framebuffer &framebuffer = memory.framebuffers[memory.activatedFramebuffer];
    const Image &stencil = framebuffer.stencil;

    // Check if the stencil buffer is valid
    if(!stencil.data) {
        return;
    }

    // Clear the stencil buffer for each pixel
    for(uint32_t y = 0; y < framebuffer.height; y++) {
        for(uint32_t x = 0; x < framebuffer.width; x++) {
            uint8_t *pixel = getPixelMaybeReversed(stencil, x, y, framebuffer.height, framebuffer.yReversed);

            // Stencil is always 1 channel (hence the [0]) uint8_t value
            pixel[0] = clearCommand.value;
        }
    }
} // handleClearStencilCommand()

// === TEST 11 ===
static inline void handleUserCommand(const UserCommand &userCommand) {
    if(userCommand.callback) {
        userCommand.callback(userCommand.data);
    }
} // handleUserCommand()

// === TEST 12, 14-15, 17-29 ===
static inline void handleDrawCommand(GPUMemory &memory, const DrawCommand &drawCommand) {
    // Get the drawing program from memory (as described in === TEST 14 ===)
    const Program &program = memory.programs[memory.activatedProgram];

    // === TEST 12 ==
    // We skip the drawing and only increment the draw ID if the program is not set
    if(!program.vertexShader) {
        memory.gl_DrawID++;
        return;
    }

    // Prepare constant input of shader interface (07 Vertex Processor image)
    ShaderInterface shaderInterface;
    shaderInterface.gl_DrawID = memory.gl_DrawID;  // === TEST 15 ===
    shaderInterface.uniforms = memory.uniforms;    // === TEST 17 ===
    shaderInterface.textures = memory.textures;    // === TEST 17 ===

    // Get the currently activated framebuffer from memory
    const auto &frameBuffer = memory.framebuffers[memory.activatedFramebuffer];
    const uint32_t width = frameBuffer.width;
    const uint32_t height = frameBuffer.height;

    for(uint32_t iTriangleStart = 0; iTriangleStart < drawCommand.nofVertices; iTriangleStart += 3) {
        OutVertex outTriangle[3];
        glm::vec3 screenVertex[3];
        float oneOverW[3];

        // === TEST 14 ===
        // Process each I/O vertex in the draw command
        for(int iVertex = 0; iVertex < 3; iVertex++) {
            InVertex inVertex;

            // === TEST 18 ===
            // getVertexIndex() function supports both indexed and non-indexed drawing
            inVertex.gl_VertexID = getVertexIndex(memory, iTriangleStart + iVertex);

            // === TEST 19-21 ===
            // Assemble vertex from buffers using Vertex Assembly unit
            assembleVertex(memory, inVertex);

            // === TEST 14 ===
            // Run vertex shader for each vertex
            program.vertexShader(outTriangle[iVertex], inVertex, shaderInterface);

            screenVertex[iVertex] = clipSpacePositionToScreenSpace(outTriangle[iVertex].gl_Position,
                                                                   width, height, oneOverW[iVertex]);
        } // for(iVertex)

        // === TEST 26 ===
        if(backFaceCulling(screenVertex, memory.backfaceCulling)) {
            continue;
        }

        // All these steps are handled by the rasterizeTriangleUsingPineda function
        rasterizeTriangleUsingPineda(memory,            // GPU state
                                     program,           // active program
                                     frameBuffer,       // active framebuffer
                                     shaderInterface,   // constants for fragment shader
                                     outTriangle,       // vertex shader outputs
                                     screenVertex,      // vertices in screen-space
                                     oneOverW);         // 1/w for perspective correction
    } // for(iTriangle)

    // === TEST 12 ===
    memory.gl_DrawID++;  // increment the draw ID for each draw command
} // handleDrawCommand()

// === TEST 13 ===
static inline void handleSubCommand(GPUMemory &memory, const CommandBuffer *subCommandBuffer) {
    // If subCommand isn't NULL, we recursively execute the (sub)command buffer
    if(subCommandBuffer) {
        executeCommandBuffer(memory, *subCommandBuffer);
    }
} // handleSubCommand()


/******************************************************************************/
/*                                                                            */
/*                              HELPER FUNCTIONS                              */
/*                                                                            */
/******************************************************************************/

// === TEST 8-10 ===
static inline uint8_t *getPixelMaybeReversed(const Image &image, const uint32_t x, const uint32_t y,
                                             const uint32_t height, const bool yReversed) {
    // Choose the y-coordinate based on whether the image is y-reversed
    uint32_t maybeReversedY;
    if(yReversed) {
        maybeReversedY = height - y - 1;  // flip the y-coordinate and '-1' as we index from 0
    }
    else {
        maybeReversedY = y;  // use the y-coordinate as is
    }

    return static_cast<uint8_t*>(image.data) + (maybeReversedY * image.pitch) + (x * image.bytesPerPixel);
} // getPixelMaybeReversed()

// === TEST 8-10 ===
static inline uint8_t castNormalizedFloatToUnsignedInt8(const float value) {
    return static_cast<uint8_t>(glm::clamp(value, 0.f, 1.f) * 255.f + 0.5f);
} // castNormalizedFloatToUnsignedInt8()

// === TEST 8-10 ===
static inline float getColorChannel(const glm::vec4 &color, const Image::Channel channel) {
    switch(channel) {
        case Image::RED:
            return color.r;
        case Image::GREEN:
            return color.g;
        case Image::BLUE:
            return color.b;
        case Image::ALPHA:
            return color.a;
        default:
            return 0.f;
    } // switch(channel)
} // pickChannel()


/******************************************************************************/
/*                                                                            */
/*                           VERTEX SHADER HELPERS                            */
/*                                                                            */
/******************************************************************************/

// === TEST 18 ===
static inline uint32_t getVertexIndex(const GPUMemory &memory, const uint32_t vertexNumber) {
    const VertexArray &vertexArray = memory.vertexArrays[memory.activatedVertexArray];

    // For indexed drawing approache (indexBufferID >= 0)
    if(vertexArray.indexBufferID >= 0) {
        const auto &[indexBufferData, indexBufferSize] = memory.buffers[vertexArray.indexBufferID];

        // Get the base address of the index buffer with the applied offset
        const uint8_t *basePtr = static_cast<const uint8_t*>(indexBufferData) + vertexArray.indexOffset;

        // Read the index value based on index type (U8, U16, U32)
        switch(vertexArray.indexType) {
            case IndexType::U8:
                return basePtr[vertexNumber];

            case IndexType::U16:
                return reinterpret_cast<const uint16_t*>(basePtr)[vertexNumber];

            case IndexType::U32:
                return reinterpret_cast<const uint32_t*>(basePtr)[vertexNumber];

            default:
                return vertexNumber;
        }
    } // switch(vertexArray.indexType)

    // For non-indexed drawing approache (vertexNumber as is)
    return vertexNumber;
} // getVertexIndex()

// === TEST 19-21 ===
static inline void assembleVertex(const GPUMemory &memory, InVertex &inVertex) {
    const VertexArray &vertexArray = memory.vertexArrays[memory.activatedVertexArray];

    // For each vertex attribute, we read the data from the corresponding buffer
    for(uint32_t iAttribute = 0; iAttribute < maxAttribs; iAttribute++) {
        const auto &[attributeBufferID, attributeStride, attrbitueOffset, attributeType] = vertexArray.vertexAttrib[iAttribute];

        // We proccess only active attributes (with valid type and buffer ID)
        if(attributeType != AttribType::EMPTY && attributeBufferID >= 0) {
            const auto &[bufferData, bufferSize] = memory.buffers[attributeBufferID];

            // We read the attributes from addres: buf_ptr + offset + stride*gl_VertexID
            const uint8_t *pAttribute = static_cast<const uint8_t*>(bufferData) + attrbitueOffset + attributeStride * inVertex.gl_VertexID;

            // We copy the attribute data to the inVertex structure based on its type
            switch(attributeType) {
                case AttribType::FLOAT:
                    inVertex.attributes[iAttribute].v1 = *reinterpret_cast<const float*>(pAttribute);
                    break;

                case AttribType::VEC2:
                    inVertex.attributes[iAttribute].v2 = *reinterpret_cast<const glm::vec2*>(pAttribute);
                    break;

                case AttribType::VEC3:
                    inVertex.attributes[iAttribute].v3 = *reinterpret_cast<const glm::vec3*>(pAttribute);
                    break;

                case AttribType::VEC4:
                    inVertex.attributes[iAttribute].v4 = *reinterpret_cast<const glm::vec4*>(pAttribute);
                    break;

                case AttribType::UINT:
                    inVertex.attributes[iAttribute].u1 = *reinterpret_cast<const uint32_t*>(pAttribute);
                    break;

                case AttribType::UVEC2:
                    inVertex.attributes[iAttribute].u2 = *reinterpret_cast<const glm::uvec2*>(pAttribute);
                    break;

                case AttribType::UVEC3:
                    inVertex.attributes[iAttribute].u3 = *reinterpret_cast<const glm::uvec3*>(pAttribute);
                    break;

                case AttribType::UVEC4:
                    inVertex.attributes[iAttribute].u4 = *reinterpret_cast<const glm::uvec4*>(pAttribute);
                    break;

                case AttribType::EMPTY:
                default:
                    break;
            } // switch(attributeType)
        } // if(attributeType != AttribType::EMPTY && attributeBufferID >= 0)
    } // for(iAttribute)
} // assembleVertex()


/******************************************************************************/
/*                                                                            */
/*                               RASTERIZATION                                */
/*                                                                            */
/******************************************************************************/

// === TEST 22-24, 27-29 ===
static inline void rasterizeTriangleUsingPineda(const GPUMemory &memory, const Program &program,
                                                const Framebuffer &frameBuffer, const ShaderInterface &shaderInterface,
                                                const OutVertex outTriangle[3], const glm::vec3 vertices[3], const float oneOverW[3]) {
    // Edge AB = B - A
    const float edgeX01 = vertices[1].x - vertices[0].x;
    const float edgeY01 = vertices[1].y - vertices[0].y;

    // Edge BC = C - B
    const float edgeX12 = vertices[2].x - vertices[1].x;
    const float edgeY12 = vertices[2].y - vertices[1].y;

    // Edge CA = A - C
    const float edgeX20 = vertices[0].x - vertices[2].x;
    const float edgeY20 = vertices[0].y - vertices[2].y;

    // Edge AC = C - A (needed for area calculation)
    const float edgeX02 = vertices[2].x - vertices[0].x;
    const float edgeY02 = vertices[2].y - vertices[0].y;

    // Calculate the signed double-area of the triangle
    const float signedDoubleArea = edgeX01 * edgeY02 - edgeY01 * edgeX02;

    // Check if the area is valid (a non-zero value)
    if(signedDoubleArea == 0.f) {
        return;
    }

    // Calculate the absolute value of the signed double area for barycentric coordinates
    const float areaAbs = std::fabs(signedDoubleArea);

    // The area is positive for counter-clockwise triangles and negative for clockwise triangles
    const float orientation = signedDoubleArea > 0.f ? 1.f : -1.f;

    // Calculation of coefficients for the implicit line equation for each edge of the triangle (Ax + By + C = 0)
    const float edgeA01Normal = -edgeY01 * orientation;  // A = -(y1 - y0)
    const float edgeB01Normal = edgeX01 * orientation;   // B =  (x1 - x0)
    const float edgeC01Normal = -(edgeA01Normal * vertices[0].x + edgeB01Normal * vertices[0].y);

    const float edgeA12Normal = -edgeY12 * orientation;  // A = -(y2 - y1)
    const float edgeB12Normal = edgeX12 * orientation;   // B =  (x2 - x1)
    const float edgeC12Normal = -(edgeA12Normal * vertices[1].x + edgeB12Normal * vertices[1].y);

    const float edgeA20Normal = -edgeY20 * orientation;  // A = -(y0 - y2)
    const float edgeB20Normal = edgeX20 * orientation;   // B =  (x0 - x2)
    const float edgeC20Normal = -(edgeA20Normal * vertices[2].x + edgeB20Normal * vertices[2].y);

    // Set initial steps for edge functions
    // Step in X axis
    const float edgeStepX_12 = edgeA12Normal;
    const float edgeStepX_20 = edgeA20Normal;
    const float edgeStepX_01 = edgeA01Normal;

    // Step in Y axis
    const float edgeStepY_12 = edgeB12Normal;
    const float edgeStepY_20 = edgeB20Normal;
    const float edgeStepY_01 = edgeB01Normal;

    // Calculate the bounding box of the triangle
    // For x-cordinates: | the min/max values of x-coordinates of vertices | left bottom corner of the bounding box | largest x-coordinate |
    const int minX = glm::clamp(static_cast<int>(std::floor(std::min({vertices[0].x, vertices[1].x, vertices[2].x}))), 0, static_cast<int>(frameBuffer.width - 1));
    const int maxX = glm::clamp(static_cast<int>(std::ceil(std::max({vertices[0].x, vertices[1].x, vertices[2].x}))), 0, static_cast<int>(frameBuffer.width - 1));

    // For y-cordinates: | the min/max values of y-coordinates of vertices | left bottom corner of the bounding box | largest y-coordinate |
    const int minY = glm::clamp(static_cast<int>(std::floor(std::min({vertices[0].y, vertices[1].y, vertices[2].y}))), 0, static_cast<int>(frameBuffer.height - 1));
    const int maxY = glm::clamp(static_cast<int>(std::ceil(std::max({vertices[0].y, vertices[1].y, vertices[2].y}))), 0, static_cast<int>(frameBuffer.height - 1));

    // Calculate the coordinates of the center of the first pixel in the top-left corner of the bounding box
    float rowX = minX + 0.5f;
    float rowY = minY + 0.5f;

    // Initialization of edge functions for the first pixel in the top-left corner of the bounding box
    float edge12Row = edgeA12Normal * rowX + edgeB12Normal * rowY + edgeC12Normal;
    float edge20Row = edgeA20Normal * rowX + edgeB20Normal * rowY + edgeC20Normal;
    float edge01Row = edgeA01Normal * rowX + edgeB01Normal * rowY + edgeC01Normal;

    // Determine if we include the top-left pixel in the bounding box
    const bool edge01TopLeft = (edgeB01Normal > 0) || (edgeB01Normal == 0 && edgeA01Normal > 0);
    const bool edge12TopLeft = (edgeB12Normal > 0) || (edgeB12Normal == 0 && edgeA12Normal > 0);
    const bool edge20TopLeft = (edgeB20Normal > 0) || (edgeB20Normal == 0 && edgeA20Normal > 0);

    // === TEST 22-24 ==
    // Rasterization loop over the bounding box of the triangle using Pineda's edge functions
    for(int y = minY; y <= maxY; y++) {
        // Values of edge functions at the beginning of the row
        float edge12 = edge12Row;
        float edge20 = edge20Row;
        float edge01 = edge01Row;

        for(int x = minX; x <= maxX; x++) {
            // Test if the point lies inside the triangle
            if((edge12 > 0 || (edge12 == 0 && edge12TopLeft)) &&
                (edge20 > 0 || (edge20 == 0 && edge20TopLeft)) &&
                (edge01 > 0 || (edge01 == 0 && edge01TopLeft))) {
                // Barycentric coordinates
                const float lambda0 = edge12 / areaAbs;
                const float lambda1 = edge20 / areaAbs;
                const float lambda2 = edge01 / areaAbs;

                // === TEST 27 ===
                // Depth interpolation
                const float depth = vertices[0].z * lambda0 + vertices[1].z * lambda1 + vertices[2].z * lambda2;

                // Perspective correction for attribute interpolation
                const float lambda0_perspectiveCorrection = lambda0 * oneOverW[0];
                const float lambda1_perspectiveCorrection = lambda1 * oneOverW[1];
                const float lambda2_perspectiveCorrection = lambda2 * oneOverW[2];

                // Sum of perspective-corrected weights for normalization
                const float sumWeight = lambda0_perspectiveCorrection + lambda1_perspectiveCorrection + lambda2_perspectiveCorrection;
                const float inverseSumWeight = 1.0f / sumWeight;

                // Normalized perspective-corrected weights
                const float l0 = lambda0_perspectiveCorrection * inverseSumWeight;
                const float l1 = lambda1_perspectiveCorrection * inverseSumWeight;
                const float l2 = lambda2_perspectiveCorrection * inverseSumWeight;

                // Creation and initialization of input structure for fragment shader
                InFragment inFragment;
                inFragment.gl_FragCoord = glm::vec4(x + 0.5f, y + 0.5f, depth, inverseSumWeight);

                // === TEST 28-29 ===
                // Interpolation of attributes from vertex shader
                interpolateFragmentAttributes(inFragment, program, outTriangle, l0, l1, l2);

                // Determine the front face orientation
                const bool isFrontCounterClockWise = memory.backfaceCulling.frontFaceIsCounterClockWise;
                const bool isFront  = ((signedDoubleArea > 0) == isFrontCounterClockWise);

                // === TEST 30-33 ===
                // If EFO returned false, we skip the fragment shader execution
                if(!executeEarlyPerFragmentOperations(memory, frameBuffer, inFragment, isFront)){
                    edge12 += edgeStepX_12;
                    edge20 += edgeStepX_20;
                    edge01 += edgeStepX_01;
                    continue;
                }

                // === TEST 22 ===
                // Call the fragment shader
                OutFragment outFragment;
                program.fragmentShader(outFragment, inFragment, shaderInterface);

                // === TEST 22-24 ===
                // Write the color to the framebuffer
                writeColor(frameBuffer, x, y, outFragment.gl_FragColor, memory.blockWrites.color, frameBuffer.yReversed);
            } // if(edge12 >= 0 && edge20 >= 0 && edge01 >= 0)

            // Update the horizontal part of edge functions for the next pixel in the row
            edge12 += edgeStepX_12;
            edge20 += edgeStepX_20;
            edge01 += edgeStepX_01;
        } // for(x)

        // Update the vertical part of edge functions for the next row
        edge12Row += edgeStepY_12;
        edge20Row += edgeStepY_20;
        edge01Row += edgeStepY_01;
    } // for(y)
} // rasterizeTriangleUsingPineda()

/******************************************************************************/
/*                                                                            */
/*                           RASTERIZATION HELPERS                            */
/*                                                                            */
/******************************************************************************/

// === TEST 25 ===
static inline glm::vec3 clipSpacePositionToScreenSpace(const glm::vec4 &clipSpacePosition, const uint32_t width,
                                                       const uint32_t height, float &oneOverW) {
    // Calculate inverse of w-component (1/w) for perspective division
    oneOverW = 1.f / clipSpacePosition.w;

    // Perform perspective division to convert from clip space to normalized device coordinates (NDC)
    const glm::vec3 normalizedDeviceCoordinates = glm::vec3(clipSpacePosition) * oneOverW;

    // Transform NDC coordinates (range [-1,1]) to screen space coordinates (range [0,width/height])
    glm::vec3 screenSpacePosition;
    screenSpacePosition.x = (normalizedDeviceCoordinates.x * 0.5f + 0.5f) * static_cast<float>(width);
    screenSpacePosition.y = (normalizedDeviceCoordinates.y * 0.5f + 0.5f) * static_cast<float>(height);
    screenSpacePosition.z = normalizedDeviceCoordinates.z;

    return screenSpacePosition;
} // clipSpacePositionToScreenSpace()

// === TEST 26 ===
static inline bool backFaceCulling(const glm::vec3 triangleVertex[3], const BackfaceCulling &backfaceCulling) {
    // Skip culling if backface culling is disabled
    if(!backfaceCulling.enabled) {
        return false;
    }

    // Calculate the first edge vector (from vertex 0 to vertex 1)
    const float edge1_x = triangleVertex[1].x - triangleVertex[0].x;
    const float edge1_y = triangleVertex[1].y - triangleVertex[0].y;

    // Calculate the second edge vector (from vertex 0 to vertex 2)
    const float edge2_x = triangleVertex[2].x - triangleVertex[0].x;
    const float edge2_y = triangleVertex[2].y - triangleVertex[0].y;

    // Compute twice the signed area using 2D cross product: (edge1 × edge2)
    // Positive area indicates counter-clockwise winding, clockwise winding is negative.
    const float signedArea = (edge1_x * edge2_y) - (edge1_y * edge2_x);

    // Determine triangle orientation (true for counter-clockwise)
    const bool isCounterClockwise = (signedArea > 0.f);

    // Determine if the triangle should be culled
    return (isCounterClockwise != backfaceCulling.frontFaceIsCounterClockWise);
} // backFaceCulling()


// === TEST 28-29 ===
static inline void interpolateFragmentAttributes(InFragment &inFragment, const Program &program, const OutVertex outVertices[3],
                                                 const float lambda0, const float lambda1, const float lambda2) {
    // Iterate through all possible vertex attributes
    for(uint32_t iAttribute = 0; iAttribute < maxAttribs; iAttribute++) {
        // Process the attribute based on its type
        switch(program.vs2fs[iAttribute]) {
            case AttribType::EMPTY: {
                // Skip empty attributes
                continue;
            }
            case AttribType::FLOAT: {
                // Interpolate float value using barycentric coordinates
                float vertex0 = outVertices[0].attributes[iAttribute].v1;
                float vertex1 = outVertices[1].attributes[iAttribute].v1;
                float vertex2 = outVertices[2].attributes[iAttribute].v1;

                inFragment.attributes[iAttribute].v1 = vertex0 * lambda0 + vertex1 * lambda1 + vertex2 * lambda2;

                break;
            }
            case AttribType::VEC2: {
                // Interpolate vec2 components using barycentric coordinates
                glm::vec2 vertex0 = outVertices[0].attributes[iAttribute].v2;
                glm::vec2 vertex1 = outVertices[1].attributes[iAttribute].v2;
                glm::vec2 vertex2 = outVertices[2].attributes[iAttribute].v2;

                inFragment.attributes[iAttribute].v2 = vertex0 * lambda0 + vertex1 * lambda1 + vertex2 * lambda2;

                break;
            }
            case AttribType::VEC3: {
                // Interpolate vec3 components using barycentric coordinates
                glm::vec3 vertex0 = outVertices[0].attributes[iAttribute].v3;
                glm::vec3 vertex1 = outVertices[1].attributes[iAttribute].v3;
                glm::vec3 vertex2 = outVertices[2].attributes[iAttribute].v3;

                inFragment.attributes[iAttribute].v3 = vertex0 * lambda0 + vertex1 * lambda1 + vertex2 * lambda2;

                break;
            }
            case AttribType::VEC4: {
                // Interpolate vec4 components using barycentric coordinates
                glm::vec4 vertex0 = outVertices[0].attributes[iAttribute].v4;
                glm::vec4 vertex1 = outVertices[1].attributes[iAttribute].v4;
                glm::vec4 vertex2 = outVertices[2].attributes[iAttribute].v4;

                inFragment.attributes[iAttribute].v4 = vertex0 * lambda0 + vertex1 * lambda1 + vertex2 * lambda2;

                break;
            }
            case AttribType::UINT:
            case AttribType::UVEC2:
            case AttribType::UVEC3:
            case AttribType::UVEC4: {
                // For integer attributes, we use flat shading
                inFragment.attributes[iAttribute] = outVertices[0].attributes[iAttribute];
                continue;
            }
            default:
                break;
        } // switch(AttributeType)
    } // for(iAttribute)
} // interpolateFragmentAttributes()

// === TEST 22-24 ===
static inline void writeColor(const Framebuffer &frameBuffer, const uint32_t pixelX, const uint32_t pixelY,
                              const glm::vec4 &color, const bool blockWrites, const bool yReversed) {
    // Skip writing if color buffer is not available or color writing is blocked
    if(!frameBuffer.color.data || blockWrites) {
        return;
    }

    // Get pointer to the pixel at coordinates [x,y], handling y-reversed if needed
    uint8_t *pixelPointer = getPixelMaybeReversed(frameBuffer.color, pixelX, pixelY, frameBuffer.height, yReversed);

    // Write R, G, B, A color components converted from normalized [0,1] to byte [0,255] values
    pixelPointer[Image::RED] = castNormalizedFloatToUnsignedInt8(color.r);    // RED
    pixelPointer[Image::GREEN] = castNormalizedFloatToUnsignedInt8(color.g);  // GREEN
    pixelPointer[Image::BLUE] = castNormalizedFloatToUnsignedInt8(color.b);   // BLUE
    pixelPointer[Image::ALPHA] = castNormalizedFloatToUnsignedInt8(color.a);  // ALPHA
} // writeColor()

/******************************************************************************/
/*                                                                            */
/*                       EARLY PER FRAGMENT OPERATIONS                        */
/*                                                                            */
/******************************************************************************/

static inline bool executeEarlyPerFragmentOperations(const GPUMemory &memory, const Framebuffer &frameBuffer,
                                                     const InFragment &inFragment, const bool isFacingFront) {
    // Check if the fragment is facing front or back
    const auto &[sfail, dpfail, dppass] = isFacingFront
                                              ? memory.stencilSettings.frontOps
                                              : memory.stencilSettings.backOps;

    // === TEST 30-31 ===
    // Is stencil test active?           // Has stencil buffer?
    if(memory.stencilSettings.enabled && frameBuffer.stencil.data) {
        // Get pointer to the stencil value at the fragment's position
        uint8_t *pStencilPixel = getPixelMaybeReversed(frameBuffer.stencil, static_cast<uint32_t>(inFragment.gl_FragCoord.x),
                                                       static_cast<uint32_t>(inFragment.gl_FragCoord.y), frameBuffer.height,
                                                       frameBuffer.yReversed);

        // Read the current stencil value
        bool pass{false};

        // Stencil Test
        switch(memory.stencilSettings.func) {
            case StencilFunc::NEVER:
                pass = false; // sfail
                break;
            case StencilFunc::LESS:
                pass = *pStencilPixel < memory.stencilSettings.refValue;
                break;
            case StencilFunc::LEQUAL:
                pass = *pStencilPixel <= memory.stencilSettings.refValue;
                break;
            case StencilFunc::GREATER:
                pass = *pStencilPixel > memory.stencilSettings.refValue;
                break;
            case StencilFunc::GEQUAL:
                pass = *pStencilPixel >= memory.stencilSettings.refValue;
                break;
            case StencilFunc::EQUAL:
                pass = *pStencilPixel == memory.stencilSettings.refValue;
                break;
            case StencilFunc::NOTEQUAL:
                pass = *pStencilPixel != memory.stencilSettings.refValue;
                break;
            case StencilFunc::ALWAYS:
                pass = true; // spass
                break;
        } // switch(memory.stencilSettings.func)

        // Handle stencil test failure (sfail case)
        if(!pass) {
            // Block stencil writes?
            if(!memory.blockWrites.stencil) {
                executeStencilOperation(*pStencilPixel, sfail, memory.stencilSettings.refValue); // sfail
            }

            return false; // fragment processing is stopped
        }
    } // if(stencilTest)

    // === TEST 32-33 ===
    // Has depth buffer?
    if(frameBuffer.depth.data) {
        // Get pointer to the depth value at the fragment's position
        const auto pDepthPixel = reinterpret_cast<float*>(getPixelMaybeReversed(frameBuffer.depth, static_cast<uint32_t>(inFragment.gl_FragCoord.x),
                                                                                static_cast<uint32_t>(inFragment.gl_FragCoord.y), frameBuffer.height,
                                                                                frameBuffer.yReversed));

        // Perform depth test (z >= buffer depth means fragment is behind what's already there)
        if(inFragment.gl_FragCoord.z >= *pDepthPixel) {
            // Is stencil test active?           // Block stecnil writes?       // Has stencil buffer?
            if(memory.stencilSettings.enabled && !memory.blockWrites.stencil && frameBuffer.stencil.data) {
                uint8_t *pStencilPixel = getPixelMaybeReversed(frameBuffer.stencil,static_cast<uint32_t>(inFragment.gl_FragCoord.x),
                        static_cast<uint32_t>(inFragment.gl_FragCoord.y),frameBuffer.height, frameBuffer.yReversed);

                executeStencilOperation(*pStencilPixel, dpfail, memory.stencilSettings.refValue); // dpfail
            }

            return false; // fragment processing is stopped
        }

        // dppass
        if(!memory.blockWrites.depth){
            *pDepthPixel = inFragment.gl_FragCoord.z;
        }

        if(memory.stencilSettings.enabled && !memory.blockWrites.stencil &&
            frameBuffer.stencil.data) {
            uint8_t *pStencilPixel = getPixelMaybeReversed(frameBuffer.stencil, static_cast<uint32_t>(inFragment.gl_FragCoord.x),
                                                           static_cast<uint32_t>(inFragment.gl_FragCoord.y), frameBuffer.height,
                                                           frameBuffer.yReversed);
            executeStencilOperation(*pStencilPixel, dppass, memory.stencilSettings.refValue);
        }
    } // if(depthTest)

    return true; // fragment continues processing (goes to fragment shader)
} // executeEarlyPerFragmentOperations()

static inline void executeStencilOperation(uint8_t &stencilValue, const StencilOp stencilOperation, const uint32_t stencilValueReference) {
    switch(stencilOperation) {
        case StencilOp::KEEP:
            // Keep the current stencil value unchanged
            break;

        case StencilOp::ZERO:
            // Set stencil value to zero
            stencilValue = 0;
            break;

        case StencilOp::REPLACE:
            // Replace stencil value with the reference value
            stencilValue = static_cast<uint8_t>(stencilValueReference);
            break;

        case StencilOp::INCR:
            // Increment stencil value with saturation (clamped to 255)
            if(stencilValue < 255) {
                stencilValue++;
            }
            break;

        case StencilOp::INCR_WRAP:
            // Increment stencil value with wrapping (255+1 -> 0)
            stencilValue = static_cast<uint8_t>(stencilValue + 1);
            break;

        case StencilOp::DECR:
            // Decrement stencil value with saturation (clamped to 0)
            if(stencilValue > 0) {
                stencilValue--;
            }
            break;

        case StencilOp::DECR_WRAP:
            // Decrement stencil value with wrapping (0-1 -> 255)
            stencilValue = static_cast<uint8_t>(stencilValue - 1);
            break;

        case StencilOp::INVERT:
            // Bitwise inversion of the stencil value
            stencilValue = ~stencilValue;
            break;
    } // switch(stencilOperation)
} // executeStencilOperation()

/*** end of file gpu.cpp ***/
