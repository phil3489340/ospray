/*! \file ospray/ospray.h Public OSPRay API definition file (core components)

  This file defines the public API for the OSPRay core. Since many of
  OSPRays components are intentionally realized in specific modules
  this file will *only* define the very *core* components, with all
  extensions/modules implemented in the respective modules.
 */

#pragma once

// -------------------------------------------------------
// include embree components 
// -------------------------------------------------------
#include "common/math/vec2.h"
#include "common/math/vec3.h"
#include "common/math/bbox.h"

/*! namespace for classes in the public core API */
namespace osp {
  typedef embree::Vec2i  vec2i;
  typedef embree::Vec3f  vec3f;
  typedef embree::Vec3fa vec3fa;

  struct ManagedObject { unsigned long ID; virtual ~ManagedObject() {} };
  struct FrameBuffer  : public ManagedObject {};
  struct Renderer     : public ManagedObject {};
  struct Model        : public ManagedObject {};
  struct Data         : public ManagedObject {};
  struct Geometry     : public ManagedObject {};
  struct TriangleMesh : public Geometry {};
}


/* OSPRay constants for Frame Buffer creation ('and' ed together) */
typedef enum {
  OSP_RGBA_I8,  /*!< one dword per pixel: rgb+alpha, each on byte */
  OSP_RGBA_F32, /*!< one float4 per pixel: rgb+alpha, each one float */
  OSP_RGBZ_F32, /*!< one float4 per pixel: rgb+depth, each one float */ 
} OSPFrameBufferMode;

typedef enum {
  // object reference type
  OSP_OBJECT,
  // atomic types
  OSP_INT, 
  OSP_FLOAT, 
  // vector types
  OSP_vec2f,
  OSP_vec3f,
  OSP_vec3fa, 
  OSP_vec4i
} OSPDataType;

typedef enum {
  OSP_OK=0, /*! no error; any value other than zero means 'some kind of error' */
  OSP_GENERAL_ERROR /*! unspecified error */
} OSPResult;

typedef osp::FrameBuffer   *OSPFrameBuffer;
typedef osp::Renderer      *OSPRenderer;
typedef osp::Model         *OSPModel;
typedef osp::Data          *OSPData;
typedef osp::Geometry      *OSPGeometry;
typedef osp::TriangleMesh  *OSPTriangleMesh;
typedef osp::ManagedObject *OSPObject;

/*! \defgroup cplusplus_api Public OSPRay Core API */
/*! @{ */
extern "C" {
  /*! initialize the ospray engine (for single-node user application) */
  void ospInit(int *ac, const char **av);


  // -------------------------------------------------------
  /*! \defgroup ospray_render Creating and using Renderers */
  /*! @{ */

  /*! use renderer to render given model into specified frame buffer */
  void ospRenderFrame(OSPFrameBuffer fb, OSPRenderer renderer, OSPModel model);

  /*! \brief create a new renderer of given type */
  /*! \detailed return 'NULL' if that type is not known */
  OSPRenderer ospNewRenderer(const char *type);

  /*! @} */

  // -------------------------------------------------------
  /*! \defgroup ospray_framebuffer Frame Buffer Manipulation API */
  /*! @{ */
  /*! create a new framebuffer (actual format is internal to ospray) */
  OSPFrameBuffer ospNewFrameBuffer(const osp::vec2i &size, 
                                   const OSPFrameBufferMode mode=OSP_RGBA_I8,
                                   const size_t swapChainDepth=2);
  /*! \brief free a framebuffer \detailed due to refcounting the frame
      buffer may not immeidately be deleted at this time */
  void ospFreeFrameBuffer(OSPFrameBuffer fb);
  /*! map app-side content of a framebuffer (see \ref frame_buffer_handling) */
  const void *ospMapFrameBuffer(OSPFrameBuffer fb);
  /*! unmap a previously mapped frame buffer (see \ref frame_buffer_handling) */
  void ospUnmapFrameBuffer(const void *mapped, OSPFrameBuffer fb);

  // /*! (not implemented yet) add a additional dedicated channel to a
  //   frame buffer. ie, add a (OSPint:"ID0") channel for ID rendering,
  //   or a "OSPFloat:"depth" depth channel (in addition to the primary
  //   RGBA_F32 buffer), etc). Note that the channel inherits the
  //   framebuffer's default compression mode (with the data type
  //   indicating the best compression mode) */
  // void ospFrameBufferChannelAdd(const char *which, OSPDataType data);
  // /*! (not implemented yet) map a specific (non-default) frame buffer
  //     channel by name */
  // const void *ospFrameBufferChannelMap(OSPFrameBuffer fb, const char *channel);
  /*! @} end of ospray_framebuffer */


  // -------------------------------------------------------
  /*! \defgroup ospray_data Data Buffer Handling */
  /*! @{ */
  /*! create a new data buffer, with optional init data and control flags */
  OSPData ospNewData(size_t numItems, OSPDataType format, void *init=NULL, int flags=0);
  /*! add a data array to another object */
  void ospSetData(OSPObject _object, const char *id, OSPData data);
  /*! @} end of ospray_data */

  // -------------------------------------------------------
  /*! \defgroup ospray_geometry Geometry Handling */
  /*! @{ */

  /*! \brief create a new triangle mesh. \detailed the mesh doesn't do
      anything 'til it is added to a model. to set the model's
      vertices etc, set the respective data arrays for "position",
      "index", "normal", "texcoord", "color", etc. Data format for
      vertices and normals in vec3fa, and vec4i for index (fourth
      component is the material ID). */
  OSPTriangleMesh ospNewTriangleMesh();
  /*! add an already created geometry to a model */
  void ospAddGeometry(OSPModel model, OSPGeometry mesh);
  /*! @} end of ospray_trianglemesh */

  // -------------------------------------------------------
  /*! \defgroup ospray_model OSPRay Model Handling 
    
    \detailed models are the equivalent of 'scenes' in embree (ie,
      they consist of one or more pieces of content that share a
      single logical acceleration structure); however, models can
      contain more than just embree triangle meshes - they can also
      contain cameras, materials, volume data, etc, as well as
      references to (and instances of) other models.*/
  /*! @{ */

  /*! \brief create a new ospray model.  */
  OSPModel ospNewModel();
  void ospFinalizeModel(OSPModel model);
  /*! @} end of ospray_model */
  


}
/*! @} */
