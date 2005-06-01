/*
 *   This file is part of GEGL.
 *
 *    GEGL is free software; you can redistribute it and/or modify
 *    it under the terms of the GNU General Public License as published by
 *    the Free Software Foundation; either version 2 of the License, or
 *    (at your option) any later version.
 *
 *    GEGL is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU General Public License for more details.
 *
 *    You should have received a copy of the GNU General Public License
 *    along with GEGL; if not, write to the Free Software
 *    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 *  Copyright 2003 Daniel S. Rogers
 *
 */

#include <glib-object.h>

#include "gegl-image-types.h"

#include "gegl-cache-entry.h"


static void class_init    (gpointer       g_class,
                           gpointer       class_data);
static void instance_init (GTypeInstance *instance,
                           gpointer       g_class);


GType
gegl_cache_entry_get_type (void)
{
  static GType type=0;
  if (!type)
    {
      static const GTypeInfo typeInfo =
	{
	  /* interface types, classed types, instantiated types */
	  sizeof(GeglCacheEntryClass),
	  NULL, /*base_init*/
	  NULL, /* base_finalize */

	  /* classed types, instantiated types */
	  class_init, /* class_init */
	  NULL, /* class_finalize */
	  NULL, /* class_data */

	  /* instantiated types */
	  sizeof(GeglCacheEntry),
	  0, /* n_preallocs */
	  instance_init, /* instance_init */

	  /* value handling */
	  NULL /* value_table */
	};

      type = g_type_register_static (G_TYPE_OBJECT ,
				     "GeglCacheEntry",
				     &typeInfo,
				     G_TYPE_FLAG_ABSTRACT);
    }
  return type;
}


static void
class_init(gpointer g_class,
	   gpointer class_data)
{
  GeglCacheEntryClass *class=GEGL_CACHE_ENTRY_CLASS(g_class);
  class->flattened_size=NULL;
  class->flatten=NULL;
  class->unflatten=NULL;
}
static void
instance_init(GTypeInstance *instance,
	      gpointer g_class)
{
  GeglCacheEntry *self = GEGL_CACHE_ENTRY(instance);
  self->cache_hints=NULL;
}

gsize
gegl_cache_entry_flattened_size (const GeglCacheEntry* self)
{
  g_return_val_if_fail(self != NULL, 0);
  g_return_val_if_fail(GEGL_IS_CACHE_ENTRY(self),0);

  GeglCacheEntryClass * class=GEGL_CACHE_ENTRY_GET_CLASS(self);
  g_return_val_if_fail(class->flattened_size != NULL,0);
  return class->flattened_size(self);
}

void
gegl_cache_entry_flatten (GeglCacheEntry* self,
			  gpointer buffer,
			  gsize length)
{
  g_return_if_fail (self != NULL);
  g_return_if_fail (GEGL_IS_CACHE_ENTRY(self));

  GeglCacheEntryClass * class=GEGL_CACHE_ENTRY_GET_CLASS(self);
  g_return_if_fail (class->flatten != NULL);
  g_return_if_fail (gegl_cache_entry_flattened_size (self) <= length);
  class->flatten (self, buffer, length);
}

void
gegl_cache_entry_unflatten (GeglCacheEntry* self,
			    gpointer buffer,
			    gsize length)
{
  g_return_if_fail (self != NULL);
  g_return_if_fail (GEGL_IS_CACHE_ENTRY(self));

  GeglCacheEntryClass * class=GEGL_CACHE_ENTRY_GET_CLASS(self);
  g_return_if_fail (class->unflatten != NULL);
  g_return_if_fail (gegl_cache_entry_flattened_size (self) <= length);
  class->unflatten (self, buffer, length);
}

void
gegl_cache_entry_discard (GeglCacheEntry* self)
{
  g_return_if_fail (self != NULL);
  g_return_if_fail (GEGL_IS_CACHE_ENTRY(self));

  GeglCacheEntryClass * class=GEGL_CACHE_ENTRY_GET_CLASS(self);
  g_return_if_fail (class->discard != NULL);
  class->discard (self);
}
