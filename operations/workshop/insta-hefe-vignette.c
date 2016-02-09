/* This file is an image processing operation for GEGL
 *
 * GEGL is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 3 of the License, or (at your option) any later version.
 *
 * GEGL is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with GEGL; if not, see <http://www.gnu.org/licenses/>.
 *
 * Copyright 2016 Red Hat, Inc.
 */

#include "config.h"
#include <glib/gi18n-lib.h>

#ifdef GEGL_PROPERTIES

#else

#define GEGL_OP_POINT_RENDER
#define GEGL_OP_C_FILE "insta-hefe-vignette.c"

#include "gegl-op.h"

static void
prepare (GeglOperation *operation)
{
  const Babl *format;

  format = babl_format ("R'G'B'A float");
  gegl_operation_set_format (operation, "input", format);
  gegl_operation_set_format (operation, "output", format);
}

static gboolean
process (GeglOperation       *operation,
         void                *out_buf,
         glong                n_pixels,
         const GeglRectangle *roi,
         gint                 level)
{
  return TRUE;
}

static void
gegl_op_class_init (GeglOpClass *klass)
{
  GeglOperationClass *operation_class = GEGL_OPERATION_CLASS (klass);
  GeglOperationPointRenderClass *point_render_class =
    GEGL_OPERATION_POINT_RENDER_CLASS (klass);

  operation_class->get_bounding_box = get_bounding_box;
  operation_class->prepare = prepare;
  operation_class->opencl_support = FALSE;

  point_render_class->process = process;

  gegl_operation_class_set_keys (operation_class,
    "name",        "gegl:insta-hefe-vignette",
    "title",       _("Insta Hefe Vignette"),
    "categories",  "hidden",
    "description", _("Apply the Hefe vignette to an image"),
    NULL);
}

#endif
