/**
 * @file gtk/gmqueue.c Message queue for GLib
 *
 * Copyright (C) 2015 Charles E. Lehner
 */

#include <gtk/gtk.h>
#include "gmqueue.h"

struct gmqueue {
	void *arg;
	gmqueue_h *h;
};

struct qitem {
	struct gmqueue *mq;
	void *data;
	int id;
};


/* This handler will be run in the global GMainContext with the GDK lock held.
*/
static gboolean on_idle(gpointer arg)
{
	struct qitem *item = arg;
	struct gmqueue *mq = item->mq;

	mq->h(item->id, item->data, mq->arg);
	g_free(item);

	return G_SOURCE_REMOVE;
}


struct gmqueue *gmqueue_new(gmqueue_h *h, void *arg)
{
	struct gmqueue *mq = g_new(struct gmqueue, 1);
	if (mq) {
		mq->arg = arg;
		mq->h = h;
	}
	return mq;
}


void gmqueue_destroy(struct gmqueue *mq)
{
	g_free(mq);
}


/* gmqueue_push can be run from any thread. The handler will be executed in
 * whichever thread is running GLib's global GMainContext. */
void gmqueue_push(struct gmqueue *gmq, int id, void *data)
{
	struct qitem *item = g_new(struct qitem, 1);
	item->mq = gmq;
	item->id = id;
	item->data = data;
	gdk_threads_add_idle(on_idle, item);
}
