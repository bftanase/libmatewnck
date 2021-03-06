/* class group object */
/* vim: set sw=2 et: */

/*
 * Copyright (C) 2003 Ximian, Inc.
 * Authors: Federico Mena-Quintero <federico@ximian.com>
 * Copyright (C) 2006-2007 Vincent Untz
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 51 Franklin St, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#include <string.h>
#include "class-group.h"
#include "window.h"
#include "private.h"

/**
 * SECTION:class-group
 * @short_description: an object representing a group of windows of the same
 * class.
 * @see_also: matewnck_window_get_class_group()
 * @stability: Unstable
 *
 * The #MatewnckClassGroup is a group of #MatewnckWindow that are all in the same
 * class. It can be used to represent windows by classes, group windows by
 * classes or to manipulate all windows of a particular class.
 *
 * The class of a window is defined by the WM_CLASS property of this window.
 * More information about the WM_CLASS property is available in the <ulink
 * url="http://tronche.com/gui/x/icccm/sec-4.html&num;s-4.1.2.5">WM_CLASS Property</ulink>
 * section (section 4.1.2.5) of the <ulink
 * url="http://tronche.com/gui/x/icccm/">ICCCM</ulink>.
 *
 * The #MatewnckClassGroup objects are always owned by libmatewnck and must not be
 * referenced or unreferenced.
 */

/* Private part of the MatewnckClassGroup structure */
struct _MatewnckClassGroupPrivate {
  char *res_class;
  char *name;
  GList *windows;

  GdkPixbuf *icon;
  GdkPixbuf *mini_icon;
};

G_DEFINE_TYPE (MatewnckClassGroup, matewnck_class_group, G_TYPE_OBJECT);
#define MATEWNCK_CLASS_GROUP_GET_PRIVATE(o) (G_TYPE_INSTANCE_GET_PRIVATE ((o), MATEWNCK_TYPE_CLASS_GROUP, MatewnckClassGroupPrivate))

/* Hash table that maps res_class strings -> MatewnckClassGroup instances */
static GHashTable *class_group_hash = NULL;



static void matewnck_class_group_class_init  (MatewnckClassGroupClass *class);
static void matewnck_class_group_init        (MatewnckClassGroup      *class_group);
static void matewnck_class_group_finalize    (GObject             *object);

enum {
  NAME_CHANGED,
  ICON_CHANGED,
  LAST_SIGNAL
};

static guint signals[LAST_SIGNAL] = { 0 };

static void
matewnck_class_group_class_init (MatewnckClassGroupClass *class)
{
  GObjectClass *gobject_class = G_OBJECT_CLASS (class);

  g_type_class_add_private (class, sizeof (MatewnckClassGroupPrivate));

  gobject_class->finalize = matewnck_class_group_finalize;

  /**
   * MatewnckClassGroup::name-changed:
   * @class_group: the #MatewnckClassGroup which emitted the signal.
   *
   * Emitted when the name of @class_group changes.
   */
  signals[NAME_CHANGED] =
    g_signal_new ("name_changed",
                  G_OBJECT_CLASS_TYPE (gobject_class),
                  G_SIGNAL_RUN_LAST,
                  G_STRUCT_OFFSET (MatewnckClassGroupClass, name_changed),
                  NULL, NULL,
                  g_cclosure_marshal_VOID__VOID,
                  G_TYPE_NONE, 0);
  /**
   * MatewnckClassGroup::icon-changed:
   * @class_group: the #MatewnckClassGroup which emitted the signal.
   *
   * Emitted when the icon of @class_group changes.
   */
  signals[ICON_CHANGED] =
    g_signal_new ("icon_changed",
                  G_OBJECT_CLASS_TYPE (gobject_class),
                  G_SIGNAL_RUN_LAST,
                  G_STRUCT_OFFSET (MatewnckClassGroupClass, icon_changed),
                  NULL, NULL,
                  g_cclosure_marshal_VOID__VOID,
                  G_TYPE_NONE, 0);
}

static void
matewnck_class_group_init (MatewnckClassGroup *class_group)
{
  class_group->priv = MATEWNCK_CLASS_GROUP_GET_PRIVATE (class_group);

  class_group->priv->res_class = NULL;
  class_group->priv->name = NULL;
  class_group->priv->windows = NULL;

  class_group->priv->icon = NULL;
  class_group->priv->mini_icon = NULL;
}

static void
matewnck_class_group_finalize (GObject *object)
{
  MatewnckClassGroup *class_group;

  class_group = MATEWNCK_CLASS_GROUP (object);

  if (class_group->priv->res_class)
    g_free (class_group->priv->res_class);
  class_group->priv->res_class = NULL;

  if (class_group->priv->name)
    g_free (class_group->priv->name);
  class_group->priv->name = NULL;

  g_list_free (class_group->priv->windows);
  class_group->priv->windows = NULL;

  if (class_group->priv->icon)
    g_object_unref (class_group->priv->icon);
  class_group->priv->icon = NULL;

  if (class_group->priv->mini_icon)
    g_object_unref (class_group->priv->mini_icon);
  class_group->priv->mini_icon = NULL;

  G_OBJECT_CLASS (matewnck_class_group_parent_class)->finalize (object);
}

/**
 * matewnck_class_group_get:
 * @res_class: name of the sought resource class.
 *
 * Gets the #MatewnckClassGroup corresponding to @res_class.
 *
 * Return value: (transfer none): the #MatewnckClassGroup corresponding to
 * @res_class, or %NULL if there is no #MatewnckClassGroup with the specified
 * @res_class. The returned #MatewnckClassGroup is owned by libmatewnck and must not be
 * referenced or unreferenced.
 *
 * Since: 2.2
 **/
MatewnckClassGroup *
matewnck_class_group_get (const char *res_class)
{
  if (!class_group_hash)
    return NULL;
  else
    return g_hash_table_lookup (class_group_hash, res_class ? res_class : "");
}

/**
 * _matewnck_class_group_create:
 * @res_class: name of the resource class for the group.
 *
 * Creates a new MatewnckClassGroup with the specified resource class name.  If
 * @res_class is #NULL, then windows without a resource class name will get
 * grouped under this class group.
 *
 * Return value: a newly-created #MatewnckClassGroup, or an existing one that
 * matches the @res_class.
 **/
MatewnckClassGroup *
_matewnck_class_group_create (const char *res_class)
{
  MatewnckClassGroup *class_group;

  if (class_group_hash == NULL)
    class_group_hash = g_hash_table_new (g_str_hash, g_str_equal);

  g_return_val_if_fail (g_hash_table_lookup (class_group_hash, res_class ? res_class : "") == NULL,
			NULL);

  class_group = g_object_new (MATEWNCK_TYPE_CLASS_GROUP, NULL);

  class_group->priv->res_class = g_strdup (res_class ? res_class : "");

  g_hash_table_insert (class_group_hash,
                       class_group->priv->res_class, class_group);
  /* Hash now owns one ref, caller gets none */

  return class_group;
}

/**
 * _matewnck_class_group_destroy:
 * @class_group: a #MatewnckClassGroup.
 *
 * Destroys the specified @class_group.
 **/
void
_matewnck_class_group_destroy (MatewnckClassGroup *class_group)
{
  g_return_if_fail (MATEWNCK_IS_CLASS_GROUP (class_group));

  g_hash_table_remove (class_group_hash, class_group->priv->res_class);

  g_free (class_group->priv->res_class);
  class_group->priv->res_class = NULL;

  /* remove hash's ref on the class group */
  g_object_unref (class_group);
}

static const char *
get_name_from_applications (MatewnckClassGroup *class_group)
{
  const char *first_name;
  GList *l;

  /* Try to get the name from the group leaders.  If all have the same name, we
   * can use that.
   */

  first_name = NULL;

  for (l = class_group->priv->windows; l; l = l->next)
    {
      MatewnckWindow *w;
      MatewnckApplication *app;

      w = MATEWNCK_WINDOW (l->data);
      app = matewnck_window_get_application (w);

      if (!first_name)
	{
	  if (app)
	    first_name = matewnck_application_get_name (app);
	}
      else
	{
	  if (!app || strcmp (first_name, matewnck_application_get_name (app)) != 0)
	    break;
	}
    }

  if (!l)
    {
      /* All names are the same, so use one of them */
      return first_name;
    }
  else
    return NULL;
}

static const char *
get_name_from_windows (MatewnckClassGroup *class_group)
{
  const char *first_name;
  GList *l;

  /* Try to get the name from windows, following the same rationale as
   * get_name_from_applications()
   */

  first_name = NULL;

  for (l = class_group->priv->windows; l; l = l->next)
    {
      MatewnckWindow *window;

      window = MATEWNCK_WINDOW (l->data);

      if (!first_name)
	first_name = matewnck_window_get_name (window);
      else
	if (strcmp (first_name, matewnck_window_get_name (window)) != 0)
	  break;
    }

  if (!l)
    {
      /* All names are the same, so use one of them */
      return first_name;
    }
  else
    return NULL;
}


/* Gets a sensible name for the class group from the application group leaders
 * or from individual windows.
 */
static void
set_name (MatewnckClassGroup *class_group)
{
  const char *new_name;

  if (class_group->priv->name)
    {
      g_free (class_group->priv->name);
      class_group->priv->name = NULL;
    }

  new_name = get_name_from_applications (class_group);

  if (!new_name)
    {
      new_name = get_name_from_windows (class_group);

      if (!new_name)
	new_name = class_group->priv->res_class;
    }

  g_assert (new_name != NULL);

  if (!class_group->priv->name ||
      strcmp (class_group->priv->name, new_name) != 0)
    {
      g_free (class_group->priv->name);
      class_group->priv->name = g_strdup (new_name);

      g_signal_emit (G_OBJECT (class_group), signals[NAME_CHANGED], 0);
    }
}

/* Walks the list of applications, trying to get an icon from them */
static void
get_icons_from_applications (MatewnckClassGroup *class_group, GdkPixbuf **icon, GdkPixbuf **mini_icon)
{
  GList *l;

  *icon = NULL;
  *mini_icon = NULL;

  for (l = class_group->priv->windows; l; l = l->next)
    {
      MatewnckWindow *window;
      MatewnckApplication *app;

      window = MATEWNCK_WINDOW (l->data);
      app = matewnck_window_get_application (window);
      if (app)
	{
	  *icon = matewnck_application_get_icon (app);
	  *mini_icon = matewnck_application_get_mini_icon (app);

	  if (*icon && *mini_icon)
	    return;
	  else
	    {
	      *icon = NULL;
	      *mini_icon = NULL;
	    }
	}
    }
}

/* Walks the list of windows, trying to get an icon from them */
static void
get_icons_from_windows (MatewnckClassGroup *class_group, GdkPixbuf **icon, GdkPixbuf **mini_icon)
{
  GList *l;

  *icon = NULL;
  *mini_icon = NULL;

  for (l = class_group->priv->windows; l; l = l->next)
    {
      MatewnckWindow *window;

      window = MATEWNCK_WINDOW (l->data);

      *icon = matewnck_window_get_icon (window);
      *mini_icon = matewnck_window_get_mini_icon (window);

      if (*icon && *mini_icon)
	return;
      else
	{
	  *icon = NULL;
	  *mini_icon = NULL;
	}
    }
}

/* Gets a sensible icon and mini_icon for the class group from the application
 * group leaders or from individual windows.
 */
static void
set_icon (MatewnckClassGroup *class_group)
{
  GdkPixbuf *icon, *mini_icon;

  get_icons_from_applications (class_group, &icon, &mini_icon);

  if (!icon || !mini_icon)
    get_icons_from_windows (class_group, &icon, &mini_icon);

  if (!icon || !mini_icon)
      _matewnck_get_fallback_icons (&icon,
                                DEFAULT_ICON_WIDTH,
                                DEFAULT_ICON_HEIGHT,
                                &mini_icon,
                                DEFAULT_MINI_ICON_WIDTH,
                                DEFAULT_MINI_ICON_HEIGHT);

  g_assert (icon && mini_icon);

  if (class_group->priv->icon)
    g_object_unref (class_group->priv->icon);

  if (class_group->priv->mini_icon)
    g_object_unref (class_group->priv->mini_icon);

  class_group->priv->icon = g_object_ref (icon);
  class_group->priv->mini_icon = g_object_ref (mini_icon);

  g_signal_emit (G_OBJECT (class_group), signals[ICON_CHANGED], 0);
}

/**
 * _matewnck_class_group_add_window:
 * @class_group: a #MatewnckClassGroup.
 * @window: a #MatewnckWindow.
 *
 * Adds a window to @class_group.  You should only do this if the resource
 * class of the window matches the @class_group<!-- -->'s.
 **/
void
_matewnck_class_group_add_window (MatewnckClassGroup *class_group,
                              MatewnckWindow     *window)
{

  g_return_if_fail (MATEWNCK_IS_CLASS_GROUP (class_group));
  g_return_if_fail (MATEWNCK_IS_WINDOW (window));
  g_return_if_fail (matewnck_window_get_class_group (window) == NULL);

  class_group->priv->windows = g_list_prepend (class_group->priv->windows,
                                               window);
  _matewnck_window_set_class_group (window, class_group);

  set_name (class_group);
  set_icon (class_group);

  /* FIXME: should we monitor class group changes on the window?  The ICCCM says
   * that clients should never change WM_CLASS unless the window is withdrawn.
   */
}

/**
 * _matewnck_class_group_remove_window:
 * @class_group: a #MatewnckClassGroup.
 * @window: a #MatewnckWindow.
 * 
 * Removes a window from the list of windows that are grouped under the
 * specified @class_group.
 **/
void
_matewnck_class_group_remove_window (MatewnckClassGroup *class_group,
				 MatewnckWindow     *window)
{
  g_return_if_fail (MATEWNCK_IS_CLASS_GROUP (class_group));
  g_return_if_fail (MATEWNCK_IS_WINDOW (window));
  g_return_if_fail (matewnck_window_get_class_group (window) == class_group);

  class_group->priv->windows = g_list_remove (class_group->priv->windows,
                                              window);
  _matewnck_window_set_class_group (window, NULL);

  set_name (class_group);
  set_icon (class_group);
}

/**
 * matewnck_class_group_get_windows:
 * @class_group: a #MatewnckClassGroup.
 * 
 * Gets the list of #MatewnckWindow that are grouped in @class_group.
 * 
 * Return value: (element-type MatewnckWindow) (transfer none): the list of
 * #MatewnckWindow grouped in @class_group, or %NULL if the group contains no
 * window. The list should not be modified nor freed, as it is owned by
 * @class_group.
 *
 * Since: 2.2
 **/
GList *
matewnck_class_group_get_windows (MatewnckClassGroup *class_group)
{
  g_return_val_if_fail (class_group != NULL, NULL);

  return class_group->priv->windows;
}

/**
 * matewnck_class_group_get_res_class:
 * @class_group: a #MatewnckClassGroup.
 * 
 * Gets the resource class name for @class_group.
 * 
 * Return value: the resource class name of @class_group, or an
 * empty string if the group has no resource class name.
 *
 * Since: 2.2
 **/
const char *
matewnck_class_group_get_res_class (MatewnckClassGroup *class_group)
{
  g_return_val_if_fail (class_group != NULL, NULL);

  return class_group->priv->res_class;
}

/**
 * matewnck_class_group_get_name:
 * @class_group: a #MatewnckClassGroup.
 * 
 * Gets an human-readable name for @class_group. Since there is no way to
 * properly find this name, a suboptimal heuristic is used to find it. The name
 * is the name of all #MatewnckApplication for each #MatewnckWindow in @class_group if
 * they all have the same name. If all #MatewnckApplication don't have the same
 * name, the name is the name of all #MatewnckWindow in @class_group if they all
 * have the same name. If all #MatewnckWindow don't have the same name, the
 * resource class name is used.
 * 
 * Return value: an human-readable name for @class_group.
 *
 * Since: 2.2
 **/
const char *
matewnck_class_group_get_name (MatewnckClassGroup *class_group)
{
  g_return_val_if_fail (class_group != NULL, NULL);

  return class_group->priv->name;
}

/**
 * matewnck_class_group_get_icon:
 * @class_group: a #MatewnckClassGroup.
 * 
 * Gets the icon to be used for @class_group. Since there is no way to
 * properly find the icon, a suboptimal heuristic is used to find it. The icon
 * is the first icon found by looking at all the #MatewnckApplication for each
 * #MatewnckWindow in @class_group, then at all the #MatewnckWindow in @class_group. If
 * no icon was found, a fallback icon is used.
 * 
 * Return value: the icon for @class_group. The caller should reference the
 * returned <classname>GdkPixbuf</classname> if it needs to keep the icon
 * around.
 *
 * Since: 2.2
 **/
GdkPixbuf *
matewnck_class_group_get_icon (MatewnckClassGroup *class_group)
{
  g_return_val_if_fail (class_group != NULL, NULL);

  return class_group->priv->icon;
}

/**
 * matewnck_class_group_get_mini_icon:
 * @class_group: a #MatewnckClassGroup.
 * 
 * Gets the mini-icon to be used for @class_group. Since there is no way to
 * properly find the mini-icon, the same suboptimal heuristic as the one for
 * matewnck_class_group_get_icon() is used to find it.
 * 
 * Return value: the mini-icon for @class_group. The caller should reference
 * the returned <classname>GdkPixbuf</classname> if it needs to keep the
 * mini-icon around.
 *
 * Since: 2.2
 **/
GdkPixbuf *
matewnck_class_group_get_mini_icon (MatewnckClassGroup *class_group)
{
  g_return_val_if_fail (class_group != NULL, NULL);

  return class_group->priv->mini_icon;
}
