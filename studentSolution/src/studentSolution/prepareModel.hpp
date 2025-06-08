/*!
 * @file prepareModel.hpp
 * @brief This file contains function declarations for model rendering.
 *
 * @author Tomáš Milet, imilet@fit.vutbr.cz
 * @author Jan kalina, xkalinj00@stud.fit.vutbr.cz
 */
#pragma once

#include <solutionInterface/modelFwd.hpp>

/*
 * DISCLAIMER: This header file prototype documentation was co-created with the
 *             help of GitHub Copilot as the header file documentation is not
 *             part of the solution or a mandatory requirement. I only prepared
 *             this documentation to keep the beautiful assignment documentation
 *             consistent and so it contains my function if generated anew.
 *             Other parts of the code were written by me, Jan Kalina.
 */

/******************************************************************************/
/*                                                                            */
/*                PREPARE MODEL strongly based on pseudo-code                 */
/*                                                                            */
/******************************************************************************/

/**
 * @brief This function prepares model into memory and creates command buffer.
 *
 * @details This function initializes GPU memory with model data and prepares
 *          a command buffer for rendering. It copies buffers and textures to
 *          GPU memory, then recursively processes the model hierarchy starting
 *          from root nodes. For each node with a valid mesh, appropriate vertex
 *          arrays are created and rendering commands are added to the command
 *          buffer. Transformation matrices are calculated to properly position
 *          objects in the scene.
 *
 * @param mem GPU memory where resources and vertex arrays will be stored.
 * @param commandBuffer Buffer that will be filled with rendering commands.
 * @param model The 3D model containing hierarchical scene data with meshes and materials.
 */
void student_prepareModel(GPUMemory &mem, CommandBuffer &commandBuffer, const Model &model);

/**
 * @brief Recursively processes nodes in the model hierarchy.
 *
 * @details This helper function traverses the scene graph and processes each
 *          node with a mesh for rendering. It creates vertex array objects
 *          with appropriate attributes, adds commands to the command buffer
 *          for rendering, and prepares uniform data including transformation
 *          matrices. The function handles transformation inheritance through
 *          the hierarchy by combining parent and local transformation matrices.
 *          For each node with a valid mesh, it creates the necessary rendering
 *          setup and then recursively processes all child nodes.
 *
 * @param memory GPU memory where vertex arrays and uniforms will be stored.
 * @param commandBuffer Buffer where rendering commands will be stored.
 * @param model The 3D model containing meshes and materials.
 * @param node Current node being processed in the scene hierarchy.
 * @param parentMatrix Accumulated transformation matrix from parent nodes.
 * @param drawCounter Counter for draw commands used for uniform locations.
 * @param vertexArrayCounter Counter for vertex array objects in memory.
 */
void prepareNode(GPUMemory &memory, CommandBuffer &commandBuffer, const Model &model,
                 const Node &node, const glm::mat4 &parentMatrix, uint32_t &drawCounter,
                 uint32_t &vertexArrayCounter);


/******************************************************************************/
/*                                                                            */
/*       VERTEX SHADER strongly based on description in the assignment,       */
/*                   which I followed almost to the letter                    */
/*                                                                            */
/******************************************************************************/

/**
 * @brief This function represents vertex shader of texture rendering method.
 *
 * @details Transforms vertices from model space to world space using
 *          transformation matrices. Takes vertex attributes (position, normal,
 *          texture coordinates) in model space and outputs transformed
 *          attributes in world space along with light clip-space position for
 *          shadow mapping. The shader applies model matrix for positions,
 *          inverse transpose matrix for normals, and calculates final position
 *          in both camera and light clip spaces using respective projection-view
 *          matrices.
 *
 * @param outVertex Output vertex with transformed world-space position, normal,
 *                  texture coordinates, and light-space position.
 * @param inVertex Input vertex with model-space position, normal, and texture
 *                 coordinates.
 * @param si Shader interface with uniform variables including transformation
 *           matrices.
 */
void student_drawModel_vertexShader(OutVertex &outVertex, const InVertex &inVertex,
                                    const ShaderInterface &si);


/******************************************************************************/
/*                                                                            */
/*      FRAGMENT SHADER strongly based on description in the assignment,      */
/*                   which I followed almost to the letter                    */
/*                                                                            */
/******************************************************************************/

/**
 * @brief This functionrepresents fragment shader of texture rendering method.
 *
* @details Calculates fragment colors using Lambert illumination model with
*          shadow mapping. Processes world-space positions and normals, applies
*          texturing when available, handles double-sided surfaces, and computes
*          shadows by comparing fragment depth with shadow map values. The final
*          color combines ambient lighting with diffuse lighting affected by
*          shadows. Alpha transparency is preserved from the material color.
 *
 * @param outFragment Output fragment with calculated color and alpha.
 * @param inFragment Input fragment with world-space position, normal, texture
 *                   coordinates, and light-space position.
 * @param si Shader interface with uniform variables for lighting parameters
 *           and textures.
 */
void student_drawModel_fragmentShader(OutFragment &outFragment, const InFragment &inFragment,
                                      const ShaderInterface &si);

/*** end of file prepareModel.hpp ***/
