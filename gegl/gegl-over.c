#include "gegl-over.h"
#include "gegl-scanline-processor.h"
#include "gegl-tile-iterator.h"
#include "gegl-utils.h"

static void class_init (GeglOverClass * klass);
static void init (GeglOver * self, GeglOverClass * klass);

static GeglScanlineFunc get_scanline_func(GeglComp * comp, GeglColorSpace space, GeglChannelDataType type);

static void fg_over_bg_float (GeglFilter * filter, GeglTileIterator ** iters, gint width);

static gpointer parent_class = NULL;

GType
gegl_over_get_type (void)
{
  static GType type = 0;

  if (!type)
    {
      static const GTypeInfo typeInfo =
      {
        sizeof (GeglOverClass),
        (GBaseInitFunc) NULL,
        (GBaseFinalizeFunc) NULL,
        (GClassInitFunc) class_init,
        (GClassFinalizeFunc) NULL,
        NULL,
        sizeof (GeglOver),
        0,
        (GInstanceInitFunc) init,
      };

      type = g_type_register_static (GEGL_TYPE_COMP, 
                                     "GeglOver", 
                                     &typeInfo, 
                                     0);
    }
    return type;
}

static void 
class_init (GeglOverClass * klass)
{
  GeglCompClass *comp_class = GEGL_COMP_CLASS(klass);
  parent_class = g_type_class_peek_parent(klass);
  comp_class->get_scanline_func = get_scanline_func;
}

static void 
init (GeglOver * self, 
      GeglOverClass * klass)
{
}

/* scanline_funcs[data type] */
static GeglScanlineFunc scanline_funcs[] = 
{ 
  NULL, 
  NULL, 
  fg_over_bg_float, 
  NULL 
};

static GeglScanlineFunc
get_scanline_func(GeglComp * comp,
                  GeglColorSpace space,
                  GeglChannelDataType type)
{
  return scanline_funcs[type];
}


static void                                                            
fg_over_bg_float (GeglFilter * filter,              
                      GeglTileIterator ** iters,        
                      gint width)                       
{                                                                       
  gfloat **d = (gfloat**)gegl_tile_iterator_color_channels(iters[0]);
  gfloat *da = (gfloat*)gegl_tile_iterator_alpha_channel(iters[0]);
  gint d_color_chans = gegl_tile_iterator_get_num_colors(iters[0]);

  gfloat **b = (gfloat**)gegl_tile_iterator_color_channels(iters[1]);
  gfloat *ba = (gfloat*)gegl_tile_iterator_alpha_channel(iters[1]);
  gint b_color_chans = gegl_tile_iterator_get_num_colors(iters[1]);

  gfloat **f = (gfloat**)gegl_tile_iterator_color_channels(iters[2]);
  gfloat * fa = (gfloat*)gegl_tile_iterator_alpha_channel(iters[2]);
  gint f_color_chans = gegl_tile_iterator_get_num_colors(iters[2]);

  gint alpha_mask = 0x0;

  if(ba) 
    alpha_mask |= GEGL_BG_ALPHA; 
  if(fa)
    alpha_mask |= GEGL_FG_ALPHA; 

  {
    gfloat *d0 = (d_color_chans > 0) ? d[0]: NULL;   
    gfloat *d1 = (d_color_chans > 1) ? d[1]: NULL;
    gfloat *d2 = (d_color_chans > 2) ? d[2]: NULL;

    gfloat *b0 = (b_color_chans > 0) ? b[0]: NULL;   
    gfloat *b1 = (b_color_chans > 1) ? b[1]: NULL;
    gfloat *b2 = (b_color_chans > 2) ? b[2]: NULL;

    gfloat *f0 = (f_color_chans > 0) ? f[0]: NULL;   
    gfloat *f1 = (f_color_chans > 1) ? f[1]: NULL;
    gfloat *f2 = (f_color_chans > 2) ? f[2]: NULL;

    switch(alpha_mask)
      {
      case GEGL_NO_ALPHA:
          {
            while(width--)                                                        
              {                                                                   
                switch(d_color_chans)
                  {
                    case 3: *d2++ = *f2++;
                    case 2: *d1++ = *f1++;
                    case 1: *d0++ = *f0++;
                    case 0:        
                  }
              }
          }
        break;
      case GEGL_FG_ALPHA:
          {
            gfloat a;                                              
            while(width--)                                                        
              {                                                                   
                a = 1.0 - *fa++;                                              
                switch(d_color_chans)
                  {
                    case 3: *d2++ = a * *b2++ + *f2++;
                    case 2: *d1++ = a * *b1++ + *f1++;
                    case 1: *d0++ = a * *b0++ + *f0++;
                    case 0:        
                  }

                *da++ = 1.0; 
              }
          }
        break;
      case GEGL_BG_ALPHA:
          {
            while(width--)                                                        
              {                                                                   
                switch(d_color_chans)
                  {
                    case 3: *d2++ = *f2++;
                    case 2: *d1++ = *f1++;
                    case 1: *d0++ = *f0++;
                    case 0:        
                  }

                *da++ = 1.0; 
              }
          }
        break;
      case GEGL_FG_BG_ALPHA:
          {
            gfloat a;                                              
            while(width--)                                                        
              {                                                                   
                a = 1.0 - *fa;                                              
                switch(d_color_chans)
                  {
                    case 3: *d2++ = a * *b2++ + *f2++;
                    case 2: *d1++ = a * *b1++ + *f1++;
                    case 1: *d0++ = a * *b0++ + *f0++;
                    case 0:        
                  }

                *da++ = a * *ba++ + *fa++; 
              }
          }
        break;
      }
  }

  g_free(d);
  g_free(b);
  g_free(f);
}                                                                       
