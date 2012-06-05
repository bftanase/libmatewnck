/* selector */
/* vim: set sw=2 et: */

/*
 * Copyright (C) 2001 Havoc Pennington
 * Copyright (C) 2005-2007 Vincent Untz
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
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */

#ifndef MATEWNCK_SELECTOR_H
#define MATEWNCK_SELECTOR_H

#include <gtk/gtk.h>

G_BEGIN_DECLS
#define MATEWNCK_TYPE_SELECTOR              (matewnck_selector_get_type ())
#define MATEWNCK_SELECTOR(object)           (G_TYPE_CHECK_INSTANCE_CAST ((object), MATEWNCK_TYPE_SELECTOR, MatewnckSelector))
#define MATEWNCK_SELECTOR_CLASS(klass)      (G_TYPE_CHECK_CLASS_CAST ((klass), MATEWNCK_TYPE_SELECTOR, MatewnckSelectorClass))
#define MATEWNCK_IS_SELECTOR(object)        (G_TYPE_CHECK_INSTANCE_TYPE ((object), MATEWNCK_TYPE_SELECTOR))
#define MATEWNCK_IS_SELECTOR_CLASS(klass)   (G_TYPE_CHECK_CLASS_TYPE ((klass), MATEWNCK_TYPE_SELECTOR))
#define MATEWNCK_SELECTOR_GET_CLASS(obj)    (G_TYPE_INSTANCE_GET_CLASS ((obj), MATEWNCK_TYPE_SELECTOR, MatewnckSelectorClass))
typedef struct _MatewnckSelector MatewnckSelector;
typedef struct _MatewnckSelectorClass MatewnckSelectorClass;
typedef struct _MatewnckSelectorPrivate MatewnckSelectorPrivate;

/**
 * MatewnckSelector:
 *
 * The #MatewnckSelector struct contains only private fields and should not be
 * directly accessed.
 */
struct _MatewnckSelector
{
  GtkMenuBar parent_instance;
  MatewnckSelectorPrivate *priv;
};

struct _MatewnckSelectorClass 
{
  GtkMenuBarClass parent_class;
  
  /* Padding for future expansion */
  void (* pad1) (void);
  void (* pad2) (void);
  void (* pad3) (void);
  void (* pad4) (void);
};

GtkWidget *matewnck_selector_new      (void);
GType      matewnck_selector_get_type (void) G_GNUC_CONST;

G_END_DECLS
#endif /* MATEWNCK_SELECTOR_H */
