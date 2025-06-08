/*!
 * @file prepareModel.cpp
 * @brief This file contains functions for model rendering.
 *
 * @author Tomáš Milet, imilet@fit.vutbr.cz
 * @author Jan kalina, xkalinj00@stud.fit.vutbr.cz
 */

#include <studentSolution/prepareModel.hpp>
#include <studentSolution/gpu.hpp>
#include <studentSolution/shaderFunctions.hpp>
#include <solutionInterface/uniformLocations.hpp>

/*
 * When implementing this part of the project, I maximally based my code on the
 * assignment description, graphics, hints and pseudo-code provided. I also tried
 * to comment on my thoughts and decisions so it is evident that I understand
 * the problematics even though I used the pseudo-code as a base for my
 * implementation. I'd like to thank doctor Tomáš Milet for his great work
 * on the assignment and project skeleton. I only wish more assignments were
 * like this one ;-).
 */

/******************************************************************************/
/*                                                                            */
/*                    PROTOTYPES of used custom functions                     */
/*                                                                            */
/******************************************************************************/

#ifndef XKALINJ00_PREPARE_MODEL_SOLUTION
#define XKALINJ00_PREPARE_MODEL_SOLUTION

void prepareNode(GPUMemory &memory, CommandBuffer &commandBuffer, const Model &model,
                 const Node &node, const glm::mat4 &parentMatrix, uint32_t &drawCounter,
                 uint32_t &vertexArrayCounter);

#endif // XKALINJ00_PREPARE_MODEL_SOLUTION


/******************************************************************************/
/*                                                                            */
/*                PREPARE MODEL strongly based on pseudo-code                 */
/*                                                                            */
/******************************************************************************/

//! [drawModel]
void student_prepareModel(GPUMemory &mem, CommandBuffer &commandBuffer, const Model &model) {
    // Copy buffers: mem.buffers  = ...;
    for(size_t iBuffer = 0; iBuffer < model.nofBuffers; iBuffer++) {
        mem.buffers[iBuffer] = model.buffers[iBuffer];
    }

    // Copy textures: mem.textures = ...;
    for(size_t iTexture = 0; iTexture < model.nofTextures; iTexture++) {
        mem.textures[iTexture] = model.textures[iTexture];
    }

    uint32_t vertexArrayCounter{0};  // index into 'vertexArrays'
    uint32_t drawCounter{0};         // draw commands IDs

    // Recursively prepare nodes using pre-order traversal
    constexpr auto identityMatrix = glm::mat4(1.f);
    for(size_t iRoot = 0; iRoot < model.nofRoots; iRoot++) {
        prepareNode(mem, commandBuffer, model, model.roots[iRoot],
                    identityMatrix, drawCounter, vertexArrayCounter);
    }
} // student_prepareModel()
//! [drawModel]

inline void prepareNode(GPUMemory &memory, CommandBuffer &commandBuffer, const Model &model,
                        const Node &node, const glm::mat4 &parentMatrix, uint32_t &drawCounter,
                        uint32_t &vertexArrayCounter) {
    // Calculate the model matrix by combining parent and local transformations
    const glm::mat4 modelMatrix = parentMatrix * node.modelMatrix;

    // Process the node only if it has a valid mesh
    if(node.mesh >= 0) {
        // Get mesh data from the model
        const Mesh &mesh = model.meshes[node.mesh];

        // Create and initialize vertex array
        VertexArray vertexArray{};
        vertexArray.indexBufferID = mesh.indexBufferID;
        vertexArray.indexOffset = mesh.indexOffset;
        vertexArray.indexType = mesh.indexType;
        vertexArray.vertexAttrib[0] = mesh.position;
        vertexArray.vertexAttrib[1] = mesh.normal;
        vertexArray.vertexAttrib[2] = mesh.texCoord;

        // Save the new vertex array in the GPU memory
        memory.vertexArrays[vertexArrayCounter] = vertexArray;

        // Bind the vertex array for the current draw call
        pushBindVertexArrayCommand(commandBuffer, vertexArrayCounter);

        // Configure backface culling (disable for double-sided materials)
        pushSetBackfaceCullingCommand(commandBuffer, !mesh.doubleSided);

        // Create draw command with the number of indices to render
        pushDrawCommand(commandBuffer, mesh.nofIndices);

        // Write uniform data to memory
        memory.uniforms[getUniformLocation(drawCounter, MODEL_MATRIX)].m4 = modelMatrix;
        memory.uniforms[getUniformLocation(drawCounter, INVERSE_TRANSPOSE_MODEL_MATRIX)].m4 = glm::transpose(glm::inverse(modelMatrix));
        memory.uniforms[getUniformLocation(drawCounter, DIFFUSE_COLOR)].v4 = mesh.diffuseColor;
        memory.uniforms[getUniformLocation(drawCounter, TEXTURE_ID)].i1 = mesh.diffuseTexture;
        memory.uniforms[getUniformLocation(drawCounter, DOUBLE_SIDED)].v1 = mesh.doubleSided ? 1.f : 0.f;

        // Increment counters for the next draw call
        vertexArrayCounter++;
        drawCounter++;
    } // if(node.mesh >= 0)

    // Recursively prepare nodes using pre-order traversal
    for(size_t iChild = 0; iChild < node.nofChildren; iChild++) {
        prepareNode(memory, commandBuffer, model, node.children[iChild],
                    modelMatrix, drawCounter, vertexArrayCounter);
    } // for(iChild)
} // prepareNode()


/******************************************************************************/
/*                                                                            */
/*       VERTEX SHADER strongly based on description in the assignment,       */
/*                   which I followed almost to the letter                    */
/*                                                                            */
/******************************************************************************/

//! [drawModel_vs]
void student_drawModel_vertexShader(OutVertex &outVertex, const InVertex &inVertex, const ShaderInterface &si) {
    // The input is vertices that have position (3f), normal (3f), and
    // texturing coordinates (2f) (attributes 0, 1, and 2)
    const glm::vec3 modelSpaceVertexPosition = inVertex.attributes[0].v3;  // vertex position in model-space
    const glm::vec3 modelSpaceVertexNormal = inVertex.attributes[1].v3;    // vertex normal in model-space
    const glm::vec2 texturingCoordinates = inVertex.attributes[2].v2;      // texture coordinates

    // Uniform variables include the projectionView matrix, the model matrix, and the inverse transpose matrix
    const glm::mat4 cameraProjectionViewMatrix = si.uniforms[getUniformLocation(si.gl_DrawID, PROJECTION_VIEW_MATRIX)].m4;           // cameraProjectionView camera projection and view matrix
    const glm::mat4 lightProjectionViewMatrix = si.uniforms[getUniformLocation(si.gl_DrawID, USE_SHADOW_MAP_MATRIX)].m4;             // lightProjectionView light projection and view matrix - for shadows
    const glm::mat4 modelMatrix = si.uniforms[getUniformLocation(si.gl_DrawID, MODEL_MATRIX)].m4;                                    // model matrix
    const glm::mat4 inverseTransposeModelMatrix = si.uniforms[getUniformLocation(si.gl_DrawID, INVERSE_TRANSPOSE_MODEL_MATRIX)].m4;  // inverse transpose matrix

    // Transform vertex and normal to world space "m*glm::vec4(pos,1.f)"
    const auto worldSpaceVertexPosition = glm::vec3(modelMatrix * glm::vec4(modelSpaceVertexPosition, 1.f));

    // Transform normal to world space "itm*glm::vec4(nor,0.f)"
    const auto worldSpaceVertexNormal = glm::vec3(inverseTransposeModelMatrix * glm::vec4(modelSpaceVertexNormal, 0.f));

    // The output is vertices that have a position (3f) and normal (3f) in world space,
    // texturing coordinates (2f) and vertex position in light clip-space (4f) (attributes 0, 1, 2, 3).
    outVertex.attributes[0].v3 = worldSpaceVertexPosition;  // vertex position in world-space
    outVertex.attributes[1].v3 = worldSpaceVertexNormal;    // vertex normal in world-space
    outVertex.attributes[2].v2 = texturingCoordinates;      // texture coordinates
    outVertex.attributes[3].v4 = lightProjectionViewMatrix * glm::vec4(worldSpaceVertexPosition, 1.f);  // vertex position in clip-space of the light

    // Transform position to camera clip space "cameraProjectionView*model*pos" (worldSpaceVertexPosition = model * pos)
    outVertex.gl_Position = cameraProjectionViewMatrix * glm::vec4(worldSpaceVertexPosition, 1.f);
} // student_drawModel_vertexShader()
//! [drawModel_vs]


/******************************************************************************/
/*                                                                            */
/*      FRAGMENT SHADER strongly based on description in the assignment,      */
/*                   which I followed almost to the letter                    */
/*                                                                            */
/******************************************************************************/

//! [drawModel_fs]
void student_drawModel_fragmentShader(OutFragment &outFragment, const InFragment &inFragment, const ShaderInterface &si) {
    // The input is fragments that have: position (3f), normal (3f), texturing coordinates (2f) and
    // position in clip-space of the light space for reading from the shadow map (attributes 0,1,2,3)
    const glm::vec3 worldSpaceFragmentPosition = inFragment.attributes[0].v3;  // fragment position in world-space
    glm::vec3 worldSpaceFragmentNormal = inFragment.attributes[1].v3;          // fragment normal in world-space
    const glm::vec2 texturingCoordinates = inFragment.attributes[2].v2;        // texture coordinates
    glm::vec4 clipSpaceShadowPosition = inFragment.attributes[3].v4;           /* fragment position in clip-space of light for
                                                                                  shadow map addressing and shadow calculation */

    // Uniform variables include light position (3f), camera position (3f),
    // diffuse color (4f), texture number (1i), and doubleSided flag (1f)
    const glm::vec3 lightPosition = si.uniforms[getUniformLocation(si.gl_DrawID, LIGHT_POSITION)].v3;          // light position in world-space
    const glm::vec3 cameraPosition = si.uniforms[getUniformLocation(si.gl_DrawID, CAMERA_POSITION)].v3;        // camera position in world-space
    const int32_t shadowMapId = si.uniforms[getUniformLocation(si.gl_DrawID, SHADOWMAP_ID)].i1;                // texture number that contains the shadow map, or -1 if there are no shadows
    const glm::vec3 ambientLightColor = si.uniforms[getUniformLocation(si.gl_DrawID, AMBIENT_LIGHT_COLOR)].v3; // ambient light color
    const glm::vec3 lightColor = si.uniforms[getUniformLocation(si.gl_DrawID, LIGHT_COLOR)].v3;                // light color
    glm::vec4 diffuseColor = si.uniforms[getUniformLocation(si.gl_DrawID, DIFFUSE_COLOR)].v4;                  // diffuse color
    const int32_t textureId = si.uniforms[getUniformLocation(si.gl_DrawID, TEXTURE_ID)].i1;                    // texture number or -1 if there are no textures
    const float isDoubleSided = si.uniforms[getUniformLocation(si.gl_DrawID, DOUBLE_SIDED)].v1;                // doubleSided flag (1.f if it is, 0.f if it is not)

    // Input normal should be normalized with N=glm::normalize(nor)
    worldSpaceFragmentNormal = glm::normalize(worldSpaceFragmentNormal);

    // The diffuse color of the material is either stored in a uniform variable or in a texture
    // It depends on whether the texture number is negative or not
    if(textureId >= 0) {
        diffuseColor = student_read_textureClamp(si.textures[textureId], texturingCoordinates);
    } // if(diffuse color is stored in a texture)

    // If the doubleSided flag is set (is > 0), it is a double-sided surface.
    // In this case, it is necessary to rotate the normal if it is facing away
    // from the camera (use the camera position in the uniform variable)
    if(isDoubleSided > 0.f) {
        const glm::vec3 viewDirection = glm::normalize(cameraPosition - worldSpaceFragmentPosition);
        const float normalDirection = glm::dot(viewDirection, worldSpaceFragmentNormal);

        // Flip the normal if it is facing away
        if(normalDirection < 0.f) {
            worldSpaceFragmentNormal = glm::normalize(-worldSpaceFragmentNormal);
        }
    } // if(isDoubleSided)

    // Calculate a Lambert lighting model with shadows using shadow mapping
    // Note: From IZG lecture presentation: ID = IL * rD * cos(phi), ID = IL * rD * (vec(N) * vec(L)),
    //       where ID = diffuse component, IL = light intensity, rD = diffusion coefficient of the surface,
    //       vec(N) = surface normal, vec(L) = vector from the fragment to the light source
    const glm::vec3 lightDirection = glm::normalize(lightPosition - worldSpaceFragmentPosition);  // normalized vector from the fragment to the light

    // Scalar product of the normal and the light direction (vec(N) * vec(L))
    const float scalarProductNL = glm::clamp(glm::dot(worldSpaceFragmentNormal, lightDirection), 0.f, 1.f);  // surfaces facing away from the light are not illuminated

    // Ambient component according to the formula IA = Ia * rD
    const glm::vec3 ambientComponent = ambientLightColor * glm::vec3(diffuseColor);

    // Diffusion component according to the formula ID = IL * rD * (vec(N) * vec(L))
    const glm::vec3 diffuseComponent = lightColor * glm::vec3(diffuseColor) * scalarProductNL;

    // The resulting color is the sum of the ambient and diffuse components
    glm::vec3 color = ambientComponent + diffuseComponent;

    // Check if alpha is less than 0.5 and set discard flag if needed
    if(diffuseColor.a < 0.5f) {
        outFragment.discard = true;
        return;  // Discarding so color does not matter
    }

    // Calculate whether the fragment is in shadow
    // You need to read the depth from the shadow map and compare it to the distance to the light
    if(shadowMapId >= 0) {
        // Perspective division - convert from clip-space to NDC
        const glm::vec3 shadowPosition = glm::vec3(clipSpaceShadowPosition) / clipSpaceShadowPosition.w;

        // Check if fragment is outside shadow map bounds (not in shadow)
        if(shadowPosition.x >= 0.0f && shadowPosition.x <= 1.0f &&
            shadowPosition.y >= 0.0f && shadowPosition.y <= 1.0f) {
            // Read depth from shadow map
            auto depthFromShadowMap = student_read_textureClamp(si.textures[shadowMapId], glm::vec2(shadowPosition)).r;


            // Is the fragment depth greater than what is in the shadow map?
            if(shadowPosition.z > depthFromShadowMap) {
                color = ambientComponent;
            }
        } // if(shadowPosition is in shadow map bounds)
    } // if(shadowing)

    // The output is a fragment with color and the correct transparency 'alpha'
    outFragment.gl_FragColor = glm::vec4(glm::clamp(color, 0.f, 1.f), diffuseColor.a);
} // student_drawModel_fragmentShader()
//! [drawModel_fs]

/*** end of file prepareModel.cpp ***/
