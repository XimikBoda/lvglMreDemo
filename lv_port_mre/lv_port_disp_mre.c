/*********************
 *      INCLUDES
 *********************/
#include "lv_port_disp_mre.h"
#include <stdbool.h>
#include "vmgraph.h"

/*********************
 *      DEFINES
 *********************/

#define BYTE_PER_PIXEL (LV_COLOR_FORMAT_GET_SIZE(LV_COLOR_FORMAT_RGB565)) /*will be 2 for RGB565 */

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void disp_init(void);

static void disp_flush(lv_display_t* disp, const lv_area_t* area, uint8_t* px_map);

static void full_flush_timer(lv_timer_t* timer);

/**********************
 *  STATIC VARIABLES
 **********************/

static VMINT    screen_w = 0;
static VMINT    screen_h = 0;
static VMINT	layer_hdls[2];	// layer handle array. 
static VMUINT8* layer_bufs[2];

static lv_timer_t* display_timer;

static bool need_to_flush = false;

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void lv_port_disp_init(void)
{
	/*-------------------------
	 * Initialize your display
	 * -----------------------*/
	disp_init();

	/*------------------------------------
	 * Create a display and set a flush_cb
	 * -----------------------------------*/
	lv_display_t* disp = lv_display_create(screen_w, screen_h);
	lv_display_set_flush_cb(disp, disp_flush);

	lv_display_set_buffers(disp, layer_bufs[1], 0, screen_w * screen_h * 2, LV_DISPLAY_RENDER_MODE_DIRECT);
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

 /*Initialize your display and the required peripherals.*/
static void disp_init(void)
{
	screen_w = vm_graphic_get_screen_width();
	screen_h = vm_graphic_get_screen_height();

	for (int i = 0; i < 2; ++i) {
		layer_hdls[i] = vm_graphic_create_layer(0, 0, screen_w, screen_h, -1);
		layer_bufs[i] = vm_graphic_get_layer_buffer(layer_hdls[i]);
	}

	vm_graphic_active_layer(layer_hdls[0]);

	display_timer = lv_timer_create(full_flush_timer, 33, 0);
}

/* Enable updating the screen (the flushing process) when disp_flush() is called by LVGL
 */
void disp_enable_update(void)
{
	lv_timer_resume(display_timer);
}

/* Disable updating the screen (the flushing process) when disp_flush() is called by LVGL
 */
void disp_disable_update(void)
{
	lv_timer_pause(display_timer);
}

/*Flush the content of the internal buffer the specific area on the display.
 *`px_map` contains the rendered image as raw pixel map and it should be copied to `area` on the display.
 *You can use DMA or any hardware acceleration to do this operation in the background but
 *'lv_display_flush_ready()' has to be called when it's finished.*/
static void disp_flush(lv_display_t* disp_drv, const lv_area_t* area, uint8_t* px_map)
{
	// We already in right buffer
	need_to_flush = true;

	/*IMPORTANT!!!
	 *Inform the graphics library that you are ready with the flushing*/
	lv_display_flush_ready(disp_drv);
}

static void full_flush_timer(lv_timer_t* timer) {
	if (need_to_flush) {
		memcpy(layer_bufs[0], layer_bufs[1], screen_w * screen_h * 2);
		//vm_graphic_flatten_layer_ex(layer_hdls, 2);			// Copy second buffer to first
		vm_graphic_flush_layer_non_block(layer_hdls, 1);	// Async copy to display 
		need_to_flush = false;
	}
}