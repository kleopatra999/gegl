#ifndef __GEGL_BLEND_H__
#define __GEGL_BLEND_H__

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include "gegl-comp.h"

#define GEGL_TYPE_BLEND               (gegl_blend_get_type ())
#define GEGL_BLEND(obj)               (G_TYPE_CHECK_INSTANCE_CAST ((obj), GEGL_TYPE_BLEND, GeglBlend))
#define GEGL_BLEND_CLASS(klass)       (G_TYPE_CHECK_CLASS_CAST ((klass),  GEGL_TYPE_BLEND, GeglBlendClass))
#define GEGL_IS_BLEND(obj)            (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GEGL_TYPE_BLEND))
#define GEGL_IS_BLEND_CLASS(klass)    (G_TYPE_CHECK_CLASS_TYPE ((klass),  GEGL_TYPE_BLEND))
#define GEGL_BLEND_GET_CLASS(obj)     (G_TYPE_INSTANCE_GET_CLASS ((obj),  GEGL_TYPE_BLEND, GeglBlendClass))

#ifndef __TYPEDEF_GEGL_BLEND__
#define __TYPEDEF_GEGL_BLEND__
typedef struct _GeglBlend GeglBlend;
#endif
struct _GeglBlend 
{
    GeglComp comp;

    /*< private >*/
    gfloat opacity;
};

typedef struct _GeglBlendClass GeglBlendClass;
struct _GeglBlendClass 
{
    GeglCompClass comp_class;
};

GType           gegl_blend_get_type          (void);

gfloat          gegl_blend_get_opacity       (GeglBlend * self);
void            gegl_blend_set_opacity       (GeglBlend * self, 
                                              gfloat opacity);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
