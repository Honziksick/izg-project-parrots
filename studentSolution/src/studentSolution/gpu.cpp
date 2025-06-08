/*!
 * @file gpu.cpp
 * @brief This file contains implementation of GPU functionality.
 *
 * @author Tomáš Milet, imilet@fit.vutbr.cz
 * @author Jan kalina, xkalinj00@stud.fit.vutbr.cz
 */

#include <studentSolution/gpu.hpp>
#include <algorithm>  // std::min, std::max, std::fabs

/*
 * When implementing this part of the project, I maximally based my code on the
 * assignment description, graphics, hints and pseudo-code provided. I also tried
 * to comment on my thoughts and decisions so it is evident that I understand
 * the problematics even though I used the pseudo-code as a base for my
 * implementation. I'd like to thank doctor Tomáš Milet for his great work
 * on the assignment and project skeleton. I only wish more assignments were
 * like this one ;-).
 *
 * To make the implementation more effective I made the sub-function
 * `static inline` so it can be inlined by the compiler. This could lead to
 * longer compilation time, but the performance should be better in the end,
 * because there is no function call overhead. When using local variables
 * serving as constants with predefined unchanging values, I made them `constexpr`
 * to allow the compiler to optimize them even further (evaluation during compile
 * time). Because of optimization, I tryed to use switch-case statements where
 * possible as the switch-case statements are usually more efficient than
 * classic if-else statements (and often are more readable too :-) ).
 */


/******************************************************************************/
/*                                                                            */
/*                    PROTOTYPES of used custom functions                     */
/*                                                                            */
/******************************************************************************/

#ifndef XKALINJ00_GPU_SOLUTION
#define XKALINJ00_GPU_SOLUTION

void executeCommandBuffer(GPUMemory &memory, const CommandBuffer &commandBuffer);
void handleCommand(GPUMemory &memory, CommandType type, const CommandData &data);
void handleBindFramebufferCommand(GPUMemory &memory, const CommandData &commandData);
void handleBindProgramCommand(GPUMemory &memory, const CommandData &commandData);
void handleBindVertexArrayCommand(GPUMemory &memory, const CommandData &commandData);
void handleBlockWritesCommand(GPUMemory &memory, const CommandData &commandData);
void handleSetBackfaceCullingCommand(GPUMemory &memory, const CommandData &commandData);
void handleSetFrontFaceCommand(GPUMemory &memory, const CommandData &commandData);
void handleSetStencilCommand(GPUMemory &memory, const CommandData &commandData);
void handleSetDrawIdCommand(GPUMemory &memory, const CommandData &commandData);
void handleClearColorCommand(const GPUMemory &memory, const ClearColorCommand &clearColorCommand);
void handleClearDepthCommand(const GPUMemory &memory, const ClearDepthCommand &clearDepthCommand);
void handleClearStencilCommand(const GPUMemory &memory, const ClearStencilCommand &clearStencilCommand);
void handleUserCommand(const UserCommand &userCommand);
void handleDrawCommand(GPUMemory &memory, const DrawCommand &drawCommand);
void handleSubCommand(GPUMemory &memory, const CommandBuffer *pSubCommandBuffer);
uint32_t getVertexIndex(const GPUMemory &memory, uint32_t vertexIndex);
void vertexAssemblyUnit(const GPUMemory &memory, InVertex &inVertex);
glm::vec3 perspectiveDivision(const glm::vec4 &clipSpacePosition, float &oneOverW);
glm::vec3 viewportTransformation(const glm::vec3 &normalizedDeviceCoordinates, uint32_t width, uint32_t height);
bool backFaceCulling(const glm::vec3 triangleVertex[3], const BackfaceCulling &backfaceCulling);
void rasterizeTriangleUsingPineda(const GPUMemory &memory, const Program &program, const Framebuffer &frameBuffer, const ShaderInterface &shaderInterface,
                                  const OutVertex outTriangle[3], const glm::vec3 vertices[3], const float oneOverW[3]);
void interpolateFragmentAttributes(const Program &program, float lambda0, float lambda1, float lambda2, InFragment &inFragment, const OutVertex outVertices[3]);
bool executeEarlyPerFragmentOperations(const GPUMemory &memory, const Framebuffer &frameBuffer, const InFragment &inFragment, bool isFacingFront);
void executeLatePerFragmentOperations(const GPUMemory &memory, const Framebuffer &frameBuffer, const InFragment &inFragment, const OutFragment &outFragment, bool isFacingFront);
void executeStencilOperation(uint8_t &stencilValue, StencilOp stencilOperation, uint32_t stencilValueReference);
uint32_t clippingSutherlandHodgman(const Program &program, const OutVertex inputTriangle[3], OutVertex outputTriangles[2][3]);
bool isVertexInsideClipPlane(const OutVertex &vertex);
OutVertex calculateClipPlaneIntersection(const Program &program, const OutVertex &startVertex, const OutVertex &endVertex);
OutVertex interpolateVertex(const Program &program, const OutVertex &startVertex, const OutVertex &endVertex, float t);
uint8_t *getPixelMaybeReversed(const Image &image, uint32_t x, uint32_t y, uint32_t height, bool yReversed);
uint8_t castNormalizedFloatToUnsignedInt8(float value);
float castUnsignedInt8ToNormalizedFloat(uint8_t value);

#endif // XKALINJ00_GPU_SOLUTION

/******************************************************************************/
/*                                                                            */
/*                               MAIN GPU LOGIC                               */
/*                                                                            */
/******************************************************************************/

//! [student_GPU_run]
void student_GPU_run(GPUMemory &mem, const CommandBuffer &cb) {
    // === TEST 12 ===
    // Initialize the draw ID to 0 before processing commands from main-cb and sub-cbs
    mem.gl_DrawID = 0;

    // Main loop is separated into its own function, so the draw ID is correctly
    // incremented when recursively calling main loop for sub-commands
    executeCommandBuffer(mem, cb);
} // student_GPU_run()
//! [student_GPU_run]

inline void executeCommandBuffer(GPUMemory &memory, const CommandBuffer &commandBuffer) {
    // Execute each command in the command buffer
    for(uint32_t iCommand = 0; iCommand < commandBuffer.nofCommands; iCommand++) {
        handleCommand(memory, commandBuffer.commands[iCommand].type, commandBuffer.commands[iCommand].data);
    } // for(iCommand)
} // executeCommandBuffer()

inline void handleCommand(GPUMemory &memory, const CommandType type, const CommandData &data) {
    switch(type) {
        /**********************************************************************/
        /*                       03.1 BINDING commands                        */
        /**********************************************************************/
        // === TEST 0 ===
        case CommandType::BIND_FRAMEBUFFER:
            handleBindFramebufferCommand(memory, data);
            break;

        // === TEST 1 ===
        case CommandType::BIND_PROGRAM:
            handleBindProgramCommand(memory, data);
            break;

        // === TEST 2 ===
        case CommandType::BIND_VERTEXARRAY:
            handleBindVertexArrayCommand(memory, data);
            break;

        /**********************************************************************/
        /*                       03.2 SETTING commands                        */
        /**********************************************************************/
        // === TEST 3 ===
        case CommandType::BLOCK_WRITES_COMMAND:
            handleBlockWritesCommand(memory, data);
            break;

        // === TEST 4 ===
        case CommandType::SET_BACKFACE_CULLING_COMMAND:
            handleSetBackfaceCullingCommand(memory, data);
            break;

        // === TEST 5 ===
        case CommandType::SET_FRONT_FACE_COMMAND:
            handleSetFrontFaceCommand(memory, data);
            break;

        // === TEST 6 ===
        case CommandType::SET_STENCIL_COMMAND:
            handleSetStencilCommand(memory, data);
            break;

        // === TEST 7 ===
        case CommandType::SET_DRAW_ID:
            handleSetDrawIdCommand(memory, data);
            break;

        /**********************************************************************/
        /*                         04 CLEAR commands                          */
        /**********************************************************************/
        // === TEST 8-10 ===
        case CommandType::CLEAR_COLOR:
            handleClearColorCommand(memory, data.clearColorCommand);
            break;

        // === TEST 8-10 ===
        case CommandType::CLEAR_DEPTH:
            handleClearDepthCommand(memory, data.clearDepthCommand);
            break;

        // === TEST 8-10 ===
        case CommandType::CLEAR_STENCIL:
            handleClearStencilCommand(memory, data.clearStencilCommand);
            break;

        /**********************************************************************/
        /*          05 User command, numbering and subcommand buffer          */
        /**********************************************************************/
        // === TEST 11 ===
        case CommandType::USER_COMMAND:
            handleUserCommand(data.userCommand);
            break;

        // === TEST 12, 14-41 ===
        case CommandType::DRAW:
            handleDrawCommand(memory, data.drawCommand);
            break;

        // === TEST 13 ===
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
/*                           03-05 COMMAND HANDLERS                           */
/*                                                                            */
/******************************************************************************/

// === TEST 0 ===
inline void handleBindFramebufferCommand(GPUMemory &memory, const CommandData &commandData) {
    memory.activatedFramebuffer = commandData.bindFramebufferCommand.id;
} // handleBindFramebufferCommand()

// === TEST 1 ===
inline void handleBindProgramCommand(GPUMemory &memory, const CommandData &commandData) {
    memory.activatedProgram = commandData.bindProgramCommand.id;
} // handleBindProgramCommand()

// === TEST 2 ===
inline void handleBindVertexArrayCommand(GPUMemory &memory, const CommandData &commandData) {
    memory.activatedVertexArray = commandData.bindVertexArrayCommand.id;
} // handleBindVertexArrayCommand()

// === TEST 3 ===
inline void handleBlockWritesCommand(GPUMemory &memory, const CommandData &commandData) {
    memory.blockWrites = commandData.blockWritesCommand.blockWrites;
} // handleBlockWritesCommand()

// === TEST 4 ===
inline void handleSetBackfaceCullingCommand(GPUMemory &memory, const CommandData &commandData) {
    memory.backfaceCulling.enabled = commandData.setBackfaceCullingCommand.enabled;
} // handleSetBackfaceCullingCommand()

// === TEST 5 ===
inline void handleSetFrontFaceCommand(GPUMemory &memory, const CommandData &commandData) {
    memory.backfaceCulling.frontFaceIsCounterClockWise = commandData.setFrontFaceCommand.frontFaceIsCounterClockWise;
} // handleSetFrontFaceCommand()

// === TEST 6 ===
inline void handleSetStencilCommand(GPUMemory &memory, const CommandData &commandData) {
    memory.stencilSettings = commandData.setStencilCommand.settings;
} // handleSetStencilCommand()

// === TEST 7 ===
inline void handleSetDrawIdCommand(GPUMemory &memory, const CommandData &commandData) {
    memory.gl_DrawID = commandData.setDrawIdCommand.id;
} // handleSetDrawIdCommand()

// === TEST 8-10 ===
inline void handleClearColorCommand(const GPUMemory &memory, const ClearColorCommand &clearColorCommand) {
    // Select framebuffer
    const Framebuffer &framebuffer = memory.framebuffers[memory.activatedFramebuffer];

    // Does framebuffer contain color buffer?
    if(framebuffer.color.data) {
        // Clear the color buffer for each pixel
        for(uint32_t y = 0; y < framebuffer.height; y++) {
            for(uint32_t x = 0; x < framebuffer.width; x++) {
                uint8_t *pPixelStart = getPixelMaybeReversed(framebuffer.color, x, y, framebuffer.height, framebuffer.yReversed);

                for(uint32_t iChannel = 0; iChannel < framebuffer.color.channels; iChannel++) {
                    float selectedChannel{0.f};

                    // Pick the channel to clear based on the channel type
                    switch(framebuffer.color.channelTypes[iChannel]) {
                        case Image::RED:
                            selectedChannel = clearColorCommand.value.r;
                            break;
                        case Image::GREEN:
                            selectedChannel = clearColorCommand.value.g;
                            break;
                        case Image::BLUE:
                            selectedChannel = clearColorCommand.value.b;
                            break;
                        case Image::ALPHA:
                            selectedChannel = clearColorCommand.value.a;
                            break;
                        default:
                            selectedChannel = 0.f;
                            break;
                    } // switch(channel)

                    // Clear each channel (R, G, B, A) based on the image format
                    switch(framebuffer.color.format) {
                        case Image::U8:
                            pPixelStart[iChannel] = castNormalizedFloatToUnsignedInt8(selectedChannel);
                            break;
                        case Image::F32:
                            reinterpret_cast<float*>(pPixelStart)[iChannel] = selectedChannel;
                            break;
                    } // switch(format)
                } // for(iChannel)
            } // for(x)
        } // for(y)
    } // if(framebuffer.color.data)
} // handleClearColorCommand()

// === TEST 8-10 ===
inline void handleClearDepthCommand(const GPUMemory &memory, const ClearDepthCommand &clearDepthCommand) {
    // Select framebuffer
    const Framebuffer &framebuffer = memory.framebuffers[memory.activatedFramebuffer];

    // Does framebuffer contain depth buffer?
    if(framebuffer.depth.data) {
        // Clear the depth buffer for each pixel
        for(uint32_t y = 0; y < framebuffer.height; y++) {
            for(uint32_t x = 0; x < framebuffer.width; x++) {
                uint8_t *pPixelStart = getPixelMaybeReversed(framebuffer.depth, x, y, framebuffer.height, framebuffer.yReversed);

                // Depth is always 1 channel float value
                *reinterpret_cast<float*>(pPixelStart) = clearDepthCommand.value;
            } // for(x)
        } // for(y)
    } // if(framebuffer.depth.data)
} // handleClearDepthCommand()

// === TEST 8-10 ===
inline void handleClearStencilCommand(const GPUMemory &memory, const ClearStencilCommand &clearStencilCommand) {
    // Select framebuffer
    const Framebuffer &framebuffer = memory.framebuffers[memory.activatedFramebuffer];

    // Does framebuffer contain stencil buffer?
    if(framebuffer.stencil.data) {
        // Clear the stencil buffer for each pixel
        for(uint32_t y = 0; y < framebuffer.height; y++) {
            for(uint32_t x = 0; x < framebuffer.width; x++) {
                uint8_t *pPixelStart = getPixelMaybeReversed(framebuffer.stencil, x, y, framebuffer.height, framebuffer.yReversed);

                // Stencil is always 1 channel uint8_t value
                *pPixelStart = clearStencilCommand.value;
            } // for(x)
        } // for(y)
    } // if(framebuffer.stencil.data)
} // handleClearStencilCommand()

// === TEST 11 ===
inline void handleUserCommand(const UserCommand &userCommand) {
    // If user collback is NULL we ignore it as described in test 11
    if(userCommand.callback) {
        userCommand.callback(userCommand.data);
    }
} // handleUserCommand()

// === TEST 12, 14-41 ===
inline void handleDrawCommand(GPUMemory &memory, const DrawCommand &drawCommand) {
    // Get the drawing program from memory (as described in === TEST 14 ===)
    const Program &program = memory.programs[memory.activatedProgram];

    // We skip drawing and only increment the draw ID if the program is not set
    // Note: Putting this check at the beginning of the function minimalizes
    //       the number of unnecessary lines executed in the function.
    if(!program.vertexShader) {
        memory.gl_DrawID++;  // === TEST 12 ==
        return;
    }

    // Prepare constant input of shader interface ('07 Vektorová čast GPU: část vertexů')
    ShaderInterface shaderInterface;
    shaderInterface.gl_DrawID = memory.gl_DrawID;  // === TEST 15 ===
    shaderInterface.uniforms = memory.uniforms;    // === TEST 17 ===
    shaderInterface.textures = memory.textures;    // === TEST 17 ===

    // Get the currently activated framebuffer from memory
    const auto &frameBuffer = memory.framebuffers[memory.activatedFramebuffer];

    // We process each triangle - triangle has 3 vertices, thus we increment by 3
    for(uint32_t iTriangleStart = 0; iTriangleStart < drawCommand.nofVertices; iTriangleStart += 3) {
        OutVertex outTriangle[3];

        // === TEST 14, 18, 19-21 ===
        // Vertex Processor & Vertex Assembly Unit ('07 Vektorová čast GPU: část vertexů')
        for(int iVertex = 0; iVertex < 3; iVertex++) {
            InVertex inVertex;

            // === TEST 18 ===
            // getVertexIndex() function supports both indexed and non-indexed drawing
            inVertex.gl_VertexID = getVertexIndex(memory, iTriangleStart + iVertex);

            // === TEST 19-21 ===
            // Assemble vertex from buffers using Vertex Assembly unit
            vertexAssemblyUnit(memory, inVertex);

            // === TEST 14 ===
            // Run vertex shader for each vertex
            program.vertexShader(outTriangle[iVertex], inVertex, shaderInterface);
        } // for(iVertex)

        // === TEST 38-41 ===
        // Apply triangle clipping using Sutherland-Hodgman algorithm
        OutVertex clippedTriangles[2][3];
        const uint32_t clippedTrinaglesCount = clippingSutherlandHodgman(program, outTriangle, clippedTriangles);

        for(uint32_t iClippedTriangle = 0; iClippedTriangle < clippedTrinaglesCount; iClippedTriangle++) {
            glm::vec3 screenSpaceVertices[3];
            float oneOverW[3];

            // === TEST 25 ===
            // Apply viewport transform to each vertex of the clipped triangle
            for(int iVertex = 0; iVertex < 3; iVertex++) {
                // Perform perspective division to convert from clip space to normalized device coordinates (NDC)
                const glm::vec3 normalizedDeviceCoordinates = perspectiveDivision(clippedTriangles[iClippedTriangle][iVertex].gl_Position, oneOverW[iVertex]);

                // Transform NDC coordinates (range [-1,1]) to screen space coordinates (range [0,width/height])
                screenSpaceVertices[iVertex] = viewportTransformation(normalizedDeviceCoordinates, frameBuffer.width, frameBuffer.height);
            } // for(iVertex)

            // === TEST 26 ===
            if(backFaceCulling(screenSpaceVertices, memory.backfaceCulling)) {
                continue;
            }

            // === TEST 22-24, 27-37 ===
            // All these steps are handled by the rasterizeTriangleUsingPineda function
            rasterizeTriangleUsingPineda(memory,                              // GPU state
                                         program,                             // active program
                                         frameBuffer,                         // active framebuffer
                                         shaderInterface,                     // constants for fragment shader
                                         clippedTriangles[iClippedTriangle],  // vertex shader outputs
                                         screenSpaceVertices,                 // vertices in screen-space
                                         oneOverW);                           // 1/w for perspective correction
        } // for(iClippedTriangle)
    } // for(iTriangle)

    // === TEST 12 ===
    memory.gl_DrawID++;  // increment the draw ID for each draw command
} // handleDrawCommand()

// === TEST 13 ===
inline void handleSubCommand(GPUMemory &memory, const CommandBuffer *pSubCommandBuffer) {
    // Note: In this implementation I slightly deviated from the given pseudo-code.
    //       In my version, I spared minimally one if-else statement and also
    //       rather elegantly solved the Draw ID incrementation as shown int the
    //       "black" Command Buffer picture under the TEST 12 description.

    // If subCommand isn't NULL, we recursively execute the (sub)command buffer
    if(pSubCommandBuffer) {
        executeCommandBuffer(memory, *pSubCommandBuffer);
    }
} // handleSubCommand()


/******************************************************************************/
/*                                                                            */
/*           VERTEX PART of VECTOR GPU PART - VERTEX ASSEMBLY UNIT            */
/*                   ('07 Vektorová čast GPU: část vertexů')                  */
/*                                                                            */
/******************************************************************************/

// === TEST 18 ===
inline uint32_t getVertexIndex(const GPUMemory &memory, const uint32_t vertexIndex) {
    // Activated settings table for vertex assembly unit
    const VertexArray &vertexArray = memory.vertexArrays[memory.activatedVertexArray];

    // Is indexing turned on?
    // 'indexBufferID' is a number of a bufferu or -1 if index buffer is turned off (mentioned in test 18)
    if(vertexArray.indexBufferID >= 0) {
        // Index buffer
        const Buffer indexBuffer = memory.buffers[vertexArray.indexBufferID];

        // If index buffer is not set, we return the vertexIndex as is
        if(!indexBuffer.data) {
            return vertexIndex;
        }

        // Read the index value based on possible index type (U8=1, U16=2, U32=4)
        switch(vertexArray.indexType) {
            case IndexType::U8: {
                auto ptr = static_cast<const uint8_t*>(indexBuffer.data);  // pointer to the index buffer data
                ptr += vertexArray.indexOffset;                            // move pointer to the index offset
                return ptr[vertexIndex];
            }
            case IndexType::U16: {
                const auto ptr = reinterpret_cast<const uint16_t*>(static_cast<const uint8_t*>(indexBuffer.data) + vertexArray.indexOffset);
                return ptr[vertexIndex];
            }
            case IndexType::U32: {
                const auto ptr = reinterpret_cast<const uint32_t*>(static_cast<const uint8_t*>(indexBuffer.data) + vertexArray.indexOffset);
                return ptr[vertexIndex];
            }
            default:
                return vertexIndex;
        } // switch(vertexArray.indexType)
    } // if(indexing is turned on)
    else {
        // If indexing is turned off, we return 'vertexIndex' as is
        return vertexIndex;
    }
} // getVertexIndex()

// === TEST 19-21 ===
inline void vertexAssemblyUnit(const GPUMemory &memory, InVertex &inVertex) {
    const VertexArray &vertexArray = memory.vertexArrays[memory.activatedVertexArray];

    // For each vertex attribute, we read the data from the corresponding buffer
    for(uint32_t iAttribute = 0; iAttribute < maxAttribs; iAttribute++) {
        const auto &[attributeBufferID, attributeStride, attrbitueOffset, attributeType] = vertexArray.vertexAttrib[iAttribute];

        // We proccess only active attributes (ad described in '07 Tabulka nastavení Vertex Array')
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

                // Should never happen (but better sure than sorry)
                case AttribType::EMPTY:
                default:
                    break;
            } // switch(attributeType)
        } // if(attributeType != AttribType::EMPTY && attributeBufferID >= 0)
    } // for(iAttribute)
} // assembleVertex()


/******************************************************************************/
/*                                                                            */
/*        PRIMITIVE PART of VECTOR GPU PART - PRIMITIVE ASSEMBLY UNIT,        */
/*          PERSPECTIVE DIVISION, VIEWPORT TRANSFORMATION & CULLING           */
/*                  ('08 Vektorová část GPU: část přimitiv')                  */
/*                                                                            */
/******************************************************************************/

// === TEST 25 ===
inline glm::vec3 perspectiveDivision(const glm::vec4 &clipSpacePosition, float &oneOverW) {
    // Calculate inverse of w-component (1/w) for perspective division
    oneOverW = 1.f / clipSpacePosition.w;

    // Perform perspective division to convert from clip space to normalized device coordinates (NDC)
    return glm::vec3(clipSpacePosition) * oneOverW;
} // perspectiveDivision()

// === TEST 25 ===
inline glm::vec3 viewportTransformation(const glm::vec3 &normalizedDeviceCoordinates,
                                        const uint32_t width, const uint32_t height) {
    glm::vec3 screenSpacePosition;

    // Transform NDC coordinates (range [-1,1]) to screen space coordinates (range [0,width/height])
    screenSpacePosition.x = (normalizedDeviceCoordinates.x * 0.5f + 0.5f) * static_cast<float>(width);
    screenSpacePosition.y = (normalizedDeviceCoordinates.y * 0.5f + 0.5f) * static_cast<float>(height);
    screenSpacePosition.z = normalizedDeviceCoordinates.z;

    return screenSpacePosition;
} // viewportTransformation()

// === TEST 26 ===
inline bool backFaceCulling(const glm::vec3 triangleVertex[3], const BackfaceCulling &backfaceCulling) {
    // Skip culling if backface culling is disabled
    if(!backfaceCulling.enabled) {
        return false;
    }

    /*
     * Note: The backface culling is based on description in presentation
     *       'Řešení viditelnosti objektů'.
     */

    // Calculate the normal of the triangle using the cross product of two edges
    const glm::vec3 edge1 = triangleVertex[1] - triangleVertex[0];
    const glm::vec3 edge2 = triangleVertex[2] - triangleVertex[0];
    const glm::vec3 triangleNormal = glm::cross(edge1, edge2);

    // View direction is assumed to be along the positive Z-axis in clip space
    constexpr glm::vec3 viewVector(0.f, 0.f, 1.f);

    // Test of orientation of the triangle normal to the view direction is computed by dot-product
    // Positive area indicates counter-clockwise winding, clockwise winding is negative.
    const float dotProduct = glm::dot(triangleNormal, viewVector);

    // Determine triangle orientation (true for counter-clockwise)
    const bool isFrontFacing = (dotProduct > 0.f);

    // Determine if the triangle should be culled
    return (isFrontFacing != backfaceCulling.frontFaceIsCounterClockWise);
} // backFaceCulling()


/******************************************************************************/
/*                                                                            */
/*       RASTERIZATION inspired by IZG labs - ATTRIBUTES INTERPOLATION,       */
/*   2D BARYCENTRIC COORD. FOR DEPTH INTERPOLATION AND PERSPECTIVE-CORRECT    */
/*            BARYCENTRIC COORD. FOR USER ATTRIBUTE INTERPOLATION             */
/*                             ('09 Rasterizace')                             */
/*                                                                            */
/******************************************************************************/

// === TEST 22-24, 27-29 ===
inline void rasterizeTriangleUsingPineda(const GPUMemory &memory,
    /**********************************/ const Program &program,
    /*                    C           */ const Framebuffer &frameBuffer,
    /*   v(CA) = -v(AC)  / \  v(BC)   */ const ShaderInterface &shaderInterface,
    /*                  /   \         */ const OutVertex outTriangle[3],
    /*                 A-----B        */ const glm::vec3 vertices[3],
    /*                  v(AB)         */ const float oneOverW[3]) {
    /**************************************************************************/
    /*                Calculate vectors/edges of the triangle                 */
    /**************************************************************************/
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
    const float edgeX02 = -edgeX20;
    const float edgeY02 = -edgeY20;


    /**************************************************************************/
    /*            Determine trinagle orientation based on its area            */
    /**************************************************************************/

    // Calculate the signed double-area of the triangle
    const float signedDoubleArea = edgeX01 * edgeY02 - edgeY01 * edgeX02;

    // Check if the area is valid (a non-zero value)
    if(signedDoubleArea == 0.f) {
        return;
    }

    // Calculate the absolute value of the signed double area for barycentric coordinates
    const float triangleArea = std::fabs(signedDoubleArea);

    // The area is positive for counter-clockwise triangles and negative for clockwise triangles
    const float orientation = signedDoubleArea > 0.f ? 1.f : -1.f;


    /**************************************************************************/
    /*   Calculate edge equations and step values for Pineda rasterization    */
    /**************************************************************************/

    // Calculate line equation coefficients (Ax + By + C = 0) for each edge
    const float edge01CoefficientA = -edgeY01 * orientation;  // A = -(y1 - y0)
    const float edge01CoefficientB = edgeX01 * orientation;   // B =  (x1 - x0)
    const float edge01CoefficientC = -(edge01CoefficientA * vertices[0].x + edge01CoefficientB * vertices[0].y);

    const float edge12CoefficientA = -edgeY12 * orientation;  // A = -(y2 - y1)
    const float edge12CoefficientB = edgeX12 * orientation;   // B =  (x2 - x1)
    const float edge12CoefficientC = -(edge12CoefficientA * vertices[1].x + edge12CoefficientB * vertices[1].y);

    const float edge20CoefficientA = -edgeY20 * orientation;  // A = -(y0 - y2)
    const float edge20CoefficientB = edgeX20 * orientation;   // B =  (x0 - x2)
    const float edge20CoefficientC = -(edge20CoefficientA * vertices[2].x + edge20CoefficientB * vertices[2].y);

    // X-direction increments for edge functions (equals to A coefficient)
    const float edgeStep12X = edge12CoefficientA;
    const float edgeStep20X = edge20CoefficientA;
    const float edgeStep01X = edge01CoefficientA;

    // Y-direction increments for edge functions (equals to B coefficient)
    const float edgeStep12Y = edge12CoefficientB;
    const float edgeStep20Y = edge20CoefficientB;
    const float edgeStep01Y = edge01CoefficientB;


    /**************************************************************************/
    /*                 Calculate bounding box of the triangle                 */
    /**************************************************************************/

    // Find the min/max coordinates among the triangle vertices
    const float minVertexX = std::min({vertices[0].x, vertices[1].x, vertices[2].x});
    const float maxVertexX = std::max({vertices[0].x, vertices[1].x, vertices[2].x});
    const float minVertexY = std::min({vertices[0].y, vertices[1].y, vertices[2].y});
    const float maxVertexY = std::max({vertices[0].y, vertices[1].y, vertices[2].y});

    // Round the minimum coordinate down and maximum coordinate up
    const int minVertexFlooredX = static_cast<int>(std::floor(minVertexX));
    const int maxVertexCeiledX = static_cast<int>(std::ceil(maxVertexX));
    const int minVertexFlooredY = static_cast<int>(std::floor(minVertexY));
    const int maxVertexCeiledY = static_cast<int>(std::ceil(maxVertexY));

    // Clamp the coordinates to the framebuffer boundaries
    //                         | vertices extreme | MINimal pixel coord. | MAXimal pixel coordinate               |
    const int minX = glm::clamp(minVertexFlooredX,  0,                     static_cast<int>(frameBuffer.width - 1));
    const int maxX = glm::clamp(maxVertexCeiledX,   0,                     static_cast<int>(frameBuffer.width - 1));
    const int minY = glm::clamp(minVertexFlooredY,  0,                     static_cast<int>(frameBuffer.height - 1));
    const int maxY = glm::clamp(maxVertexCeiledY,   0,                     static_cast<int>(frameBuffer.height - 1));


    /**************************************************************************/
    /*                   Prepare for scanline rasterization                   */
    /*   (inspired by 'Vyplňování 2D oblastí' presentation and assignment)    */
    /**************************************************************************/

    // Position of the first pixel center in the bounding box
    // Note: Adding '0.5' ensures we sample at pixel centers, not corners
    float firstPixelX = minX + 0.5f;
    float firstPixelY = minY + 0.5f;

    // Evaluate edge functions at the first pixel center
    float edge12RowStart = edge12CoefficientA * firstPixelX + edge12CoefficientB * firstPixelY + edge12CoefficientC;
    float edge20RowStart = edge20CoefficientA * firstPixelX + edge20CoefficientB * firstPixelY + edge20CoefficientC;
    float edge01RowStart = edge01CoefficientA * firstPixelX + edge01CoefficientB * firstPixelY + edge01CoefficientC;

    // Apply 'top-left' rule to prevent double-drawing shared edges
    // Note: Pixels exactly on an edge are drawn only if the edge is a top or left edge
    const bool edge01TopLeft = (edge01CoefficientB > 0) || (edge01CoefficientB == 0 && edge01CoefficientA > 0);
    const bool edge12TopLeft = (edge12CoefficientB > 0) || (edge12CoefficientB == 0 && edge12CoefficientA > 0);
    const bool edge20TopLeft = (edge20CoefficientB > 0) || (edge20CoefficientB == 0 && edge20CoefficientA > 0);


    /**************************************************************************/
    /*   Rasterization loop using Pineda's edge functions and Scanline fill   */
    /**************************************************************************/

    // === TEST 22-24 ==
    // Rasterization loop over the bounding box of the triangle using Pineda's edge functions
    for(int y = minY; y <= maxY; y++) {
        // Initialize edge function values at the start of this scanline
        float edgeFunction12 = edge12RowStart;
        float edgeFunction20 = edge20RowStart;
        float edgeFunction01 = edge01RowStart;

        for(int x = minX; x <= maxX; x++) {
            // Determine if pixel is inside triangle using edge functions
            // Includes top-left rule to avoid double-drawing shared edges
            const bool isInsideEdge12 = (edgeFunction12 > 0 || (edgeFunction12 == 0 && edge12TopLeft));
            const bool isInsideEdge20 = (edgeFunction20 > 0 || (edgeFunction20 == 0 && edge20TopLeft));
            const bool isInsideEdge01 = (edgeFunction01 > 0 || (edgeFunction01 == 0 && edge01TopLeft));

            // Point is inside the triangle if all edge functions have the same sign
            if(isInsideEdge12 && isInsideEdge20 && isInsideEdge01) {
                // Calculate barycentric coordinates for interpolation
                const float lambda0 = edgeFunction12 / triangleArea;
                const float lambda1 = edgeFunction20 / triangleArea;
                const float lambda2 = edgeFunction01 / triangleArea;

                // === TEST 27 ===
                // Calculating 2D Barycentric coordinates for depth interpolation
                // fragment.gl_FragCoord.z = vertex[0].gl_Position.z * λ0_2D + vertex[1].gl_Position.z * λ1_2D + vertex[2].gl_Position.z * λ2_2D
                const float depth = vertices[0].z * lambda0 + vertices[1].z * lambda1 + vertices[2].z * lambda2;

                // Perspective correction for attribute interpolation
                const float perspectiveLambda0 = lambda0 * oneOverW[0];
                const float perspectiveLambda1 = lambda1 * oneOverW[1];
                const float perspectiveLambda2 = lambda2 * oneOverW[2];

                // Sum of perspective-corrected weights for normalization
                // s  = (λ0_2D / h0) + (λ1_2D / h1) + (λ2_2D / h2);
                const float sumWeight = perspectiveLambda0 + perspectiveLambda1 + perspectiveLambda2;
                const float oneOverSumWeight = 1.0f / sumWeight;

                // Normalized perspective-corrected weights
                const float l0 = perspectiveLambda0 * oneOverSumWeight;  // λ0 = (λ0_2D / h0) / s;
                const float l1 = perspectiveLambda1 * oneOverSumWeight;  // λ1 = (λ1_2D / h1) / s;
                const float l2 = perspectiveLambda2 * oneOverSumWeight;  // λ2 = (λ2_2D / h2) / s;

                // Set up fragment data structure (as described in '09 Rasterizace: InFragment')
                InFragment inFragment;
                inFragment.gl_FragCoord = glm::vec4(x + 0.5f, y + 0.5f, depth, oneOverSumWeight);

                // === TEST 28-29 ===
                // Interpolation of attributes for the fragment shader
                interpolateFragmentAttributes(program, l0, l1, l2, inFragment, outTriangle);

                // Determine front/back face for culling and stencil operations
                const bool isFacingFront = ((signedDoubleArea > 0) == memory.backfaceCulling.frontFaceIsCounterClockWise);

                // === TEST 30-33 ===
                // If EPFO returned false, we skip the fragment shader execution
                if(!executeEarlyPerFragmentOperations(memory, frameBuffer, inFragment, isFacingFront)) {
                    edgeFunction12 += edgeStep12X;
                    edgeFunction20 += edgeStep20X;
                    edgeFunction01 += edgeStep01X;
                    continue;
                }

                // === TEST 22 ===
                // Call the fragment shader
                OutFragment outFragment;
                program.fragmentShader(outFragment, inFragment, shaderInterface);

                // === TEST 22-24 ===
                // Apply LPFO and write the color to the framebuffer
                executeLatePerFragmentOperations(memory, frameBuffer, inFragment, outFragment, isFacingFront);
            } // if(shouldDraw)

            // Increment edge function values for the next pixel in this scanline
            edgeFunction12 += edgeStep12X;
            edgeFunction20 += edgeStep20X;
            edgeFunction01 += edgeStep01X;
        } // for(x)

        // Increment edge function values for the start of the next scanline
        edge12RowStart += edgeStep12Y;
        edge20RowStart += edgeStep20Y;
        edge01RowStart += edgeStep01Y;
    } // for(y)
} // rasterizeTriangleUsingPineda()

// === TEST 28-29 ===
inline void interpolateFragmentAttributes(const Program &program, const float lambda0,
                                          const float lambda1, const float lambda2,
                                          InFragment &inFragment, const OutVertex outVertices[3]) {
    // Iterate through all possible vertex attributes
    for(uint32_t iAttribute = 0; iAttribute < maxAttribs; iAttribute++) {
        // fragment.attribute = vertex[0].attribute * λ0 + vertex[1].attribute * λ1 + vertex[2].attribute * λ2
        switch(program.vs2fs[iAttribute]) {
            case AttribType::EMPTY: {
                // Skip empty attributes
                continue;
            }
            case AttribType::FLOAT: {
                // Interpolate 'float' value using barycentric coordinates
                float vertex0 = outVertices[0].attributes[iAttribute].v1;
                float vertex1 = outVertices[1].attributes[iAttribute].v1;
                float vertex2 = outVertices[2].attributes[iAttribute].v1;

                inFragment.attributes[iAttribute].v1 = vertex0 * lambda0 + vertex1 * lambda1 + vertex2 * lambda2;
                break;
            }
            case AttribType::VEC2: {
                // Interpolate 'vec2' components using barycentric coordinates
                glm::vec2 vertex0 = outVertices[0].attributes[iAttribute].v2;
                glm::vec2 vertex1 = outVertices[1].attributes[iAttribute].v2;
                glm::vec2 vertex2 = outVertices[2].attributes[iAttribute].v2;

                inFragment.attributes[iAttribute].v2 = vertex0 * lambda0 + vertex1 * lambda1 + vertex2 * lambda2;
                break;
            }
            case AttribType::VEC3: {
                // Interpolate 'vec3' components using barycentric coordinates
                glm::vec3 vertex0 = outVertices[0].attributes[iAttribute].v3;
                glm::vec3 vertex1 = outVertices[1].attributes[iAttribute].v3;
                glm::vec3 vertex2 = outVertices[2].attributes[iAttribute].v3;

                inFragment.attributes[iAttribute].v3 = vertex0 * lambda0 + vertex1 * lambda1 + vertex2 * lambda2;
                break;
            }
            case AttribType::VEC4: {
                // Interpolate 'vec4' components using barycentric coordinates
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
                // For 'unsigned integer' attributes, we use flat shading
                inFragment.attributes[iAttribute] = outVertices[0].attributes[iAttribute];
                continue;
            }
            default:
                break;
        } // switch(AttributeType)
    } // for(iAttribute)
} // interpolateFragmentAttributes()


/******************************************************************************/
/*                                                                            */
/*                       10-13 PER FRAGMENT OPERATIONS                        */
/*         (implementation of these function is strongly based on the         */
/*            description and colorfuĺ graphics in the assignment)            */
/*                                                                            */
/******************************************************************************/

inline bool executeEarlyPerFragmentOperations(const GPUMemory &memory, const Framebuffer &frameBuffer,
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
        } // switch(stencil_function)

        // Handle stencil test failure (sfail case)
        if(!pass) {
            // Block stencil writes?
            if(!memory.blockWrites.stencil) {
                // Mofidy stencil buffer using sfail Op
                executeStencilOperation(*pStencilPixel, sfail, memory.stencilSettings.refValue); // sfail
            }

            return false; // fragment processing is stopped
        }
    } // if(stencilTest)

    // spass -> going to do depth test

    // === TEST 32-33 ===
    // Has depth buffer?
    if(frameBuffer.depth.data) {
        // Get pointer to the depth value at the fragment's position
        const auto pDepthPixel = reinterpret_cast<float*>(getPixelMaybeReversed(frameBuffer.depth, static_cast<uint32_t>(inFragment.gl_FragCoord.x),
                                                                                static_cast<uint32_t>(inFragment.gl_FragCoord.y), frameBuffer.height,
                                                                                frameBuffer.yReversed));

        // Perform depth test (z >= buffer depth means fragment is behind what's already there)
        if(*pDepthPixel > inFragment.gl_FragCoord.z) {
            // dppass
            if(!memory.blockWrites.depth) {
                *pDepthPixel = inFragment.gl_FragCoord.z;
            }

            // Is stencil test active?           // BlockStencil Writes         // Has stencil buffer?
            if(memory.stencilSettings.enabled && !memory.blockWrites.stencil && frameBuffer.stencil.data) {
                uint8_t *pStencilPixel = getPixelMaybeReversed(frameBuffer.stencil, static_cast<uint32_t>(inFragment.gl_FragCoord.x),
                                                               static_cast<uint32_t>(inFragment.gl_FragCoord.y), frameBuffer.height,
                                                               frameBuffer.yReversed);

                // Mofidy stencil buffer using dppas Op
                executeStencilOperation(*pStencilPixel, dppass, memory.stencilSettings.refValue); // dppass
            }

            return true; // fragment processing continues (goes to fragment shader)
        }
        // dpfail
        else {
            // Is stencil test active?           // Block stecnil writes?       // Has stencil buffer?
            if(memory.stencilSettings.enabled && !memory.blockWrites.stencil && frameBuffer.stencil.data) {
                uint8_t *pStencilPixel = getPixelMaybeReversed(frameBuffer.stencil, static_cast<uint32_t>(inFragment.gl_FragCoord.x),
                                                               static_cast<uint32_t>(inFragment.gl_FragCoord.y), frameBuffer.height, frameBuffer.yReversed);

                // Mofidy stencil buffer using dpfail Op
                executeStencilOperation(*pStencilPixel, dpfail, memory.stencilSettings.refValue); // dpfail
            }

            return false; // fragment processing is stopped
        }
    } // if(depthTest)

    return true; // fragment processing continues (goes to fragment shader)
} // executeEarlyPerFragmentOperations()

inline void executeLatePerFragmentOperations(const GPUMemory &memory, const Framebuffer &frameBuffer,
                                             const InFragment &inFragment, const OutFragment &outFragment,
                                             const bool isFacingFront) {
    // === TEST 34 ===
    // Discarding
    if(outFragment.discard) {
        return;
    }

    // === TEST 35 ===
    // Stencil writes
    // Is stencil test active?           // Block stencil writes?       // Has stencil buffer?
    if(memory.stencilSettings.enabled && !memory.blockWrites.stencil && frameBuffer.stencil.data) {
        // Check if the fragment is facing front or back
        const auto &[sfail, dpfail, dppass] = isFacingFront
                                                  ? memory.stencilSettings.frontOps
                                                  : memory.stencilSettings.backOps;

        // Get pointer to the stencil value at the fragment's position
        uint8_t *pStencilPixel = getPixelMaybeReversed(frameBuffer.stencil, static_cast<uint32_t>(inFragment.gl_FragCoord.x),
                                                       static_cast<uint32_t>(inFragment.gl_FragCoord.y), frameBuffer.height,
                                                       frameBuffer.yReversed);

        // Mofidy stencil buffer using dppas Op
        executeStencilOperation(*pStencilPixel, dppass, memory.stencilSettings.refValue);
    } // if(stencil_writes)

    // === TEST 36 ===
    // Depth writes
    // Block depth writes?          // Has depth buffer?
    if(!memory.blockWrites.depth && frameBuffer.depth.data) {
        // Get pointer to the depth value at the fragment's position
        const auto pDepthPixel = reinterpret_cast<float*>(getPixelMaybeReversed(frameBuffer.depth, static_cast<uint32_t>(inFragment.gl_FragCoord.x),
                                                                                static_cast<uint32_t>(inFragment.gl_FragCoord.y), frameBuffer.height,
                                                                                frameBuffer.yReversed));
        // Modify depth buffer
        *pDepthPixel = inFragment.gl_FragCoord.z;
    } // depth write

    // === TEST 37 ===
    // Color writes
    // Block color writes?          // Has color buffer?
    if(!memory.blockWrites.color && frameBuffer.color.data) {
        uint8_t *pColorPixel = getPixelMaybeReversed(frameBuffer.color, static_cast<uint32_t>(inFragment.gl_FragCoord.x),
                                                     static_cast<uint32_t>(inFragment.gl_FragCoord.y), frameBuffer.height,
                                                     frameBuffer.yReversed);

        // We need to convert the color from byte <0, 255> to normalized float <0.0, 1.0>
        glm::vec4 existingColor;
        existingColor.r = castUnsignedInt8ToNormalizedFloat(pColorPixel[Image::RED]);
        existingColor.g = castUnsignedInt8ToNormalizedFloat(pColorPixel[Image::GREEN]);
        existingColor.b = castUnsignedInt8ToNormalizedFloat(pColorPixel[Image::BLUE]);

        // Support both RGBA and RGB formats (just to be sure, even though the assignment states we use RGBA)
        const bool hasAlphaChannel = (frameBuffer.color.bytesPerPixel == 4);
        existingColor.a = hasAlphaChannel
                              ? castUnsignedInt8ToNormalizedFloat(pColorPixel[Image::ALPHA])
                              : 0.f;

        // New fragment color
        const glm::vec4 fragmentColor = outFragment.gl_FragColor;
        const float alpha = glm::clamp(fragmentColor.a, 0.f, 1.f);

        // Alpha Blending: colorBufferRGB = colorBufferRGB * (1 - alpha) + gl_FragColorRGB * alpha
        const auto blendedR = existingColor.r * (1.f - alpha) + fragmentColor.r * alpha;
        const auto blendedG = existingColor.g * (1.f - alpha) + fragmentColor.g * alpha;
        const auto blendedB = existingColor.b * (1.f - alpha) + fragmentColor.b * alpha;
        const auto blendedA = existingColor.a * (1.f - alpha) + alpha;

        // Modify color buffer
        // We revert the color back to byte <0, 255> and write it to the framebuffer
        for(uint32_t iChannel = 0; iChannel < frameBuffer.color.channels; iChannel++) {
            switch(frameBuffer.color.channelTypes[iChannel]) {
                case Image::RED:
                    pColorPixel[iChannel] = castNormalizedFloatToUnsignedInt8(blendedR);
                    break;
                case Image::GREEN:
                    pColorPixel[iChannel] = castNormalizedFloatToUnsignedInt8(blendedG);
                    break;
                case Image::BLUE:
                    pColorPixel[iChannel] = castNormalizedFloatToUnsignedInt8(blendedB);
                    break;
                case Image::ALPHA:
                    if(hasAlphaChannel) {
                        pColorPixel[iChannel] = castNormalizedFloatToUnsignedInt8(blendedA);
                    }
                    break;
                default:
                    break;
            } // switch(channelTypes[iChannel])
        } // for(iChannel)
    } // color write
} // executeLatePerFragmentOperations()

inline void executeStencilOperation(uint8_t &stencilValue, const StencilOp stencilOperation,
                                    const uint32_t stencilValueReference) {
    /*
     * Note: Of course, INCR_WRAP and DECR_WRAP operations could be easily implemented
     *       by using if-else statement, but that would be inefficient. As an
     *       ISU course survivor and near God of Assembler title holder, I decided
     *       to use the overflow properties of unsigned integer and implemented
     *       these to operations using simple +1/-1 and casting the result to uint8_t.
     */

    switch(stencilOperation) {
        // Leaves the value in the stencil buffer unchanged
        case StencilOp::KEEP:
            break;

        // Resets the value in the stencil buffer
        case StencilOp::ZERO:
            stencilValue = 0;
            break;

        // Writes the reference value in the stencil buffer StencilSettings::refValue
        case StencilOp::REPLACE:
            stencilValue = static_cast<uint8_t>(stencilValueReference);
            break;

        // Increments the value in the stencil buffer, if it is 255, leaves it
        case StencilOp::INCR:
            if(stencilValue < 255) {
                stencilValue++;
            }
            break;

        // Increments the value in the stencil buffer, if it is 255, writes 0
        case StencilOp::INCR_WRAP:
            stencilValue = static_cast<uint8_t>(stencilValue + 1);
            break;

        // Decrements the value in the stencil buffer, if it is 0, leaves it
        case StencilOp::DECR:
            if(stencilValue > 0) {
                stencilValue--;
            }
            break;

        // Decrements the value in the stencil buffer, if it is 0, writes 255
        case StencilOp::DECR_WRAP:
            stencilValue = static_cast<uint8_t>(stencilValue - 1);
            break;

        // Bitwise inverts the value in the stencil buffer
        case StencilOp::INVERT:
            stencilValue = ~stencilValue;
            break;
    } // switch(stencilOperation)
} // executeStencilOperation()


/********************************************************************************/
/*                                                                              */
/*      14 CLIPPING based on IZG presentation description and INSPIRED BY       */
/* https://www.geeksforgeeks.org/polygon-clipping-sutherland-hodgman-algorithm/ */
/*         (Note: the snippets in the URL were majorly modified, which          */
/*        proves my understanding of them and thus it is not plagiarism)        */
/*                                                                              */
/********************************************************************************/

inline uint32_t clippingSutherlandHodgman(const Program &program, const OutVertex inputTriangle[3],
                                          OutVertex outputTriangles[2][3]) {
    /*
     * Note: When choosing the clipping algorithm to use (from those described
     *       in the IZG lecture presentation), I was mainly choosing between
     *       Sutherland-Hodgman and Weiler-Atherton algorithms as Weiler-Atherton
     *       algorithm was one of my "practical" final exam questions. In the
     *       end, I decided to go with much easier Sutherland-Hodgman algorithm -
     *       even though it is not as powerful as Weiler-Atherton (it only works
     *       for convex polygons), it is much easier to implement and the convex
     *       polygons limitation is not a problem, because we are clipping
     *       triangles, which are always convex.
     */

    constexpr uint32_t vertexCount{3}; // local constant

    // Initialize buffer for storing the clipped polygon (may have up to 4 vertices)
    OutVertex clippedTriangle[4];
    uint32_t clippedVertexCount{0};

    // Process each edge of the input triangle using Sutherland-Hodgman algorithm
    for(uint32_t iVertex = 0; iVertex < vertexCount; iVertex++) {
        const OutVertex &previousVertex = inputTriangle[(iVertex + vertexCount - 1) % vertexCount];
        const OutVertex &currentVertex = inputTriangle[iVertex];

        // Determine if vertices are inside or outside the clipping plane
        const bool isPreviousInside = isVertexInsideClipPlane(previousVertex);
        const bool isCurrentInside = isVertexInsideClipPlane(currentVertex);

        // Clip the triangle as shown in the picture '14 Ořez: Teorie ořezu'
        // Both vertices inside - keep the current vertex
        if(isPreviousInside && isCurrentInside) {
            clippedTriangle[clippedVertexCount] = currentVertex;
            clippedVertexCount++;
        }
        // Previous inside, current outside - add the intersection point
        else if(isPreviousInside && !isCurrentInside) {
            clippedTriangle[clippedVertexCount] = calculateClipPlaneIntersection(program, previousVertex, currentVertex);
            clippedVertexCount++;
        }
        // Previous outside, current inside - add intersection point and current vertex
        else if(!isPreviousInside && isCurrentInside) {
            clippedTriangle[clippedVertexCount] = calculateClipPlaneIntersection(program, previousVertex, currentVertex);
            clippedVertexCount++;

            clippedTriangle[clippedVertexCount] = currentVertex;
            clippedVertexCount++;
        }
        // Both vertices outside - discard (add nothing)
        else {
            continue;
        }
    } // for(iVertex)

    // Handle results based on number of vertices in the clipped polygon
    if(clippedVertexCount < vertexCount) {
        return 0;  // triangle completely clipped away
    }

    // If exactly 3 vertices remain, output a single triangle
    if(clippedVertexCount == vertexCount) {
        for(uint32_t iVertex = 0; iVertex < vertexCount; iVertex++) {
            outputTriangles[0][iVertex] = clippedTriangle[iVertex];
        }
        return 1; // single triangle output
    }

    // If 4 vertices remain, create two triangles from them
    // First triangle: vertices 0,1,2
    outputTriangles[0][0] = clippedTriangle[0];
    outputTriangles[0][1] = clippedTriangle[1];
    outputTriangles[0][2] = clippedTriangle[2];

    // Second triangle: vertices 0,2,3
    outputTriangles[1][0] = clippedTriangle[0];
    outputTriangles[1][1] = clippedTriangle[2];
    outputTriangles[1][2] = clippedTriangle[3];

    return 2; // two triangles output
} // clippingSutherlandHodgman()

inline bool isVertexInsideClipPlane(const OutVertex &vertex) {
    // Inequality for near plane: -X(t)_w <= X(t)_z ~~> X(t)_z + X(t)_w >= 0
    return (vertex.gl_Position.z + vertex.gl_Position.w >= 0.f);
} // isVertexInsideClipPlane()

inline OutVertex calculateClipPlaneIntersection(const Program &program, const OutVertex &startVertex,
                                                const OutVertex &endVertex) {
    // Derivation of the parameter 't' (14 Ořez: Teorie ořezu):
    // -X(t)_w = X(t)_z
    // ...
    // t = (-Aw - Az) / (Bw - Aw + Bz - Az)
    const float Aw = startVertex.gl_Position.w;
    const float Az = startVertex.gl_Position.z;
    const float Bw = endVertex.gl_Position.w;
    const float Bz = endVertex.gl_Position.z;

    // Calculate interpolation factor 't' based on the distance to the clipping plane
    float t = (-Aw - Az) / (Bw - Aw + Bz - Az);

    // ensure interpolation factor 't' is in valid range <0.0, 1.0>
    t = glm::clamp(t, 0.f, 1.f);

    // Generate the new vertex at the intersection point
    return interpolateVertex(program, startVertex, endVertex, t);
}

inline OutVertex interpolateVertex(const Program &program, const OutVertex &startVertex,
                                   const OutVertex &endVertex, const float t) {
    OutVertex result; // create a new out vertex to store the result

    // Linearly interpolate homogeneous position coordinates (14 Ořez: Teorie ořezu):
    // X(t) = A + t * (B - A); for t in <0,1>
    result.gl_Position = startVertex.gl_Position + t * (endVertex.gl_Position - startVertex.gl_Position);

    // For other attributes we linearly combine the values from the vertices
    // of the line using the parameter t: X(t) = A + t * (B - A); for t in <0,1>
    for(uint32_t iAttribute = 0; iAttribute < maxAttribs; iAttribute++) {
        switch(program.vs2fs[iAttribute]) {
            // For scalar values, use simple linear interpolation
            case AttribType::FLOAT: {
                const float A = startVertex.attributes[iAttribute].v1;
                const float B = endVertex.attributes[iAttribute].v1;
                result.attributes[iAttribute].v1 = A + t * (B - A);
                break;
            }
            // For 'vec2' vectors, interpolate each component
            case AttribType::VEC2: {
                const glm::vec2 A = startVertex.attributes[iAttribute].v2;
                const glm::vec2 B = endVertex.attributes[iAttribute].v2;
                result.attributes[iAttribute].v2 = A + t * (B - A);
                break;
            }
            // For 'vec3' vectors, interpolate each component
            case AttribType::VEC3: {
                const glm::vec3 A = startVertex.attributes[iAttribute].v3;
                const glm::vec3 B = endVertex.attributes[iAttribute].v3;
                result.attributes[iAttribute].v3 = A + t * (B - A);
                break;
            }
            // For 'vec4' vectors, interpolate each component
            case AttribType::VEC4: {
                const glm::vec4 A = startVertex.attributes[iAttribute].v4;
                const glm::vec4 B = endVertex.attributes[iAttribute].v4;
                result.attributes[iAttribute].v4 = A + t * (B - A);
                break;
            }
            // Integer attributes use flat shading - take value from first vertex
            case AttribType::UINT:
            case AttribType::UVEC2:
            case AttribType::UVEC3:
            case AttribType::UVEC4:
            case AttribType::EMPTY:
            default: {
                result.attributes[iAttribute] = startVertex.attributes[iAttribute];
                break;
            }
        } // switch(program.vs2fs[attributeIndex])
    } // for(iAttribute)

    return result;
} // interpolateVertex()


/******************************************************************************/
/*                                                                            */
/*                          GENERAL HELPER FUNCTIONS                          */
/*                                                                            */
/******************************************************************************/

inline uint8_t *getPixelMaybeReversed(const Image &image, const uint32_t x, const uint32_t y,
                                      const uint32_t height, const bool yReversed) {
    // Choose the y-coordinate based on whether the image is y-reversed
    uint32_t yMmaybeReversed{y};

    if(yReversed) {
        yMmaybeReversed = height - y - 1;  // flip the y-coordinate and '-1' as we index from 0
    }

    // Calculate the pixel address as shown in '04 Čistící příkazy'
    return static_cast<uint8_t*>(image.data) + (yMmaybeReversed * image.pitch) + (x * image.bytesPerPixel);
} // getPixelMaybeReversed()

inline uint8_t castNormalizedFloatToUnsignedInt8(const float value) {
    return static_cast<uint8_t>(glm::clamp(value, 0.f, 1.f) * 255.f + 0.5f);
} // castNormalizedFloatToUnsignedInt8()

inline float castUnsignedInt8ToNormalizedFloat(const uint8_t value) {
    return static_cast<float>(value) / 255.0f;
} // castUnsignedInt8ToNormalizedFloat()

/*** end of file gpu.cpp ***/
