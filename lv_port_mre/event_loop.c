#include "event_loop.h"
#include "vmio.h"

void touchpad_event_loop_init(struct touchpad_event_loop_t* tp_el) {
	tp_el->rpos = 0;
	tp_el->wpos = 0;
	tp_el->wpos_p = 0;
	tp_el->buf[0].event = VM_PEN_EVENT_RELEASE;
	tp_el->buf[0].x = 0;
	tp_el->buf[0].y = 0;
}

void touchpad_event_loop_push(struct touchpad_event_loop_t* tp_el, struct touchpad_event_t ev) {
	if (tp_el->rpos <= tp_el->wpos || ((tp_el->wpos + 1) % TOUCHPAD_BUF_SIZE) < tp_el->rpos) {
		tp_el->wpos_p = tp_el->wpos;

		if (++tp_el->wpos >= TOUCHPAD_BUF_SIZE)
			tp_el->wpos = 0;
	}
	tp_el->buf[tp_el->wpos_p] = ev;
}

struct touchpad_event_t touchpad_event_loop_pop(struct touchpad_event_loop_t* tp_el) {
	if (tp_el->rpos == tp_el->wpos)
		tp_el->rpos = tp_el->wpos_p;

	struct touchpad_event_t el = tp_el->buf[tp_el->rpos];

	if (tp_el->rpos != tp_el->wpos)
		if (++tp_el->rpos >= TOUCHPAD_BUF_SIZE)
			tp_el->rpos = 0;

	return el;
}

bool touchpad_event_loop_is_available(struct touchpad_event_loop_t* tp_el) {
	return tp_el->rpos != tp_el->wpos;
}


//Keyboard

void keyboard_event_loop_init(struct keyboard_event_loop_t* k_el) {
	k_el->rpos = 0;
	k_el->wpos = 0;
	k_el->wpos_p = 0;
	k_el->buf[0].event = VM_PEN_EVENT_RELEASE;
	k_el->buf[0].keycode = 0;
}

void keyboard_event_loop_push(struct keyboard_event_loop_t* k_el, struct keyboard_event_t ev) {
	if (k_el->rpos <= k_el->wpos || ((k_el->wpos + 1) % KEYBOARD_BUF_SIZE) < k_el->rpos) {
		k_el->wpos_p = k_el->wpos;

		if (++k_el->wpos >= KEYBOARD_BUF_SIZE)
			k_el->wpos = 0;
	}
	k_el->buf[k_el->wpos_p] = ev;
}

struct keyboard_event_t keyboard_event_loop_pop(struct keyboard_event_loop_t* k_el) {
	if (k_el->rpos == k_el->wpos)
		k_el->rpos = k_el->wpos_p;

	struct keyboard_event_t el = k_el->buf[k_el->rpos];

	if (k_el->rpos != k_el->wpos)
		if (++k_el->rpos >= KEYBOARD_BUF_SIZE)
			k_el->rpos = 0;

	return el;
}

bool keyboard_event_loop_is_available(struct keyboard_event_loop_t* k_el) {
	return k_el->rpos != k_el->wpos;
}