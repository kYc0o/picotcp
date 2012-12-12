/*********************************************************************
PicoTCP. Copyright (c) 2012 TASS Belgium NV. Some rights reserved.
See LICENSE and COPYING for usage.
Do not redistribute without a written permission by the Copyright
holders.

Authors: Daniele Lacamera
*********************************************************************/


#include "pico_protocol.h"
#include "rb.h"

RB_HEAD(pico_protocol_tree, pico_protocol);
RB_PROTOTYPE_STATIC(pico_protocol_tree, pico_protocol, node, pico_proto_cmp);

static struct pico_protocol_tree Datalink_proto_tree;
static struct pico_protocol_tree Network_proto_tree;
static struct pico_protocol_tree Transport_proto_tree;
static struct pico_protocol_tree Socket_proto_tree;


static int pico_proto_cmp(struct pico_protocol *a, struct pico_protocol *b)
{
  if (a->hash < b->hash)
    return -1;
  if (a->hash > b->hash)
    return 1;
  return 0;
}

RB_GENERATE_STATIC(pico_protocol_tree, pico_protocol, node, pico_proto_cmp);

static int proto_loop(struct pico_protocol *proto, int loop_score)
{
  struct pico_frame *f;
  while(loop_score >0) {
    if (proto->q_in->frames + proto->q_out->frames <= 0)
      break;

    f = pico_dequeue(proto->q_out);
    if ((f) &&(proto->process_out(proto, f) > 0)) {
      loop_score--;
    }

    f = pico_dequeue(proto->q_in);
    if ((f) &&(proto->process_in(proto, f) > 0)) {
      loop_score--;
    }
  }
  return loop_score;
}

int pico_protocols_loop(int loop_score)
{
  struct pico_protocol *p;
  RB_FOREACH(p, pico_protocol_tree, &Datalink_proto_tree) {
    loop_score = proto_loop(p, loop_score);
    if (loop_score < 1)
      return 0;
  }
  RB_FOREACH(p, pico_protocol_tree, &Network_proto_tree) {
    loop_score = proto_loop(p, loop_score);
    if (loop_score < 1)
      return 0;
  }
  RB_FOREACH(p, pico_protocol_tree, &Transport_proto_tree) {
    loop_score = proto_loop(p, loop_score);
    if (loop_score < 1)
      return 0;
  }
  RB_FOREACH(p, pico_protocol_tree, &Socket_proto_tree) {
    loop_score = proto_loop(p, loop_score);
    if (loop_score < 1)
      return 0;
  }
  return loop_score;
}

void pico_protocol_init(struct pico_protocol *p)
{
  if (!p)
    return;

  p->hash = pico_hash(p->name);
  switch (p->layer) {
    case PICO_LAYER_DATALINK:
      RB_INSERT(pico_protocol_tree, &Datalink_proto_tree, p);
      break;
    case PICO_LAYER_NETWORK:
      RB_INSERT(pico_protocol_tree, &Network_proto_tree, p);
      break;
    case PICO_LAYER_TRANSPORT:
      RB_INSERT(pico_protocol_tree, &Transport_proto_tree, p);
      break;
    case PICO_LAYER_SOCKET:
      RB_INSERT(pico_protocol_tree, &Socket_proto_tree, p);
      break;
  }
  dbg("Protocol %s registered (layer: %d).\n", p->name, p->layer);

}

