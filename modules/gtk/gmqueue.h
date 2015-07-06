/**
 * @file gtk/gmqueue.h Message queue for GLib
 *
 * Copyright (C) 2015 Charles E. Lehner
 */

struct gmqueue;

typedef void (gmqueue_h)(int id, void *data, void *arg);

struct gmqueue *gmqueue_new(gmqueue_h *h, void *arg);
void gmqueue_destroy(struct gmqueue *gmq);
void gmqueue_push(struct gmqueue *gmq, int id, void *data);

