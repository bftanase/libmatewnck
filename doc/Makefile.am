## Process this file with automake to produce Makefile.in

# The name of the module.
DOC_MODULE=libmatewnck

# The top-level SGML file.
DOC_MAIN_SGML_FILE=libmatewnck-docs.sgml

# The directory containing the source code. Relative to $(srcdir)
DOC_SOURCE_DIR=../libmatewnck

# Extra options to pass to gtkdoc-scangobj
SCANGOBJ_OPTIONS=--nogtkinit --type-init-func="g_type_init ()"

# Extra options to supply to gtkdoc-scan
SCAN_OPTIONS=--deprecated-guards="MATEWNCK_DISABLE_DEPRECATED"

# Extra options to supply to gtkdoc-mkdb
MKDB_OPTIONS=--sgml-mode --output-format=xml

# Extra options to supply to gtkdoc-fixref
FIXXREF_OPTIONS=

# Used for dependencies
HFILE_GLOB=$(top_srcdir)/libmatewnck/*.h
CFILE_GLOB=$(top_srcdir)/libmatewnck/*.c

# Header files to ignore when scanning
IGNORE_HFILES=

EXTRA_HFILES = 

# Images to copy into HTML directory
HTML_IMAGES =

# Extra SGML files that are included by $(DOC_MAIN_SGML_FILE)
content_files =

# Other files to distribute
extra_files =

# CFLAGS and LDFLAGS for compiling scan program. Only needed
# if $(DOC_MODULE).types is non-empty.
GTKDOC_CFLAGS = -I$(top_builddir) -I$(top_srcdir) $(LIBMATEWNCK_CFLAGS)
GTKDOC_LIBS = $(top_builddir)/libmatewnck/libmatewnck.la $(LIBMATEWNCK_LIBS)

include $(top_srcdir)/gtk-doc.make

CLEANFILES += gsf-scan.*

.PHONY : dist-hook-local

-include $(top_srcdir)/git.mk
