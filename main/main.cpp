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
#include "lv_port_indev_mre.h"
#include "lv_port_fs_mre.h"

#include <music/lv_demo_music.h>

void handle_sysevt(VMINT message, VMINT param); // system events \

void tim(int tid) {
	lv_timer_handler();
}

void vm_main(void) {

	lv_init();
	lv_tick_set_cb((lv_tick_get_cb_t)vm_get_tick_count);
	lv_port_disp_init();
	lv_port_indev_init();
	lv_port_fs_init();

	lv_obj_t* file_explorer = lv_file_explorer_create(lv_screen_active());
	lv_file_explorer_open_dir(file_explorer, "e:/Applications");
	//lv_demo_music();
	
	vm_reg_sysevt_callback(handle_sysevt);

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

