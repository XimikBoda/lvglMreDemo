#include "vmsys.h"
#include "vmio.h"
#include "vmgraph.h"
#include "vmchset.h"
#include "vmstdlib.h"
#include "vm4res.h"
#include "vmres.h"
#include "vmtimer.h"
#include "lvgl.h"
#include "lv_port_disp_mre.h"
#include <music/lv_demo_music.h>

VMINT		layer_hdl[1];	// layer handle array. 
VMUINT8* layer_buf = 0;

VMINT screen_w = 0;
VMINT screen_h = 0;
VMUINT8* demo_canvas = 0;

void handle_sysevt(VMINT message, VMINT param); // system events 
void handle_keyevt(VMINT event, VMINT keycode); // key events 
void handle_penevt(VMINT event, VMINT x, VMINT y); // pen events

static void draw_hello(void);

void tim(int tid) {
	lv_timer_handler();
}

void vm_main(void) {
	layer_hdl[0] = -1;
	screen_w = vm_graphic_get_screen_width();
	screen_h = vm_graphic_get_screen_height();

	lv_init();
	lv_tick_set_cb((lv_tick_get_cb_t)vm_get_tick_count);
	lv_port_disp_init();
	lv_demo_music();
	
	vm_reg_sysevt_callback(handle_sysevt);
	vm_reg_keyboard_callback(handle_keyevt);
	vm_reg_pen_callback(handle_penevt);

	vm_create_timer(1, tim);
}

void handle_sysevt(VMINT message, VMINT param) {
	switch (message) {
	case VM_MSG_CREATE:
	case VM_MSG_ACTIVE:
		disp_enable_update();
		break;
		
	case VM_MSG_PAINT:
		break;
		
	case VM_MSG_INACTIVE:
		disp_disable_update();
		break;	
	case VM_MSG_QUIT:
		break;	
	}
}

void handle_keyevt(VMINT event, VMINT keycode) {

}

void handle_penevt(VMINT event, VMINT x, VMINT y) {

}