/*********************
 *      INCLUDES
 *********************/
#include "lv_port_disp_mre.h"
#include <stdbool.h>
#include "vmgraph.h"

 /*********************
  *      DEFINES
  *********************/

#define MY_DISP_HOR_RES 240//(screen_w)
#define MY_DISP_VER_RES 320//(screen_h)

  //#ifndef MY_DISP_HOR_RES
  //    #warning Please define or replace the macro MY_DISP_HOR_RES with the actual screen width, default value 320 is used for now.
  //    #define MY_DISP_HOR_RES    320
  //#endif
  //
  //#ifndef MY_DISP_VER_RES
  //    #warning Please define or replace the macro MY_DISP_VER_RES with the actual screen height, default value 240 is used for now.
  //    #define MY_DISP_VER_RES    240
  //#endif

#define BYTE_PER_PIXEL (LV_COLOR_FORMAT_GET_SIZE(LV_COLOR_FORMAT_RGB565)) /*will be 2 for RGB565 */

/**********************
 *      TYPEDEFS
 **********************/

 /**********************
  *  STATIC PROTOTYPES
  **********************/
static void disp_init(void);

static void disp_flush(lv_display_t* disp, const lv_area_t* area, uint8_t* px_map);

/**********************
 *  STATIC VARIABLES
 **********************/

static VMINT    screen_w = 0;
static VMINT    screen_h = 0;
static VMINT	layer_hdls[2];	// layer handle array. 
static VMUINT8* layer_bufs[2];

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
	lv_display_t* disp = lv_display_create(MY_DISP_HOR_RES, MY_DISP_VER_RES);
	lv_display_set_flush_cb(disp, disp_flush);

	//LV_ATTRIBUTE_MEM_ALIGN
	//static uint8_t buf_3_1[MY_DISP_HOR_RES * MY_DISP_VER_RES * BYTE_PER_PIXEL];

	//LV_ATTRIBUTE_MEM_ALIGN
	//static uint8_t buf_3_2[MY_DISP_HOR_RES * MY_DISP_VER_RES * BYTE_PER_PIXEL];
	lv_display_set_buffers(disp, layer_bufs[0], layer_bufs[1], screen_w * screen_h * 2, LV_DISPLAY_RENDER_MODE_DIRECT);

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

	vm_graphic_set_clip(0, 0, screen_w, screen_h);
}

volatile bool disp_flush_enabled = true;

/* Enable updating the screen (the flushing process) when disp_flush() is called by LVGL
 */
void disp_enable_update(void)
{
	disp_flush_enabled = true;
}

/* Disable updating the screen (the flushing process) when disp_flush() is called by LVGL
 */
void disp_disable_update(void)
{
	disp_flush_enabled = false;
}

/*Flush the content of the internal buffer the specific area on the display.
 *`px_map` contains the rendered image as raw pixel map and it should be copied to `area` on the display.
 *You can use DMA or any hardware acceleration to do this operation in the background but
 *'lv_display_flush_ready()' has to be called when it's finished.*/
static void disp_flush(lv_display_t* disp_drv, const lv_area_t* area, uint8_t* px_map)
{
	if (disp_flush_enabled) {
		/*The most simple case (but also the slowest) to put all pixels to the screen one-by-one*/

		vm_graphic_set_clip(area->x1, area->y1, area->x2, area->y2);
		vm_graphic_flush_layer(layer_hdls + (int)(px_map == layer_bufs[0]), 1);
	}

	/*IMPORTANT!!!
	 *Inform the graphics library that you are ready with the flushing*/
	lv_display_flush_ready(disp_drv);
}