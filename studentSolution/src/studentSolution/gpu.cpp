/*!
 * @file gpu.cpp
 * @brief This file contains implementation of GPU functionality.
 *
 * @author Tomáš Milet, imilet@fit.vutbr.cz
 * @author Jan kalina, xkalinj00@stud.fit.vutbr.cz
 */

#include <studentSolution/gpu.hpp>

//! [student_GPU_run]
void student_GPU_run(GPUMemory &mem, const CommandBuffer &cb) {
    for(uint32_t iCommand = 0; iCommand < cb.nofCommands; iCommand++) {
        CommandType type = cb.commands[iCommand].type;
        CommandData data = cb.commands[iCommand].data;
        handleCommand(mem, type, data);
    }
} // student_GPU_run()
//! [student_GPU_run]

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
        /*                           TODO commands                            */
        /**********************************************************************/
        case CommandType::USER_COMMAND:     // TODO: implement user command
        case CommandType::DRAW:             // TODO: implement draw command
        case CommandType::SUB_COMMAND:      // TODO: implement sub command
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

static inline void handleBindFramebufferCommand(GPUMemory &memory, const CommandData &data) {
    memory.activatedFramebuffer = data.bindFramebufferCommand.id;
} // handleBindFramebufferCommand()

static inline void handleBindProgramCommand(GPUMemory &memory, const CommandData &data) {
    memory.activatedProgram = data.bindProgramCommand.id;
} // handleBindProgramCommand()

static inline void handleBindVertexArrayCommand(GPUMemory &memory, const CommandData &data) {
    memory.activatedVertexArray = data.bindVertexArrayCommand.id;
} // handleBindVertexArrayCommand()

static inline void handleBlockWritesCommand(GPUMemory &memory, const CommandData &data) {
    memory.blockWrites = data.blockWritesCommand.blockWrites;
} // handleBlockWritesCommand()

static inline void handleSetBackfaceCullingCommand(GPUMemory &memory, const CommandData &data) {
    memory.backfaceCulling.enabled = data.setBackfaceCullingCommand.enabled;
} // handleSetBackfaceCullingCommand()

static inline void handleSetFrontFaceCommand(GPUMemory &memory, const CommandData &data) {
    memory.backfaceCulling.frontFaceIsCounterClockWise = data.setFrontFaceCommand.frontFaceIsCounterClockWise;
} // handleSetFrontFaceCommand()

static inline void handleSetStencilCommand(GPUMemory &memory, const CommandData &data) {
    memory.stencilSettings = data.setStencilCommand.settings;
} // handleSetStencilCommand()

static inline void handleSetDrawIdCommand(GPUMemory &memory, const CommandData &data) {
    memory.gl_DrawID = data.setDrawIdCommand.id;
} // handleSetDrawIdCommand()

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
                const float channelComponent = pickChannel(clearCommand.value, color.channelTypes[iChannel]);

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


/******************************************************************************/
/*                                                                            */
/*                              HELPER FUNCTIONS                              */
/*                                                                            */
/******************************************************************************/

static inline uint8_t *getPixelMaybeReversed(const Image &img, const uint32_t x, const uint32_t y,
                                             const uint32_t height, const bool yReversed) {
    const uint32_t maybeReversedY = yReversed ? (height - y - 1) : y;
    return static_cast<uint8_t*>(img.data) + (maybeReversedY * img.pitch) + (x * img.bytesPerPixel);
} // getPixelMaybeReversed()

static inline uint8_t castNormalizedFloatToUnsignedInt8(const float value) {
    return static_cast<uint8_t>(glm::clamp(value, 0.f, 1.f) * 255.f + 0.5f);
} // castNormalizedFloatToUnsignedInt8()

static inline float pickChannel(const glm::vec4 &color, const Image::Channel channel) {
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

/*** end of file gpu.cpp ***/
