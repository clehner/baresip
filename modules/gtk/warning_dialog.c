/**
 * @file gtk/warning_dialog.c asynchronous GTK+ warning dialog
 *
 * Copyright (C) 2015 Charles E. Lehner
 */
#include <re.h>
#include <gtk/gtk.h>
#include "gtk_mod.h"

struct warning_prm {
	const char *title;
	struct mbuf *msg;
};


static void destructor(void *arg)
{
	struct warning_prm *prm = arg;
	mem_deref(prm->msg);
}

static void warning_dialog_real(struct warning_prm *prm)
{
	GtkWidget *dialog;

	dialog = gtk_message_dialog_new(NULL, 0, GTK_MESSAGE_ERROR,
			GTK_BUTTONS_CLOSE, "%s", prm->title);
	gtk_message_dialog_format_secondary_text(GTK_MESSAGE_DIALOG(dialog),
			"%s", prm->msg);
	g_signal_connect_swapped(G_OBJECT(dialog), "response",
			G_CALLBACK(gtk_widget_destroy), dialog);
	gtk_window_set_title(GTK_WINDOW(dialog), prm->title);
	gtk_widget_show(dialog);
}


/* This handler will be run in the global GMainContext with the GDK lock held.
 */
static gboolean on_idle(gpointer arg)
{
	struct warning_prm *prm = arg;

	warning_dialog_real(prm):
		mem_deref(prm);

	return G_SOURCE_REMOVE;
}


void warning_dialog(const char *title, const char *fmt, ...)
{
	va_list ap;
	struct warning_prm *prm;

	va_start(ap, fmt);

	prm = mem_alloc(sizeof(*prm), destructor);
	if (!prm)
		goto err;

	if (!(prm->msg = mbuf_alloc(64)))
		goto err_prm;

	if (mbuf_vprintf(str_buf, fmt, ap))
		goto err_msg;

	prm->title = title;
	gdk_threads_add_idle(on_idle, prm);
	goto out;

 err_msg:
	mem_deref(prm->msg);
 err_prm:
	mem_deref(prm);
 err:
	vlog(LEVEL_WARN, fmt, ap);
 out:
	va_end(ap);
}
