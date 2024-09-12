/*********************
 *      INCLUDES
 *********************/
#include "lv_port_fs_mre.h"
#include "vmio.h"
#include "vmchset.h"
#include "vmstdlib.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

typedef struct dir_t{
    VMINT handle;
    struct vm_fileinfo_ext prev;
    VMBOOL last;
};

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void fs_init(void);

static void * fs_open(lv_fs_drv_t * drv, const char * path, lv_fs_mode_t mode);
static lv_fs_res_t fs_close(lv_fs_drv_t * drv, void * file_p);
static lv_fs_res_t fs_read(lv_fs_drv_t * drv, void * file_p, void * buf, uint32_t btr, uint32_t * br);
static lv_fs_res_t fs_write(lv_fs_drv_t * drv, void * file_p, const void * buf, uint32_t btw, uint32_t * bw);
static lv_fs_res_t fs_seek(lv_fs_drv_t * drv, void * file_p, uint32_t pos, lv_fs_whence_t whence);
static lv_fs_res_t fs_size(lv_fs_drv_t * drv, void * file_p, uint32_t * size_p);
static lv_fs_res_t fs_tell(lv_fs_drv_t * drv, void * file_p, uint32_t * pos_p);

static void * fs_dir_open(lv_fs_drv_t * drv, const char * path);
static lv_fs_res_t fs_dir_read(lv_fs_drv_t * drv, void * rddir_p, char * fn, uint32_t fn_len);
static lv_fs_res_t fs_dir_close(lv_fs_drv_t * drv, void * rddir_p);

static void* mre_file_to_p(VMFILE file);
static VMFILE* mre_file_from_p(void* file);

static void fix_path(VMWSTR path);

/**********************
 *  STATIC VARIABLES
 **********************/

const static VMWCHAR find_suf[5] = { '\\', '*', '.', '*', '\0'};

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void lv_port_fs_init(void)
{
    /*----------------------------------------------------
     * Initialize your storage device and File System
     * -------------------------------------------------*/
    fs_init();

    /*---------------------------------------------------
     * Register the file system interface in LVGL
     *--------------------------------------------------*/

    static lv_fs_drv_t fs_drv;
    lv_fs_drv_init(&fs_drv);

    /*Set up fields...*/
    fs_drv.letter = 'e';
    fs_drv.open_cb = fs_open;
    fs_drv.close_cb = fs_close;
    fs_drv.read_cb = fs_read;
    fs_drv.write_cb = fs_write;
    fs_drv.seek_cb = fs_seek;
    fs_drv.tell_cb = fs_tell;

    fs_drv.dir_close_cb = fs_dir_close;
    fs_drv.dir_open_cb = fs_dir_open;
    fs_drv.dir_read_cb = fs_dir_read;

    lv_fs_drv_register(&fs_drv);
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

/*Initialize your Storage device and File system.*/
static void fs_init(void)
{
    /*E.g. for FatFS initialize the SD card and FatFS itself*/

    /*You code here*/
}

/**
 * Open a file
 * @param drv       pointer to a driver where this function belongs
 * @param path      path to the file beginning with the driver letter (e.g. S:/folder/file.txt)
 * @param mode      read: FS_MODE_RD, write: FS_MODE_WR, both: FS_MODE_RD | FS_MODE_WR
 * @return          a file descriptor or NULL on error
 */
static void * fs_open(lv_fs_drv_t * drv, const char * path, lv_fs_mode_t mode)
{
    VMWCHAR wpath[MAX_APP_NAME_LEN] = { drv->letter, ':' };

    vm_ascii_to_ucs2(wpath + 2, MAX_APP_NAME_LEN * 2 - 4, path);

    vm_ascii_to_ucs2(wpath, MAX_APP_NAME_LEN*2, path);

    fix_path(wpath); // change / to \

    VMUINT mre_mode = 0;

    if(mode == LV_FS_MODE_WR) {
        mre_mode = MODE_CREATE_ALWAYS_WRITE; // or MODE_WRITE
    }
    else if(mode == LV_FS_MODE_RD) {
        mre_mode = MODE_READ;
    }
    else if(mode == (LV_FS_MODE_WR | LV_FS_MODE_RD)) {
        mre_mode = MODE_WRITE | MODE_READ;
    }

    VMFILE f = vm_file_open(wpath, mre_mode, 1);

    if (f < 0)
        return 0;
    else
        return mre_file_to_p(f);
}

/**
 * Close an opened file
 * @param drv       pointer to a driver where this function belongs
 * @param file_p    pointer to a file_t variable. (opened with fs_open)
 * @return          LV_FS_RES_OK: no error or  any error from @lv_fs_res_t enum
 */
static lv_fs_res_t fs_close(lv_fs_drv_t * drv, void * file_p)
{
    vm_file_close(mre_file_from_p(file_p));

    return LV_FS_RES_OK;
}

/**
 * Read data from an opened file
 * @param drv       pointer to a driver where this function belongs
 * @param file_p    pointer to a file_t variable.
 * @param buf       pointer to a memory block where to store the read data
 * @param btr       number of Bytes To Read
 * @param br        the real number of read bytes (Byte Read)
 * @return          LV_FS_RES_OK: no error or  any error from @lv_fs_res_t enum
 */
static lv_fs_res_t fs_read(lv_fs_drv_t * drv, void * file_p, void * buf, uint32_t btr, uint32_t * br)
{
    VMINT res = vm_file_read(mre_file_from_p(file_p), buf, btr, br);

    if(res > 0)
        return LV_FS_RES_OK;
    else
        return LV_FS_RES_UNKNOWN;
}

/**
 * Write into a file
 * @param drv       pointer to a driver where this function belongs
 * @param file_p    pointer to a file_t variable
 * @param buf       pointer to a buffer with the bytes to write
 * @param btw       Bytes To Write
 * @param bw        the number of real written bytes (Bytes Written). NULL if unused.
 * @return          LV_FS_RES_OK: no error or  any error from @lv_fs_res_t enum
 */
static lv_fs_res_t fs_write(lv_fs_drv_t * drv, void * file_p, const void * buf, uint32_t btw, uint32_t * bw)
{
    VMINT res = vm_file_write(mre_file_from_p(file_p), buf, btw, bw);

    if (res > 0 || btw == 0)
        return LV_FS_RES_OK;
    else
        return LV_FS_RES_UNKNOWN;
}

/**
 * Set the read write pointer. Also expand the file size if necessary.
 * @param drv       pointer to a driver where this function belongs
 * @param file_p    pointer to a file_t variable. (opened with fs_open )
 * @param pos       the new position of read write pointer
 * @param whence    tells from where to interpret the `pos`. See @lv_fs_whence_t
 * @return          LV_FS_RES_OK: no error or  any error from @lv_fs_res_t enum
 */
static lv_fs_res_t fs_seek(lv_fs_drv_t * drv, void * file_p, uint32_t pos, lv_fs_whence_t whence)
{
    VMINT base = 0;
    switch (whence)
    {
    case LV_FS_SEEK_SET:
        base = BASE_BEGIN;
        break;
    case LV_FS_SEEK_CUR:
        base = BASE_CURR;
        break;
    case LV_FS_SEEK_END:
        base = BASE_END;
        break;
    default:
        break;
    }

    VMINT res = vm_file_seek(mre_file_from_p(file_p), pos, base);

    if (res == 0)
        return LV_FS_RES_OK;
    else
        return LV_FS_RES_UNKNOWN;
}
/**
 * Give the position of the read write pointer
 * @param drv       pointer to a driver where this function belongs
 * @param file_p    pointer to a file_t variable
 * @param pos_p     pointer to store the result
 * @return          LV_FS_RES_OK: no error or  any error from @lv_fs_res_t enum
 */
static lv_fs_res_t fs_tell(lv_fs_drv_t * drv, void * file_p, uint32_t * pos_p)
{
    VMINT res = vm_file_tell(mre_file_from_p(file_p));

    if (res >= 0) {
        *pos_p = res;
        return LV_FS_RES_OK;
    }
    else
        return LV_FS_RES_UNKNOWN;
}

/**
 * Initialize a 'lv_fs_dir_t' variable for directory reading
 * @param drv       pointer to a driver where this function belongs
 * @param path      path to a directory
 * @return          pointer to the directory read descriptor or NULL on error
 */
static void * fs_dir_open(lv_fs_drv_t * drv, const char * path)
{
    struct dir_t *dir = (struct dir_t*)vm_malloc(sizeof(struct dir_t));

    VMWCHAR wpath[MAX_APP_NAME_LEN] = { drv->letter, ':'};

    vm_ascii_to_ucs2(wpath + 2, MAX_APP_NAME_LEN * 2 - 4, path);
    vm_wstrcat(wpath, find_suf);

    fix_path(wpath);

    dir->handle = vm_find_first_ext(wpath, &dir->prev);
    dir->last = VM_FALSE;

    
    if (dir->handle < 0) {
        vm_free(dir);
        return 0;
    }

    return dir;
}

/**
 * Read the next filename form a directory.
 * The name of the directories will begin with '/'
 * @param drv       pointer to a driver where this function belongs
 * @param rddir_p   pointer to an initialized 'lv_fs_dir_t' variable
 * @param fn        pointer to a buffer to store the filename
 * @param fn_len    length of the buffer to store the filename
 * @return          LV_FS_RES_OK: no error or  any error from @lv_fs_res_t enum
 */
static lv_fs_res_t fs_dir_read(lv_fs_drv_t * drv, void * rddir_p, char * fn, uint32_t fn_len)
{
    struct dir_t* dir = rddir_p;

    if (dir->last)
        return LV_FS_RES_UNKNOWN;

    if (dir->prev.attributes & VM_FS_ATTR_DIR) {
        fn[0] = '/';
        fn++, fn_len--;
    }

    vm_ucs2_to_ascii(fn, fn_len, dir->prev.filefullname);

    VMINT res = vm_find_next_ext(dir->handle, &dir->prev);
    if (res < 0)
        dir->last = VM_TRUE;

    return LV_FS_RES_OK;
}

/**
 * Close the directory reading
 * @param drv       pointer to a driver where this function belongs
 * @param rddir_p   pointer to an initialized 'lv_fs_dir_t' variable
 * @return          LV_FS_RES_OK: no error or  any error from @lv_fs_res_t enum
 */
static lv_fs_res_t fs_dir_close(lv_fs_drv_t * drv, void * rddir_p)
{
    struct dir_t* dir = rddir_p;

    vm_find_close(dir->handle);

    vm_free(dir);

    return LV_FS_RES_OK;
}

static void* mre_file_to_p(VMFILE file) {
    return (void*)(file + 1);
}

static VMFILE* mre_file_from_p(void* file) {
    return ((VMFILE)file) - 1;
}

static void fix_path(VMWSTR path) {
    for(; *path; ++path)
        if (*path == '/')
            *path = '\\';
}