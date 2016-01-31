#include "test-common.h"

void crop(GeglBuffer *buffer);

gint
main (gint    argc,
      gchar **argv)
{
  GeglBuffer *buffer;

  gegl_init (&argc, &argv);

  buffer = test_buffer (1024, 1024, babl_format ("RGBA float"));
  bench ("crop", buffer, &crop);
  g_object_unref (buffer);

  gegl_exit ();
  return 0;
}

void crop(GeglBuffer *buffer)
{
  GeglBuffer *buffer2;
  GeglNode   *crop, *gegl, *source, *sink;

  gegl = gegl_node_new ();
  source = gegl_node_new_child (gegl,
                                "operation", "gegl:buffer-source",
                                "buffer", buffer,
                                NULL);
  crop = gegl_node_new_child (gegl,
                              "operation", "gegl:crop",
                              "x", 0.0,
                              "y", 0.0,
                              "width", 10.0,
                              "height", 10.0,
                              NULL);
  sink = gegl_node_new_child (gegl,
                              "operation", "gegl:buffer-sink",
                              "buffer", &buffer2,
                              NULL);

  gegl_node_link_many (source, crop, sink, NULL);
  gegl_node_process (sink);
  g_object_unref (gegl);
  g_object_unref (buffer2);
}
