/*!
 * @file
 * @brief This file contains forward declarations, structures and constants.
 *
 * @author Tomáš Milet, imilet@fit.vutbr.cz
 */

#pragma once

#include <glm/glm.hpp>
#include <cstdint>

//#define MAKE_STUDENT_RELEASE

uint32_t const maxAttribs = 4;///< maximum number of vertex/fragment attributes

/**
 * @brief This structure represents 2D memory of image.
 * 2D images are used as textures and color or depth buffers of framebuffers.
 * Pixel [x,y] starts at uint8_t*ptr = ((uint8_t*)data) + y*pitch + x*bytesPerPixel;
 * channelTypes is swizzling table for colors (RGB,BGR,...)
 * for format == F32:
 *   bytesPerPixel = sizeof(float) * channels
 * for format == U8:
 *   bytesPerPixel = sizeof(uint8_t) * channels
 *
 * pitch = width * bytesPerPixel
 */
//! [Image]
struct Image{
  /**
   * @brief Channel type
   */
  enum Channel{
    RED   = 0,
    GREEN    ,
    BLUE     ,
    ALPHA    ,
  };
  /**
   * @brief Format of data
   */
  enum Format{
    U8 ,
    F32,
  };
  void*    data            = nullptr               ; ///< pointer to image data
  uint32_t channels        = 4                     ; ///< number of color channels
  Format   format          = U8                    ; ///< type of data for every channel
  uint32_t pitch           = 0                     ; ///< size of row in bytes 
  uint32_t bytesPerPixel   = 0                     ; ///< size of pixel in bytes
  Channel  channelTypes[4] = {RED,GREEN,BLUE,ALPHA}; ///< only for colors - what colors the channels represent
};
//! [Image]

/**
 * @brief This struct represent a texture
 */
//! [Texture]
struct Texture{
  uint32_t width  = 0; ///< width of the texture
  uint32_t height = 0; ///< height of the texture
  Image    img       ; ///< image
};
//! [Texture]


/**
 * @brief This enum represents vertex/fragment attribute type.
 */
//! [AttribType]
enum class AttribType : uint8_t{
  EMPTY =   0, ///< disabled attribute
  FLOAT =   1, ///< 1x 32-bit float
  VEC2  =   2, ///< 2x 32-bit floats
  VEC3  =   3, ///< 3x 32-bit floats
  VEC4  =   4, ///< 4x 32-bit floats
  UINT  = 8+1, ///< 1x 32-bit unsigned int
  UVEC2 = 8+2, ///< 2x 32-bit unsigned int
  UVEC3 = 8+3, ///< 3x 32-bit unsigned int
  UVEC4 = 8+4, ///< 4x 32-bit unsigned int
};
//! [AttribType]

/**
 * @brief This union represents one vertex/fragment attribute
 */
//! [Attrib]
union Attrib{
  Attrib():v4(glm::vec4(1.f)){}
  float      v1; ///< single float
  glm::vec2  v2; ///< vector of two floats
  glm::vec3  v3; ///< vector of three floats
  glm::vec4  v4; ///< vector of four floats
  uint32_t   u1; ///< single unsigned int
  glm::uvec2 u2; ///< vector of two unsigned ints
  glm::uvec3 u3; ///< vector of three unsigned ints
  glm::uvec4 u4; ///< vector of four unsigned ints
};
//! [Attrib]

/**
 * @brief This struct represents input vertex of vertex shader.
 */
//! [InVertex]
struct InVertex{
  Attrib   attributes[maxAttribs]    ; ///< vertex attributes
  uint32_t gl_VertexID            = 0; ///< vertex id
};
//! [InVertex]

/**
 * @brief This struct represents output vertex of vertex shader.
 */
//! [OutVertex]
struct OutVertex{
  Attrib    attributes[maxAttribs]                     ; ///< vertex attributes
  glm::vec4 gl_Position            = glm::vec4(0,0,0,1); ///< clip space position
};
//! [OutVertex]

/**
 * @brief This struct represents input fragment.
 */
//! [InFragment]
struct InFragment{
  Attrib    attributes[maxAttribs]               ; ///< fragment attributes
  glm::vec4 gl_FragCoord           = glm::vec4(1); ///< fragment coordinates
};
//! [InFragment]

/**
 * @brief This struct represents output fragment.
 */
//! [OutFragment]
struct OutFragment{
  glm::vec4 gl_FragColor = glm::vec4(0.f); ///< fragment color
  bool      discard      = false         ; ///< discard fragment
};
//! [OutFragment]

/**
 * @brief This union represents one uniform variable.
 */
//! [Uniform]
union Uniform{
  Uniform(){}
  float      v1; ///< single float
  glm::vec2  v2; ///< two    floats
  glm::vec3  v3; ///< three  floats
  glm::vec4  v4; ///< four   floats
  uint32_t   u1; ///< single 32bit unsigned int
  glm::uvec2 u2; ///< two    32bit unsigned ints
  glm::uvec3 u3; ///< three  32bit unsigned ints
  glm::uvec4 u4; ///< four   32bit unsigned ints
  int32_t    i1; ///< single 32bit int
  glm::ivec2 i2; ///< two    32bit ints
  glm::ivec3 i3; ///< three  32bit ints
  glm::ivec4 i4; ///< four   32bit ints
  glm::mat4  m4 = glm::mat4(1.f); ///< 4x4 float matrix
};
//! [Uniform]

/**
 * @brief This enum represents index type
 */
//! [IndexType]
enum class IndexType : uint8_t{
  U8  = 1, ///< uin8_t type
  U16 = 2, ///< uin16_t type
  U32 = 4, ///< uint32_t type
};
//! [IndexType]


/**
 * @brief This enum represents constant shader interface common for all shaders.
 *
 * @param outVertex output vertex
 * @param inVertex input vertex
 * @param uniforms uniform variables
 */
//! [ShaderInterface]
struct ShaderInterface{
  Uniform const*uniforms  = nullptr; ///< uniform variables
  Texture const*textures  = nullptr; ///< textures
  uint32_t      gl_DrawID = 0      ; ///< draw id
};
//! [ShaderInterface]

/**
 * @brief Function type for vertex shader
 *
 * @param outVertex output vertex
 * @param inVertex input vertex
 * @param uniforms uniform variables
 */
//! [VertexShader]
using VertexShader = void(*)(
    OutVertex            &outVertex,
    InVertex        const&inVertex ,
    ShaderInterface const&si       );
//! [VertexShader]

/**
 * @brief Function type for fragment shader
 *
 * @param outFragment output fragment
 * @param inFragment input fragment 
 * @param uniforms uniform variables
 */
//! [FragmentShader]
using FragmentShader = void(*)(
    OutFragment          &outFragment,
    InFragment      const&inFragment ,
    ShaderInterface const&si         );
//! [FragmentShader]

/**
 * @brief This struct describes location of one vertex attribute.
 */
//! [VertexAttrib]
struct VertexAttrib{
  int32_t       bufferID = -1               ;///< buffer id
  uint64_t      stride   = 0                ;///< stride in bytes
  uint64_t      offset   = 0                ;///< offset in bytes
  AttribType    type     = AttribType::EMPTY;///< type of attribute
};
//! [VertexAttrib]

/**
 * @brief This structure represents setting for vertex pulller (vertex assembly) unit.
 * VertexArrays holds setting for reading vertices from buffers.
 */
//! [VertexArray]
struct VertexArray{
  VertexAttrib vertexAttrib[maxAttribs];    ///< settings for vertex attributes
  int32_t      indexBufferID = -1;             ///< id of index buffer
  uint64_t     indexOffset   = 0 ;             ///< offset of indices
  IndexType    indexType     = IndexType::U32; ///< type of indices
};
//! [VertexArray]

/**
 * @brief This structu represents a program.
 * Vertex Shader is executed on every InVertex.
 * Fragment Shader is executed on every rasterized InFragment.
 */
//! [Program]
struct Program{
  VertexShader   vertexShader   = nullptr; ///< vertex shader
  FragmentShader fragmentShader = nullptr; ///< fragment shader
  AttribType  vs2fs[maxAttribs] = {AttribType::EMPTY}; ///< which attributes are interpolated from vertex shader to fragment shader
};
//! [Program]


/**
 * @brief This function gets pointer to the start of the pixel in the image
 *
 * @param image image
 * @param x x coordinates
 * @param y y coordinates
 *
 * @return pointer to the start of the pixel
 */
inline void*getPixel(Image&image,uint32_t x,uint32_t y){
  auto ptr = (uint8_t*)image.data;
  ptr += image.pitch*y + image.bytesPerPixel*x;
  return ptr;
}

/**
 * @brief This function gets constant pointer to the start of the pixel in the image
 *
 * @param image image
 * @param x x coordinates
 * @param y y coordinates
 *
 * @return constant pointer to the start of the pixel
 */
inline void const*getPixel(Image const&image,uint32_t x,uint32_t y){
  auto ptr = (uint8_t*)image.data;
  ptr += image.pitch*y + image.bytesPerPixel*x;
  return ptr;
}

/**
 * @brief This structure represent a framebuffer
 * A framebuffer is used as an output of rendering
 */
//! [Framebuffer]
struct Framebuffer{
  uint32_t width     = 0    ; ///< width of frame buffer
  uint32_t height    = 0    ; ///< height of frame buffer
  bool     yReversed = false; ///< is y axis flipped?
  Image    color            ; ///< color buffer
  Image    depth            ; ///< depth buffer
  Image    stencil          ; ///< stencil buffer
};
//! [Framebuffer]

/**
 * @brief This structure represents a buffer on GPU
 * Buffer is a linear memory.
 */
//! [Buffer]
struct Buffer{
  void const* data = nullptr; ///< pointer to data
  uint64_t    size = 0      ; ///< size of data in bytes
};
//! [Buffer]

/**
 * @brief This enum represents stencil function for stencil test.
 * Stencil test test every pixel in framebuffer againts reference value.
 */
//! [StencilFunc]
enum class StencilFunc{
  NEVER   , ///< Stencil test will never pass
  LESS    , ///< Stencil test will pass if value in stencil buffer is <  than reference value
  LEQUAL  , ///< Stencil tess will pass if value in stencil buffer is <= than reference value
  GREATER , ///< Stencil test will pass if value in stencil buffer is >  than reference value
  GEQUAL  , ///< Stencil test will pass if value in stencil buffer is >= than reference value
  EQUAL   , ///< Stencil test will pass if value in stencil buffer is == to   reference value
  NOTEQUAL, ///< Stencil test will pass if value in stencil buffer is != to   reference value
  ALWAYS  , ///< Stencil test will always pass
};
//! [StencilFunc]

/**
 * @brief This enum represents stencil operation.
 * Stencil operatation changes stencil buffer in three occasions:
 * 1.) stencil test fails
 * 2.) depth test fails
 * 3.) depth test passes
 */
//! [StencilOp]
enum class StencilOp{
   KEEP     , ///< No change to the stencil buffer
   ZERO     , ///< Stencil buffer value will be zeroed
   REPLACE  , ///< Stencil buffer value will be replace by reference value
   INCR     , ///< Stencil buffer value will be incremented with saturation (will not overflow)
   INCR_WRAP, ///< Stencil buffer value will be incremented and return to zero on overflow
   DECR     , ///< Stencil buffer value will be decremented with saturation (will not underflow)
   DECR_WRAP, ///< Stencil buffer value will be decremented and go to 255 on underflow 
   INVERT   , ///< Stencil buffer value will be inteverted
};
//! [StencilOp]

/**
 * @brief This structure represents sfail, dpfail and dppass stencil operations.
 */
//! [StencilOps]
struct StencilOps{
  StencilOp sfail  = StencilOp::KEEP; ///< operation performed when fragment of backfacing triangle fails stencil test
  StencilOp dpfail = StencilOp::KEEP; ///< operation performed when fragment of backfacing triangle fails depth test
  StencilOp dppass = StencilOp::KEEP; ///< stencil operation performed when depth test passes
};
//! [StencilOps]

/**
 * @brief Stencil settings
 */
//! [StencilSettings]
struct StencilSettings{
  bool        enabled  = false              ; ///< is stencil test enabled?
  StencilFunc func     = StencilFunc::ALWAYS; ///< comparison function
  uint32_t    refValue = 0                  ; ///< reference value to which a stencil buffer value is compared
  StencilOps  frontOps                      ; ///< operations performen when a front-facing fragment is processed
  StencilOps  backOps                       ; ///< operations performed when a back-facing fragment is processed
};
//! [StencilSettings]

/**
 * @brief Block writes to framebuffer
 */
//! [BlockWrites]
struct BlockWrites{
  bool color   = false; ///< if set to true, writes to color   buffer are blocked
  bool depth   = false; ///< if set to true, writes to depth   buffer are blocked
  bool stencil = false; ///< if set to true, writes to stencil buffer are blocked
};
//! [BlockWrites]

/**
 * @brief This structure represents settings for backface culling.
 */
//! [BackfaceCulling]
struct BackfaceCulling{
  bool enabled                     = false; ///< is backface culling enabled?
  bool frontFaceIsCounterClockWise = true ; ///< is frontface made of vertices specified in counter clock wise order?
};
//! [BackfaceCulling]

/**
 * @brief This structure represents memory on GPU
 * A GPU memory has a lot of memory types ranging from buffers, textures,
 * to uniforms programs, vertex arrays and framebuffers.
 */
//! [GPUMemory]
struct GPUMemory{
  uint32_t         maxUniforms          = 0      ; ///< maximal number of uniforms
  uint32_t         maxVertexArrays      = 0      ; ///< maximal number of vertex arrays
  uint32_t         maxTextures          = 0      ; ///< maximal number of textures
  uint32_t         maxBuffers           = 0      ; ///< maximal number of buffers
  uint32_t         maxPrograms          = 0      ; ///< maximal number of programs
  uint32_t         maxFramebuffers      = 0      ; ///< maximal number of framebuffers
  uint32_t         defaultFramebuffer   = 0      ; ///< id of default framebuffer
  Buffer          *buffers              = nullptr; ///< array of all buffers
  Texture         *textures             = nullptr; ///< array of all textures
  Uniform         *uniforms             = nullptr; ///< array of all uniform variables
  Program         *programs             = nullptr; ///< array of all programs
  Framebuffer     *framebuffers         = nullptr; ///< framebuffer - output of rendering, 0 is default framebuffer
  VertexArray     *vertexArrays         = nullptr; ///< array of all vertex arrays
  uint32_t         activatedFramebuffer = 0      ; ///< id of activated framebuffer
  uint32_t         activatedProgram     = 0      ; ///< id of activated program
  uint32_t         activatedVertexArray = 0      ; ///< id of activated vertex array
  uint32_t         gl_DrawID            = 0      ; ///< draw call id
  StencilSettings  stencilSettings               ; ///< stencil test settings
  BlockWrites      blockWrites                   ; ///< block writes to buffers of framebuffer
  BackfaceCulling  backfaceCulling               ; ///< backface culling

  //Do not worry about these.
  //This is just to suppress valgrind warnings because of the large stack.
  //Otherwise everything would be placed on the stack and not on the heap.
  //I had to allocated this structure on the heap, because it is too large.
  GPUMemory();                 
  GPUMemory(GPUMemory const&o);
  ~GPUMemory();                
  GPUMemory&operator=(GPUMemory const&o);
};
//! [GPUMemory]


struct CommandBuffer;

/**
 * @brief This structure represents clear color buffer command.
 * The structure stores clear color value to which the color buffer should be cleared.
 */
//! [ClearColorCommand]
struct ClearColorCommand{
  glm::vec4 value = glm::vec4(0); ///< color buffer will be cleared by this value
};
//! [ClearColorCommand]

/**
 * @brief This structure represents clear depth buffer command.
 * The structure stores clear depth value to which the depth buffer should be cleared.
 */
//! [ClearDepthCommand]
struct ClearDepthCommand{
  float value = 1e10; ///< depth buffer will be cleared by this value
};
//! [ClearDepthCommand]

/**
 * @brief This structure represents clear depth buffer command.
 * The structure stores clear stencil value to which the depth buffer should be cleared.
 */
//! [ClearStencilCommand]
struct ClearStencilCommand{
  uint8_t value = 0u; ///< stencil buffer will be cleared by this value
};
//! [ClearStencilCommand]

/**
 * @brief This structure represents draw command.
 * Draw command issues draw operation on the GPU.
 */
//! [DrawCommand]
struct DrawCommand{
  uint32_t nofVertices     = 0    ; ///< number of vertices to draw
};
//! [DrawCommand]

/**
 * @brief This structure represents setDrawId command.
 * SetDrawId command sets gl_DrawID during command buffer execution.
 */
//! [SetDrawIdCommand]
struct SetDrawIdCommand{
  uint32_t id = 0; ///< id of draw
};
//! [SetDrawIdCommand]

/**
 * @brief This structure represents bindFramebuffer command.
 * BindFramebuffer command binds framebuffer to which the rendering result is stored.
 */
//! [BindFramebufferCommand]
struct BindFramebufferCommand{
  uint32_t id = 0; ///< id of framebuffer to bind
};
//! [BindFramebufferCommand]

/**
 * @brief This structure represents bindProgram command.
 * bindProgram commands selects shader program that should be used during rendering.
 */
//! [BindProgramCommand]
struct BindProgramCommand{
  uint32_t id = 0; ///< selected shader program - id
};
//! [BindProgramCommand]

/**
 * @brief This structure represents bindVertexArray command.
 * bindVertexArray commands selects vertex array table that is used by Vertex Assembly unit.
 */
//! [BindVertexArrayCommand]
struct BindVertexArrayCommand{
  uint32_t id = 0; ///< selected vertex array - id
};
//! [BindVertexArrayCommand]

/**
 * @brief This structure represents sub command.
 * A sub command is secondary command buffer that should be executed.
 */
//! [SubCommand]
struct SubCommand{
  CommandBuffer*commandBuffer = nullptr; ///< pointer to secondary command buffer
};
//! [SubCommand]

/**
 * @brief This structure represents set stencil command.
 * A set stencil command sets all stencil operations.
 */
//! [SetStencilCommand]
struct SetStencilCommand{
  StencilSettings settings; ///< stencil settings to be set
};
//! [SetStencilCommand]

/**
 * @brief This structure represents block writes command.
 * A block write command sets a blocking mask for writes to color, depth and stencil buffer.
 * When a GPU rasterizes a triangle, it produces fragments.
 * These fragments can modify color buffer, depth buffer and stencil buffer.
 * If a blocking mask is set to true, this modifications are disabled.
 * Student should implement correct behaviour of this command.
 */
//! [BlockWritesCommand]
struct BlockWritesCommand{
  BlockWrites blockWrites; ///< which writes are blocked
};
//! [BlockWritesCommand]

/**
 * @brief Set Backface Culling Command can enable/disable backface culling.
 * This command can enable/disable backface culling.
 * Also, it can specify, what is backfacing face of a triangle.
 * Backface culling disable rasterization of triangles that are backfacing a camera.
 * The backfacingness is determined by the order of vertices - clockwise or counter clockwise.
 * Student should implement correct behaviour of this command.
 */
//! [SetBackfaceCullingCommand]
struct SetBackfaceCullingCommand{
  bool enabled = false; ///< is backface culling enabled?
};
//! [SetBackfaceCullingCommand]

/**
 * @brief This command specfies what is front facing face
 * A face can be either front facing or back facing.
 * Verticies of a triangle can be specified in clock wise order or counter clock wise order.
 */
//! [SetFrontFaceCommand]
struct SetFrontFaceCommand{
  bool frontFaceIsCounterClockWise = true; ///< which vertex order specifies front face
};
//! [SetFrontFaceCommand]


/**
 * @brief User function pointer type
 *
 * @param data user data
 */
using UserCommandFce = void(*)(void*data);

/**
 * @brief This structure represents user command.
 * User command can execute any task. It is usually not present on GPU.
 * The closest GPU command would be compute shader.
 * When the GPU encounter this command, it will execute user callback and give the
 * command userData.
 * If the callback is nullptr, the GPU ignores it.
 * Student should implement correct behaviour of this command.
 */
//! [UserCommand]
struct UserCommand{
  UserCommandFce callback = nullptr; ///< user function
  void*          data     = nullptr; ///< user data for user function
};
//! [UserCommand]

/**
 * @brief This enum represents type of command.
 * A GPU accept commands and execute them.
 * Each command has a type.
 * A virtual GPU in this project should be able to execute several command types.
 */
//! [CommandType]
enum class CommandType{
  EMPTY                       , ///< empty command
  BIND_FRAMEBUFFER            , ///< bind framebuffer command
  BIND_PROGRAM                , ///< bind program command
  BIND_VERTEXARRAY            , ///< bind vertex array command
  BLOCK_WRITES_COMMAND        , ///< block writes commnads
  SET_BACKFACE_CULLING_COMMAND, ///< set backface culling command
  SET_FRONT_FACE_COMMAND      , ///< command setting up, what is front face
  SET_STENCIL_COMMAND         , ///< set stencil command
  SET_DRAW_ID                 , ///< set drawId command
  USER_COMMAND                , ///< user command
  CLEAR_COLOR                 , ///< clear color   buffer command
  CLEAR_DEPTH                 , ///< clear depth   buffer command
  CLEAR_STENCIL               , ///< clear stencil buffer command                             
  DRAW                        , ///< draw command
  SUB_COMMAND                 , ///< sub command
};
//! [CommandType]

/**
 * @brief This union represents command data.
 * One instance of this union represents one command data.
 * Data of each command type is aliased.
 * Only one command data is valid according to selected command type.
 */
//! [CommandData]
union CommandData{
  CommandData():drawCommand(){}///< constructor
  BindFramebufferCommand    bindFramebufferCommand   ;///< bind framebuffer command data
  BindProgramCommand        bindProgramCommand       ;///< bind program command data
  BindVertexArrayCommand    bindVertexArrayCommand   ;///< bind vertex array command data
  BlockWritesCommand        blockWritesCommand       ;///< block writes command data
  SetBackfaceCullingCommand setBackfaceCullingCommand;///< set backface culling command data
  SetFrontFaceCommand       setFrontFaceCommand      ;///< set front face command
  SetStencilCommand         setStencilCommand        ;///< command for setting up stencil data
  SetDrawIdCommand          setDrawIdCommand         ;///< set drawId command data
  UserCommand               userCommand              ;///< user command data
  ClearColorCommand         clearColorCommand        ;///< clear color buffer command data
  ClearDepthCommand         clearDepthCommand        ;///< clear depth buffer command data
  ClearStencilCommand       clearStencilCommand      ;///< clear stencil buffer command data
  DrawCommand               drawCommand              ;///< draw command data
  SubCommand                subCommand               ;///< sub command buffer command data
};
//! [CommandData]

/**
 * @brief This struct represents a command.
 * A GPU executes commands.
 * Each command has a type and data.
 */
//! [Command]
struct Command{
  CommandData data                     ;///< data of the command
  CommandType type = CommandType::EMPTY;///< type of the command
};
//! [Command]

/**
 * @brief This struct represents a command buffer.
 * Command buffer is used for CPU -> GPU communication.
 * Jobs are sent to GPU using this structure.
 */
//! [CommandBuffer]
struct CommandBuffer{
  uint32_t static const maxCommands           = 10000; ///< maximal number of commands
  uint32_t              nofCommands           = 0    ; ///< number of used commands in command buffer
  Command               commands[maxCommands]        ; ///< array of commands
};
//! [CommandBuffer]

/**
 * @brief This function can be used to insert clear color command to command buffer.
 *
 * @param cb    command buffer
 * @param value clear color value for cleaning
 */
inline void pushClearColorCommand(
    CommandBuffer      &cb                  ,
    glm::vec4     const&value = glm::vec4(0)){
  auto&cmd=cb.commands[cb.nofCommands];
  cmd.type = CommandType::CLEAR_COLOR;
  auto&c = cmd.data.clearColorCommand;
  c.value        = value     ;
  cb.nofCommands++;
}

/**
 * @brief This function can be used to insert clear depth command to command buffer.
 *
 * @param cb    command buffer
 * @param value clear depth value for cleaning
 */
inline void pushClearDepthCommand(
    CommandBuffer &cb           ,
    float          value = 2){
  auto&cmd=cb.commands[cb.nofCommands];
  cmd.type = CommandType::CLEAR_DEPTH;
  auto&c = cmd.data.clearDepthCommand;
  c.value = value;
  cb.nofCommands++;
}

/**
 * @brief This function can be used to insert clear stencil command to command buffer.
 *
 * @param cb    command buffer
 * @param value clear stencil value for cleaning
 */
inline void pushClearStencilCommand(
    CommandBuffer &cb        ,
    uint8_t        value = 0u){
  auto&cmd=cb.commands[cb.nofCommands];
  cmd.type = CommandType::CLEAR_STENCIL;
  auto&c = cmd.data.clearStencilCommand;
  c.value = value;
  cb.nofCommands++;
}


/**
 * @brief This function can be used to insert draw command into command buffer.
 *
 * @param cb command buffer
 * @param nofVertices number of vertices that should be rendered
 */
inline void pushDrawCommand(
    CommandBuffer &cb                     ,
    uint32_t       nofVertices            ){
  auto&cmd=cb.commands[cb.nofCommands];
  cmd.type = CommandType::DRAW;
  auto&c = cmd.data.drawCommand;
  c.nofVertices     = nofVertices    ;
  cb.nofCommands++;
}

/**
 * @brief This function can be used to insert bindFramebuffer command into command buffer.
 *
 * @param cb command buffer
 * @param id id of framebuffer to bind
 */
inline void pushBindFramebufferCommand(
    CommandBuffer&cb,
    uint32_t      id){
  auto&cmd=cb.commands[cb.nofCommands++];
  cmd.type = CommandType::BIND_FRAMEBUFFER;
  cmd.data.bindFramebufferCommand.id = id;
}

/**
 * @brief This function inserts bindProgram command into command buffer.
 *
 * @param cb command buffer
 * @param id id of program
 */
inline void pushBindProgramCommand(
    CommandBuffer&cb,
    uint32_t      id){
  auto&cmd=cb.commands[cb.nofCommands++];
  cmd.type = CommandType::BIND_PROGRAM;
  cmd.data.bindProgramCommand.id = id;
}

/**
 * @brief This function inserts bindVertexArray command into command buffer.
 *
 * @param cb command buffer
 * @param id id of vertex array
 */
inline void pushBindVertexArrayCommand(
    CommandBuffer&cb,
    uint32_t      id){
  auto&cmd=cb.commands[cb.nofCommands++];
  cmd.type = CommandType::BIND_VERTEXARRAY;
  cmd.data.bindVertexArrayCommand.id = id;
}

/**
 * @brief This function inserts setDrawId command into command buffer.
 *
 * @param cb command buffer
 * @param id gl_DrawID
 */
inline void pushSetDrawIdCommand(
    CommandBuffer&cb ,
    uint32_t      id){
  auto&cmd=cb.commands[cb.nofCommands++];
  cmd.type = CommandType::SET_DRAW_ID;
  cmd.data.setDrawIdCommand.id = id;
}

/**
 * @brief This function inserts subCommand into command buffer.
 *
 * @param cb  command buffer
 * @param sub pointer to sub command
 */
inline void pushSubCommand(
    CommandBuffer&cb ,
    CommandBuffer*sub){
  auto&cmd=cb.commands[cb.nofCommands++];
  cmd.type = CommandType::SUB_COMMAND;
  cmd.data.subCommand.commandBuffer = sub;
}

/**
 * @brief This function inserts setStencil command into command buffer
 *
 * @param cb       command buffer
 * @param settings stencil settings
 */
inline void pushSetStencilCommand(
    CommandBuffer        &cb      ,
    StencilSettings const&settings){
  auto&cmd=cb.commands[cb.nofCommands++];
  cmd.type = CommandType::SET_STENCIL_COMMAND;
  cmd.data.setStencilCommand.settings = settings;
}

/**
 * @brief This function inserts block writes command into command buffer.
 *
 * @param cb           command buffer
 * @param blockColor   set blocking for color writes
 * @param blockDepth   set blocking for depth writes
 * @param blockStencil set blocking for stencil writes
 */
inline void pushBlockWritesCommand(
    CommandBuffer&      cb   ,
    bool blockColor   = false,
    bool blockDepth   = false,
    bool blockStencil = false){
  auto&cmd=cb.commands[cb.nofCommands++];
  cmd.type = CommandType::BLOCK_WRITES_COMMAND;
  auto&d = cmd.data.blockWritesCommand;
  d.blockWrites.color   = blockColor  ;
  d.blockWrites.depth   = blockDepth  ;
  d.blockWrites.stencil = blockStencil;
}

/**
 * @brief This function inserts set backface culling command into command buffer.
 *
 * @param cb                  command buffer
 * @param enabled             is backface culling enabled?
 */
inline void pushSetBackfaceCullingCommand(
    CommandBuffer& cb                                 ,
    bool           enabled                     = false){
  auto& cmd = cb.commands[cb.nofCommands++];
  auto& d   = cmd.data.setBackfaceCullingCommand;
  d.enabled = enabled                                  ;
  cmd.type  = CommandType::SET_BACKFACE_CULLING_COMMAND;
}

/**
 * @brief This function inserts set front face command into command buffer
 *
 * @param cb                          command buffer
 * @param frontFaceIsCounterClockWise is front face specified in counter clock wise order?
 */
inline void pushSetFrontFaceCommand(
    CommandBuffer& cb                                ,
    bool           frontFaceIsCounterClockWise = true){
  auto& cmd = cb.commands[cb.nofCommands++];
  auto& d   = cmd.data.setFrontFaceCommand;
  d  .frontFaceIsCounterClockWise = frontFaceIsCounterClockWise        ;
  cmd.type                        = CommandType::SET_FRONT_FACE_COMMAND;
}

/**
 * @brief This function inserts user command into command buffer
 *
 * @param cb   command buffer
 * @param fce  user command
 * @param data user data
 */
inline void pushUserCommand(
    CommandBuffer& cb            ,
    UserCommandFce fce  = nullptr,
    void*          data = nullptr){
  auto&cmd=cb.commands[cb.nofCommands++];
  cmd.type = CommandType::USER_COMMAND;
  cmd.data.userCommand.callback = fce ;
  cmd.data.userCommand.data     = data;
}
