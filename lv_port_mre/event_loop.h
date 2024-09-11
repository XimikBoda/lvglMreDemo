#ifndef EVENT_LOOP_H
#define EVENT_LOOP_H

#include <stdint.h>
#include <stdbool.h>

#define TOUCHPAD_BUF_SIZE (100)
#define KEYBOARD_BUF_SIZE (100)

typedef struct touchpad_event_t {
	int16_t x;
	int16_t y;
	int8_t event;
};

typedef struct touchpad_event_loop_t {
	struct touchpad_event_t buf[TOUCHPAD_BUF_SIZE];
	int rpos, wpos, wpos_p;
};

void touchpad_event_loop_init(struct touchpad_event_loop_t* tp_q);
void touchpad_event_loop_push(struct touchpad_event_loop_t* tp_q, struct touchpad_event_t ev);
struct touchpad_event_t touchpad_event_loop_pop(struct touchpad_event_loop_t* tp_q);
bool touchpad_event_loop_is_available(struct touchpad_event_loop_t* tp_q);

//Keyboard

typedef struct keyboard_event_t {
	int8_t event;
	int8_t keycode;
};

typedef struct keyboard_event_loop_t {
	struct keyboard_event_t buf[KEYBOARD_BUF_SIZE];
	int rpos, wpos, wpos_p;
};

void keyboard_event_loop_init(struct keyboard_event_loop_t* tp_q);
void keyboard_event_loop_push(struct keyboard_event_loop_t* tp_q, struct keyboard_event_t ev);
struct keyboard_event_t keyboard_event_loop_pop(struct keyboard_event_loop_t* tp_q);
bool keyboard_event_loop_is_available(struct keyboard_event_loop_t* tp_q);

#endif