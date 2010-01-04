bin_PROGRAMS = IHM_MX27_GUI

IHM_MX27_GUI_SOURCES =  \
            cb_config.c \
            cb_debug.c  \
            cb_log.c    \
            cb_oper.c   \
            cb_serial.c \
            cb_config.c \
            main.c

ASSEMBLIES = \
        -pkg:glib-2.0 \
        -pkg:gio-2.0  \
        -pkg:gtk-2.0

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = gtk-2.0.pc

IHM_MX27_GUI_LDFLAGS = @LDFLAGS@ @GTK_LIBS@ -lgthread

INCLUDES = @GTK_CFLAGS@
