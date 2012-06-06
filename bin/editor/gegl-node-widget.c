#include "gegl-node-widget.h"

G_DEFINE_TYPE (GeglNodeWidget, gegl_node_widget, GTK_TYPE_DRAWING_AREA);

enum {
  PROP_0,
  N_PROPERTIES,
};

static GParamSpec *obj_properties[N_PROPERTIES] = { NULL, };

static void
gegl_node_widget_set_property (GObject	*object,
			       guint		 property_id,
			       const GValue	*value,
			       GParamSpec	*pspec)
{
  GeglNodeWidget	*self = GEGL_NODE_WIDGET(object);

  switch (property_id)
    {
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID(object, property_id, pspec);
      break;
    }
}

static void
gegl_node_widget_get_property (GObject	*object,
			       guint		 property_id,
			       GValue		*value,
			       GParamSpec	*pspec)
{
  GeglNodeWidget*	self = GEGL_NODE_WIDGET(object);
  
  switch(property_id)
    {
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID(object, property_id, pspec);
      break;
    }
}

EditorNode* new_editor_node(EditorNode* prev) {
  EditorNode* node = malloc(sizeof(EditorNode));
  node->next = NULL;
  node->x = node->y = 0;
  node->width = node->height = 100;
  if(prev != NULL)
    prev->next = node;
  node->title = "New Node";

  node->inputs = NULL;
  node->outputs = NULL;

  return node;
}

EditorNode* top_node(EditorNode* first)
{
  EditorNode* node = first;
  while(node->next != NULL) node = node->next;
  return node;
}

void
connect_pads(NodePad* a, NodePad* b)
{
  a->connected = b;
  b->connected = a;
}

NodePad*
get_pad_at(gint px, gint py, GeglNodeWidget* editor)
{
  NodePad* result = NULL;

  EditorNode* node = editor->first_node;
  for(;node != NULL; node = node->next)
    {
      gint x, y;
      gint width, height;

      if(node == editor->dragged_node)
	{
	  x = node->x+editor->px-editor->dx;
	  y = node->y+editor->py-editor->dy;
	}
      else
	{
	  x = node->x;
	  y = node->y;
	}

      //TODO: be more intelligent about minimum size (should be based on number of inputs/outputs so that they all fit properly)
      if(node->width < 100)
	node->width = 100;
      if(node->height < 50)
	node->height = 50;

      if(node == editor->resized_node)
	{
	  width = node->width+editor->px-editor->dx;
	  height = node->height+editor->py-editor->dy;
	}
      else
	{
	  width = node->width;
	  height = node->height;
	}

      if(width < 100)
	width = 100;
      if(height < 50)
	height = 50;

      gint title_height = node->title_height;

      int i = 0;
      NodePad* pad = node->inputs;
      for(;pad!=NULL;pad = pad->next, i++)
	{
	  if(px > x && py > y+(title_height)+10+20*i && px < x+10 && py < y+(title_height)+20+20*i)
	    result = pad;
	}

      i = 0;
      pad = node->outputs;
      for(;pad!=NULL;pad = pad->next, i++)
	{
	  if(px > x+width-10 && px < x+width &&
	     py > y+(title_height)+10+20*i &&
	     py < y+(title_height)+20+20*i)
	    result = pad;
	}
    }
  return result;
}

void
get_pad_position_input(NodePad* pad, gint* x, gint* y, cairo_t* cr, GeglNodeWidget* editor)
{
  cairo_select_font_face(cr, "Georgia",
			 CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);
  cairo_set_font_size(cr, 12);

  cairo_text_extents_t te;
  cairo_text_extents(cr, pad->node->title, &te);
  gint title_height = te.height+5;

  int i = 0;
  NodePad* _pad = pad->node->inputs;
  for(;_pad!=NULL;_pad = _pad->next, i++)
    {
      if(_pad == pad)
	{
	  gint node_x, node_y;

	  if(pad->node == editor->dragged_node)
	    {
	      node_x = pad->node->x+editor->px-editor->dx;
	      node_y = pad->node->y+editor->py-editor->dy;
	    }
	  else
	    {
	      node_x = pad->node->x;
	      node_y = pad->node->y;
	    }

	  *x = node_x+5;
	  *y = node_y+(title_height)+15+20*i;
	  break;
	}
    }
}

static void
draw_node(EditorNode* node, cairo_t *cr, GeglNodeWidget* editor)
{
  gint x, y;
  gint width, height;

  if(node == editor->dragged_node)
    {
      x = node->x+editor->px-editor->dx;
      y = node->y+editor->py-editor->dy;
    }
  else
    {
      x = node->x;
      y = node->y;
    }

  //TODO: be more intelligent about minimum size (should be based on number of inputs/outputs so that they all fit properly)
  if(node->width < 100)
    node->width = 100;
  if(node->height < 50)
    node->height = 50;

  if(node == editor->resized_node)
    {
      width = node->width+editor->px-editor->dx;
      height = node->height+editor->py-editor->dy;
    }
  else
    {
      width = node->width;
      height = node->height;
    }

  if(width < 100)
    width = 100;
  if(height < 50)
    height = 50;

  cairo_rectangle(cr, x, y, width, height);

  cairo_set_source_rgb(cr, 1, 1, 1);
  cairo_fill_preserve(cr);

  cairo_set_line_width(cr, 1);
  cairo_set_source_rgb(cr, 0, 0, 0);
  cairo_stroke(cr);


  cairo_select_font_face(cr, "Georgia",
			 CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);
  cairo_set_font_size(cr, 12);

  cairo_font_extents_t fe;
  cairo_font_extents(cr, &fe);

  cairo_text_extents_t te;
  cairo_text_extents(cr, node->title, &te);
  gint title_height = te.height+5;
  node->title_height = title_height;

  //draw the line separating the title
  cairo_move_to(cr, x, y+te.height+5);
  cairo_line_to(cr, x+width, y+te.height+5);
  cairo_stroke(cr);

  //setup the clip for the title text
  cairo_rectangle(cr, x, y, width, te.height+5);
  cairo_clip(cr);

  //draw the text
  cairo_move_to (cr, x-te.x_bearing +2.5,
		 y - te.y_bearing +2.5);
  cairo_show_text (cr, node->title);

  //get rid of the clip
  cairo_reset_clip(cr);

  cairo_move_to(cr, x+width-15, y+height);
  cairo_line_to(cr, x+width, y+height-15);
  cairo_stroke(cr);


  int i = 0;
  NodePad* pad = node->inputs;
  for(;pad!=NULL;pad = pad->next, i++)
    {
      cairo_text_extents(cr, pad->name, &te);

      cairo_set_source_rgb(cr, 0, 0, 0);
      cairo_rectangle(cr, x, y+(title_height)+10+20*i, 10, 10);
      cairo_fill(cr);

      cairo_set_source_rgb(cr, 0, 0, 0);
      cairo_move_to(cr, x+12.5, y+(title_height)+10+20*i+te.height/2+5);
      cairo_show_text(cr, pad->name);
    }

  i = 0;
  pad = node->outputs;
  for(;pad!=NULL;pad = pad->next, i++)
    {
      cairo_rectangle(cr, x, y, width, height);
      cairo_clip(cr);

      cairo_text_extents(cr, pad->name, &te);

      cairo_set_source_rgb(cr, 0, 0, 0);
      cairo_rectangle(cr, x+width-10, y+(title_height)+10+20*i, 10, 10);
      cairo_fill(cr);

      cairo_set_source_rgb(cr, 0, 0, 0);
      cairo_move_to(cr, x+width-15-te.width, y+(title_height)+10+20*i+te.height/2+5);
      cairo_show_text(cr, pad->name);

      cairo_reset_clip(cr);
      
      //TODO: render all connections underneath all nodes
      if(pad->connected)
	{
	  gint fx, fy;
	  fx = x+width-5;
	  fy = y+(title_height)+15+20*i;

	  gint tx, ty;
	  get_pad_position_input(pad->connected, &tx, &ty, cr, editor);


	  cairo_move_to(cr, fx, fy);
	  if(tx - fx > 200)
	    cairo_curve_to(cr, (fx+tx)/2, fy,
			   (fx+tx)/2, ty,
			   tx, ty);
	  else
	    cairo_curve_to(cr, fx+100, fy,
			   tx-100, ty,
			   tx, ty);
	  cairo_stroke(cr);

	}
      else if(pad == editor->dragged_pad)
	{
	  gint fx, fy;
	  fx = x+width-5;
	  fy = y+(title_height)+15+20*i;

	  gint tx = editor->px, ty = editor->py;

	  cairo_move_to(cr, fx, fy);
	  if(tx - fx > 200)
	    cairo_curve_to(cr, (fx+tx)/2, fy,
			   (fx+tx)/2, ty,
			   tx, ty);
	  else
	    cairo_curve_to(cr, fx+100, fy,
			   tx-100, ty,
			   tx, ty);
	  cairo_stroke(cr);

	}
    }
}

static gboolean
gegl_node_widget_draw(GtkWidget *widget, cairo_t *cr)
{
  GeglNodeWidget*	editor = GEGL_NODE_WIDGET(widget);

  cairo_set_source_rgb(cr, 1, 1, 1);
  cairo_paint(cr);

  EditorNode* node = editor->first_node;
  for(;node != NULL; node = node->next)
    {
      draw_node(node, cr, editor);
    }


  return FALSE;
}

#if GTK_MAJOR_VERSION == (2)
static gboolean
gegl_node_widget_expose (GtkWidget *widget, GdkEventExpose *event)
{
  cairo_t	*cr = gdk_cairo_create(widget->window);

  gegl_node_widget_draw(widget, cr);
  cairo_destroy(cr);

  return FALSE;
}
#endif

static gboolean
gegl_node_widget_motion(GtkWidget* widget, GdkEventMotion* event)
{
  GeglNodeWidget*	editor = GEGL_NODE_WIDGET(widget);
  editor->px		       = (gint)event->x;
  editor->py		       = (gint)event->y;

  /* redraw */
  gtk_widget_queue_draw(widget);


  return FALSE;
}

static gboolean
gegl_node_widget_button_press(GtkWidget* widget, GdkEventButton* event)
{
  GeglNodeWidget*	editor = GEGL_NODE_WIDGET(widget);
  //TODO: check which mouse button was pressed
  editor->left_mouse_down = TRUE;
  editor->dx = editor->px;
  editor->dy = editor->py;

  editor->dragged_pad = NULL;

  NodePad* pad = get_pad_at(editor->px, editor->py, editor);
  if(pad)
    {
      editor->dragged_pad = pad;
      if(pad->connected) {
	pad->connected->connected = NULL;
	pad->connected = NULL;
      }
    }
  else
    {
      EditorNode* node = editor->first_node;
      EditorNode* focus = NULL;
      for(;node != NULL; node = node->next)
	{
	  if(editor->px > node->x && editor->px < node->x+node->width &&
	     editor->py > node->y && editor->py < node->y+node->height) 
	    {
	      if(editor->px >= node->x+node->width-15 &&
		 editor->py >= node->y+node->height-15+(node->x+node->width-editor->px))
		{
		  editor->dragged_node = NULL;
		  editor->resized_node = node;
		}
	      else
		{
		  editor->resized_node = NULL;
		  editor->dragged_node = node;
		}

	      focus = node;
	    }
	}

      if(focus && focus->next != NULL)
	{
	  if(focus == editor->first_node)
	    {
	      editor->first_node = focus->next;
	    }

	  EditorNode* node = editor->first_node;

	  for(;node->next != NULL; node = node->next)
	    {
	      if(node->next == focus)
		{
		  node->next = focus->next;
		}
	    }

	  focus->next = NULL;
	  node->next = focus;
	}
    }

  gtk_widget_queue_draw(widget);

  return FALSE;
}

static gboolean
gegl_node_widget_button_release(GtkWidget* widget, GdkEventButton* event)
{
  GeglNodeWidget*	editor = GEGL_NODE_WIDGET(widget);

  /* TODO: check which mouse button was released instead of assuming it's the left one */
  editor->left_mouse_down = FALSE;

  if(editor->dragged_node)
    {
      editor->dragged_node->x += editor->px-editor->dx;
      editor->dragged_node->y += editor->py-editor->dy;
      editor->dragged_node = NULL;
    }

  if(editor->resized_node)
    {
      editor->resized_node->width += editor->px-editor->dx;
      editor->resized_node->height += editor->py-editor->dy;
      editor->resized_node = NULL;
    }

  if(editor->dragged_pad)
    {
      NodePad* pad = get_pad_at(editor->px, editor->py, editor);
      if(pad && pad != editor->dragged_pad) {
	connect_pads(pad, editor->dragged_pad);
      }
      editor->dragged_pad = NULL;
    }

  return FALSE;
}

static void
gegl_node_widget_class_init(GeglNodeWidgetClass *klass)
{
  GtkWidgetClass	*widget_class = GTK_WIDGET_CLASS(klass);
#if GTK_MAJOR_VERSION == (3)
  widget_class->draw		      = gegl_node_widget_draw;
#else
  widget_class->expose_event	      = gegl_node_widget_expose;
#endif
  widget_class->motion_notify_event   = gegl_node_widget_motion;
  widget_class->button_press_event    = gegl_node_widget_button_press;
  widget_class->button_release_event    = gegl_node_widget_button_release;
}



static void
gegl_node_widget_init(GeglNodeWidget* self)
{
  gtk_widget_add_events (GTK_WIDGET(self),
			 GDK_POINTER_MOTION_MASK |
			 GDK_BUTTON_PRESS_MASK   |
			 GDK_BUTTON_RELEASE_MASK );

  self->first_node = NULL;
  self->dragged_node = NULL;
  self->dragged_pad = NULL;
  self->resized_node = NULL;

  EditorNode* node = new_editor_node(NULL);

  NodePad* input = malloc(sizeof(NodePad));
  input->next = NULL;
  input->connected = NULL;
  input->name = "In";
  input->node = node;

  NodePad* input2 = malloc(sizeof(NodePad));
  input2->next = NULL;
  input2->connected = NULL;
  input2->name = "Mask";
  input2->node = node;

  input->next = input2;
  node->inputs = input;

  NodePad* output = malloc(sizeof(NodePad));
  output->next = NULL;
  output->connected = NULL;
  output->name = "Out";
  output->node = node;

  node->outputs = output;

  self->first_node = node;

  node = new_editor_node(NULL);

  input = malloc(sizeof(NodePad));
  input->next = NULL;
  input->connected = NULL;
  input->name = "In";
  input->node = node;

  input2 = malloc(sizeof(NodePad));
  input2->next = NULL;
  input2->connected = NULL;
  input2->name = "Mask";
  input2->node = node;

  input->next = input2;
  node->inputs = input;

  output = malloc(sizeof(NodePad));
  output->next = NULL;
  output->connected = NULL;
  output->name = "Out";
  output->node = node;

  node->outputs = output;

  self->first_node->next = node;
  node->x = 200;

  connect_pads(self->first_node->inputs, node->outputs);

  /*   = new_editor_node(self->first_node);
       node->x = 50;
       node = new_editor_node(node);
       node->x = 100;
       node->y = 14;*/
}

GtkWidget* 
gegl_node_widget_new ( void )
{
  return g_object_new (GEGL_TYPE_NODE_WIDGET, NULL);
}
