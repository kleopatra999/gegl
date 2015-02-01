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
 * Copyright 2010 Øyvind Kolås <pippin@gimp.org>
 *           2012 Ville Sokk <ville.sokk@gmail.com>
 *           2013 Téo Mazars <teo.mazars@ensimag.fr>
 */

#include "config.h"
#include <glib/gi18n-lib.h>

#ifdef GEGL_PROPERTIES

property_int (wrong_pixels, _("Wrong pixels"), 0)
    description(_("Number of differing pixels."))

property_double (max_diff, _("Maximum difference"), 0.0)
    description(_("Maximum difference between two pixels."))

property_double (avg_diff_wrong, _("Average difference (wrong)"), 0.0)
    description(_("Average difference between wrong pixels."))

property_double (avg_diff_total, _("Average difference (total)"), 0.0)
    description(_("Average difference between all pixels."))

#else

#define GEGL_OP_COMPOSER
#define GEGL_OP_C_SOURCE image-compare-rgb.c

#include "gegl-op.h"

#define ERROR_TOLERANCE 0.01
#define SQR(x)          ((x) * (x))

static void
prepare (GeglOperation *self)
{
  gegl_operation_set_format (self, "input",  babl_format ("R'G'B' u8"));
  gegl_operation_set_format (self, "aux",    babl_format ("R'G'B' u8"));
  gegl_operation_set_format (self, "output", babl_format ("R'G'B' u8"));
}

static GeglRectangle
get_required_for_output (GeglOperation       *operation,
                         const gchar         *input_pad,
                         const GeglRectangle *region)
{
  return *gegl_operation_source_get_bounding_box (operation, "input");
}

static GeglRectangle
get_cached_region (GeglOperation       *operation,
                   const GeglRectangle *roi)
{
  return *gegl_operation_source_get_bounding_box (operation, "input");
}

static gboolean
process (GeglOperation       *operation,
         GeglBuffer          *input,
         GeglBuffer          *aux,
         GeglBuffer          *output,
         const GeglRectangle *result,
         gint                 level)
{
  const Babl         *srgb         = babl_format ("R'G'B' u8");
  GeglBuffer         *diff_buffer;
  GeglBufferIterator *iter;

  if (aux == NULL)
    return TRUE;

  diff_buffer = gegl_buffer_new (result, srgb);

  iter = gegl_buffer_iterator_new (diff_buffer, result, 0, srgb,
                                   GEGL_ACCESS_WRITE, GEGL_ABYSS_NONE);

  gegl_buffer_iterator_add (iter, input, result, 0, srgb,
                            GEGL_ACCESS_READ, GEGL_ABYSS_NONE);

  gegl_buffer_iterator_add (iter, aux, result, 0, srgb,
                            GEGL_ACCESS_READ, GEGL_ABYSS_NONE);

  while (gegl_buffer_iterator_next (iter))
    {
      gint    i;
      guint8  *data_in1 = iter->data[1];
      guint8  *data_in2 = iter->data[2];

      for (i = 0; i < iter->length; i++)
        {
          g_message ("(%d, %d, %d)   (%d, %d ,%d)",
                     (gint) data_in1[0],
                     (gint) data_in1[1],
                     (gint) data_in1[2],
                     (gint) data_in2[0],
                     (gint) data_in2[1],
                     (gint) data_in2[2]);
          data_in1 += 3;
          data_in2 += 3;
        }
    }

  g_object_unref (diff_buffer);

  return TRUE;
}

static void
gegl_op_class_init (GeglOpClass *klass)
{
  GeglOperationClass         *operation_class;
  GeglOperationComposerClass *composer_class;

  operation_class = GEGL_OPERATION_CLASS (klass);
  composer_class  = GEGL_OPERATION_COMPOSER_CLASS (klass);

  operation_class->prepare                 = prepare;
  operation_class->get_required_for_output = get_required_for_output;
  operation_class->get_cached_region       = get_cached_region;
  composer_class->process                  = process;
  operation_class->threaded                = FALSE;

  gegl_operation_class_set_keys (operation_class,
    "name"       , "gegl:image-compare-rgb",
    "categories" , "programming",
    "description", _("Compares if input and aux buffers are "
                     "different. Results are saved in the "
                     "properties."),
    NULL);
}

#endif
