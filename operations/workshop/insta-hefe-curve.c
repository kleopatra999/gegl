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

#define GEGL_OP_POINT_FILTER
#define GEGL_OP_C_FILE "insta-hefe-curve.c"

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
         void                *in_buf,
         void                *out_buf,
         glong                n_pixels,
         const GeglRectangle *roi,
         gint                 level)
{
  gfloat *in = in_buf;
  gfloat *out = out_buf;
  glong i;

  for (i = 0; i < n_pixels; i++)
    {
      const float b = in[2];
      const float b2 = b * b;
      const float b3 = b2 * b;
      const float g = in[1];
      const float g2 = g * g;
      const float g3 = g2 * g;
      const float r = in[0];
      const float r2 = r * r;
      const float r3 = r2 * r;

      out[0] = -13.47f * r3 * r3
        + 41.23f * r3 * r2
        - 45.04f * r2 * r2
        + 19.17 * r3
        - 1.492f * r2
        + 0.5954f * r;

      out[1] = -12.28f * g3 * g3
        + 41.09f * g3 * g2
        - 50.52f * g2 * g2
        + 26.03 * g3
        - 3.916f * g2
        + 0.58f * g;

      out[2] = -1.066f * b3 * b3
        + 9.679f * b3 * b2
        - 19.09f * b2 * b2
        + 12.92 * b3
        - 1.835f * b2
        + 0.3487f * b;

      out[3] = in[3];

      in += 4;
      out += 4;
    }

  return TRUE;
}

static void
gegl_op_class_init (GeglOpClass *klass)
{
  GeglOperationClass *operation_class = GEGL_OPERATION_CLASS (klass);
  GeglOperationPointFilterClass *point_filter_class =
    GEGL_OPERATION_POINT_FILTER_CLASS (klass);

  operation_class->prepare = prepare;
  operation_class->opencl_support = FALSE;

  point_filter_class->process = process;

  gegl_operation_class_set_keys (operation_class,
    "name",        "gegl:insta-hefe-curve",
    "title",       _("Insta Hefe Curve"),
    "categories",  "hidden",
    "description", _("Apply the Hefe curve to an image"),
    NULL);
}

#endif
