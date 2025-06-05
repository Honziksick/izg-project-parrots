/*!
 * @file gpu.cpp
 * @brief This file contains implementation of GPU functionality.
 *
 * @author Tomáš Milet, imilet@fit.vutbr.cz
 * @author Jan kalina, xkalinj00@stud.fit.vutbr.cz
 */

#include <studentSolution/gpu.hpp>


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

// === TEST 12, 14-15, 17-21 ===
static inline void handleDrawCommand(GPUMemory &memory, const DrawCommand &drawCommand) {
    // === TEST 17 ===
    // Prepare constant input of shader interface (07 Vertex Processor image)
    ShaderInterface shaderInterface;
    shaderInterface.gl_DrawID = memory.gl_DrawID;  // === TEST 15 ===
    shaderInterface.uniforms = memory.uniforms;
    shaderInterface.textures = memory.textures;

    // Get the drawing program from memory (as described in === TEST 14 ===)
    const Program &program = memory.programs[memory.activatedProgram];

    // === TEST 14 ===
    // Process each I/O vertex in the draw command
    for(uint32_t iVertex = 0; iVertex < drawCommand.nofVertices; ++iVertex) {
        InVertex inVertex;
        OutVertex outVertex;

        // === TEST 18 ===
        // getVertexIndex() function supports both indexed and non-indexed drawing
        inVertex.gl_VertexID = getVertexIndex(memory, iVertex);

        // === TEST 19-21 ===
        // Assemble vertex from buffers using Vertex Assembly unit
        assembleVertex(memory, inVertex);

        // === TEST 14 ===
        // Run vertex shader for each vertex
        if(program.vertexShader) {
            program.vertexShader(outVertex, inVertex, shaderInterface);
        }
    }

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

    // For non-indexed drawing approache (we return vertexNumber as is)
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

/*** end of file gpu.cpp ***/
