/*********************
 *      INCLUDES
 *********************/
#include "lv_port_indev_mre.h"
#include "vmio.h"
#include "event_loop.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

static void touchpad_init(void);
static void touchpad_read(lv_indev_t* indev, lv_indev_data_t* data);
static void touchpad_event_handler(VMINT event, VMINT x, VMINT y);
static lv_indev_state_t touchpad_event_mapping(uint8_t event);

static void keypad_init(void);
static void keypad_read(lv_indev_t* indev, lv_indev_data_t* data);
static void keyboard_event(VMINT event, VMINT keycode);
static lv_indev_state_t keyboard_event_mapping(uint8_t event);
static uint32_t keyboard_key_mapping(uint8_t key);

/**********************
 *  STATIC VARIABLES
 **********************/
static lv_indev_t* indev_touchpad;
static lv_indev_t* indev_keypad;

static struct touchpad_event_loop_t touchpad_event_loop;
static struct keyboard_event_loop_t keyboard_event_loop;


/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void lv_port_indev_init(void)
{
	/*------------------
	 * Touchpad
	 * -----------------*/

	 /*Initialize your touchpad if you have*/
	touchpad_init();

	/*Register a touchpad input device*/
	indev_touchpad = lv_indev_create();
	lv_indev_set_type(indev_touchpad, LV_INDEV_TYPE_POINTER);
	lv_indev_set_read_cb(indev_touchpad, touchpad_read);

	/*------------------
	 * Keypad
	 * -----------------*/

	 /*Initialize your keypad or keyboard if you have*/
	keypad_init();

	/*Register a keypad input device*/
	indev_keypad = lv_indev_create();
	lv_indev_set_type(indev_keypad, LV_INDEV_TYPE_KEYPAD);
	lv_indev_set_read_cb(indev_keypad, keypad_read);

	/*Later you should create group(s) with `lv_group_t * group = lv_group_create()`,
	 *add objects to the group with `lv_group_add_obj(group, obj)`
	 *and assign this input device to group to navigate in it:
	 *`lv_indev_set_group(indev_keypad, group);`*/
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

/*------------------
 * Touchpad
 *-----------------*/


  /*Initialize your touchpad*/
static void touchpad_init(void)
{
	vm_reg_pen_callback(touchpad_event_handler);
	touchpad_event_loop_init(&touchpad_event_loop);
}

/*Will be called by the library to read the touchpad*/
static void touchpad_read(lv_indev_t* indev_drv, lv_indev_data_t* data)
{
	struct touchpad_event_t ev = touchpad_event_loop_pop(&touchpad_event_loop);

	data->state = touchpad_event_mapping(ev.event);
	data->point.x = ev.x;
	data->point.y = ev.y;
	data->continue_reading = touchpad_event_loop_is_available(&touchpad_event_loop);
}

static void touchpad_event_handler(VMINT event, VMINT x, VMINT y) 
{
	struct touchpad_event_t ev;

	ev.event = event;
	ev.x = x;
	ev.y = y;

	touchpad_event_loop_push(&touchpad_event_loop, ev);
}

static lv_indev_state_t touchpad_event_mapping(uint8_t event) {
	switch (event)
	{
	case VM_PEN_EVENT_TAP:
	case VM_PEN_EVENT_MOVE:
	case VM_PEN_EVENT_LONG_TAP:
	case VM_PEN_EVENT_DOUBLE_CLICK:
	case VM_PEN_EVENT_REPEAT:
		return LV_INDEV_STATE_PRESSED;
		break;
	case VM_PEN_EVENT_ABORT:
	case VM_PEN_EVENT_RELEASE:
	default:
		return LV_INDEV_STATE_RELEASED;
		break;
	}
}

/*------------------
 * Keypad
 * -----------------*/

 /*Initialize your keypad*/
static void keypad_init(void)
{
	vm_reg_keyboard_callback(keyboard_event);
	keyboard_event_loop_init(&keyboard_event_loop);
}

/*Will be called by the library to read the mouse*/ // What?
static void keypad_read(lv_indev_t* indev_drv, lv_indev_data_t* data)
{
	struct keyboard_event_t ev = keyboard_event_loop_pop(&keyboard_event_loop);

	data->state = keyboard_event_mapping(ev.event);
	data->key = keyboard_key_mapping(ev.keycode);
	data->continue_reading = keyboard_event_loop_is_available(&keyboard_event_loop);
}

static void keyboard_event(VMINT event, VMINT keycode) {
	struct keyboard_event_t ev;

	ev.event = event;
	ev.keycode = keycode;

	keyboard_event_loop_push(&keyboard_event_loop, ev);
}

static lv_indev_state_t keyboard_event_mapping(int8_t event) {
	switch (event) {
	case VM_KEY_EVENT_DOWN:
	case VM_KEY_EVENT_LONG_PRESS:
	case VM_KEY_EVENT_REPEAT:
		return LV_INDEV_STATE_PRESSED;
		break;
	case VM_KEY_EVENT_UP:
		return LV_INDEV_STATE_RELEASED;
		break;
	}
}

static uint32_t keyboard_key_mapping(int8_t key) {
	switch (key) {
	case VM_KEY_UP:
		return LV_KEY_UP;
		break;
	case VM_KEY_DOWN:
		return LV_KEY_DOWN;
		break;
	case VM_KEY_LEFT:
		return LV_KEY_LEFT;
		break;
	case VM_KEY_RIGHT:
		return LV_KEY_RIGHT;
		break;
	case VM_KEY_OK:
		return LV_KEY_ENTER;
		break;
	case VM_KEY_RIGHT_SOFTKEY:
		return LV_KEY_BACKSPACE;
		break;
	case VM_KEY_LEFT_SOFTKEY:
		return LV_KEY_ESC;
		break;
	case VM_KEY_NUM6:
		return LV_KEY_NEXT;
		break;
	case VM_KEY_NUM4:
		return LV_KEY_PREV;
		break;
	case VM_KEY_NUM1:
		return LV_KEY_HOME;
		break;
	case VM_KEY_NUM7:
		return LV_KEY_END;
		break;
	case VM_KEY_NUM3:
		return LV_KEY_DEL;
		break;
	}
}