/******************************************************************************
 *                  SM64Net - An Internet framework for SM64                  *
 *                   Copyright (C) 2019, 2020  devwizard                      *
 *      This project is licensed under the GNU General Public License         *
 *      version 2.  See LICENSE for more information.                         *
 ******************************************************************************/

#ifndef _SM64_H_
#define _SM64_H_

#include "types.h"

#define A_BUTTON        0x8000
#define B_BUTTON        0x4000
#define Z_TRIG          0x2000
#define START_BUTTON    0x1000
#define U_JPAD          0x0800
#define D_JPAD          0x0400
#define L_JPAD          0x0200
#define R_JPAD          0x0100
#define L_TRIG          0x0020
#define R_TRIG          0x0010
#define U_CBUTTONS      0x0008
#define D_CBUTTONS      0x0004
#define L_CBUTTONS      0x0002
#define R_CBUTTONS      0x0001

#define GFX_TYPE_GFX            0x000A
#define GFX_TYPE_DL_BILLBOARD   0x001A
#define GFX_TYPE_DL_SCALE       0x001C
#define GFX_TYPE_CALLBACK       0x012A

#define GFX_FLAG_ENABLED        0x0001

#define GFX_RM_BG               0x00
#define GFX_RM_OPA_SURF         0x01
#define GFX_RM_OPA_DECAL        0x02
#define GFX_RM_OPA_INTER        0x03
#define GFX_RM_SPRITE           0x04
#define GFX_RM_XLU_SURF         0x05
#define GFX_RM_XLU_DECAL        0x06
#define GFX_RM_XLU_INTER        0x07

enum
{
    /* 0x00 */  PLAYER_GFX_EYES_BLINK,
    /* 0x01 */  PLAYER_GFX_EYES_OPEN,
    /* 0x02 */  PLAYER_GFX_EYES_HALF,
    /* 0x03 */  PLAYER_GFX_EYES_CLOSED,
    /* 0x04 */  PLAYER_GFX_EYES_LEFT,
    /* 0x05 */  PLAYER_GFX_EYES_RIGHT,
    /* 0x06 */  PLAYER_GFX_EYES_UP,
    /* 0x07 */  PLAYER_GFX_EYES_DOWN,
    /* 0x08 */  PLAYER_GFX_EYES_DEAD,
};

enum
{
    /* 0x00 */  PLAYER_GFX_CAP_NONE,
    /* 0x01 */  PLAYER_GFX_CAP_VANISH,
    /* 0x02 */  PLAYER_GFX_CAP_METAL,
    /* 0x03 */  PLAYER_GFX_CAP_VANISH_METAL,
};

typedef union
{
    struct
    {
        u32 w0;
        u32 w1;
    }
    w;
    u64 d;
}
Gfx;

struct pad_t
{
    /* 0x00 */  s16     stick_x;
    /* 0x02 */  s16     stick_y;
    /* 0x04 */  f32     stick[2];
    /* 0x0C */  f32     stick_mag;
    /* 0x10 */  u16     held;
    /* 0x12 */  u16     down;
    /* 0x14 */  void   *pad;
    /* 0x18 */  void   *status;
};  /* 0x1C */

struct heap_t
{
    /* 0x00 */  size_t  size;
    /* 0x04 */  size_t  used;
    /* 0x08 */  u8     *start;
    /* 0x0C */  u8     *free;
};  /* 0x10 */

struct camera_t
{
    /* 0x00 */  u8      mode;
};

struct motion_t
{
    /* 0x00 */  s16     flag;
    /* 0x02 */  s16     height;
    /* 0x04 */  s16     start;
    /* 0x06 */  s16     end;
    /* 0x08 */  s16     frames;
    /* 0x0A */  s16     joints;
    /* 0x0C */  s16    *val;
    /* 0x10 */  u16    *tbl;
    /* 0x14 */  size_t  size;
};  /* 0x18 */

struct gfx_t
{
    /* 0x00 */  s16     type;
    /* 0x02 */  s16     flag;
    /* 0x04 */  struct gfx_t *prev;
    /* 0x08 */  struct gfx_t *next;
    /* 0x0C */  struct gfx_t *parent;
    /* 0x10 */  struct gfx_t *child;
};  /* 0x14 */

struct gfx_callback_t
{
    /* 0x00 */  struct gfx_t gfx;
    /* 0x14 */  Gfx  *(*callback)(s32, struct gfx_callback_t *, f32[4][4]);
    /* 0x18 */  s32     arg;
};  /* 0x1C */

struct gfx_dl_t
{
    /* 0x00 */  struct gfx_t gfx;
    /* 0x14 */  Gfx    *dl;
};  /* 0x18 */

struct gfx_dl_billboard_t
{
    /* 0x00 */  struct gfx_dl_t gfx;
    /* 0x18 */  s16     pos[3];
};  /* 0x1E */

struct gfx_dl_scale_t
{
    /* 0x00 */  struct gfx_dl_t gfx;
    /* 0x18 */  f32     scale;
};  /* 0x1C */

struct gfx_object_motion_t
{
    /* 0x00 0x38 */ s16     index;
    /* 0x02 0x3A */ s16     height;
    /* 0x04 0x3C */ struct motion_t *motion;
    /* 0x08 0x40 */ s16     frame;
    /* 0x0A 0x42 */ u16     timer;
    /* 0x0C 0x44 */ s32     frame_amt;
    /* 0x10 0x48 */ s32     frame_vel;
};  /* 0x14 0x4C */

struct gfx_object_t
{
    /* 0x00 */  struct gfx_t gfx;
    /* 0x14 */  struct gfx_t *list;
    /* 0x18 */  s8      world_index;
    /* 0x19 */  s8      gfx_index;
    /* 0x1A */  s16     rot[3];
    /* 0x20 */  f32     pos[3];
    /* 0x2C */  f32     scale[3];
    /* 0x38 */  struct gfx_object_motion_t motion;
    /* 0x4C */  void   *_4C;
    /* 0x50 */  f32   (*mtxf)[4][4];
    /* 0x54 */  f32     pos_sfx[3];
};  /* 0x60 */

struct gfx_camera_t
{
    /* 0x00 */  struct gfx_callback_t gfx;
    /* 0x1C */  f32     pos[3];
    /* 0x28 */  f32     obj[3];
    /* 0x34 */  f32   (*mtxf)[4][4];
    /* 0x38 */  s16     rot_z_m;
    /* 0x3A */  s16     rot_z_p;
};  /* 0x3C */

struct object_t
{
    /* 0x0000 */    struct gfx_object_t gfx;
    /* 0x0060 */    struct object_t *next;
    /* 0x0064 */    struct object_t *prev;
    /* 0x0068 */    struct object_t *parent;
    /* 0x006C */    struct object_t *child;
    /* 0x0070 */    u32     touch;
    /* 0x0074 */    s16     flag;
    /* 0x0076 */    s16     object_touch_len;
    /* 0x0078 */    struct object_t *object_touch[4];
    /* 0x0088 */    union
                    {
                        s8      s8[320/sizeof(s8)];
                        u8      u8[320/sizeof(u8)];
                        s16     s16[320/sizeof(s16)];
                        u16     u16[320/sizeof(u16)];
                        s32     s32[320/sizeof(s32)];
                        u32     u32[320/sizeof(u32)];
                        void   *ptr[320/sizeof(void *)];
                    }
                    mem;
    /* 0x01C8 */    void   *_1C8;
    /* 0x01CC */    uintptr_t *pc;
    /* 0x01D0 */    uintptr_t stack_index;
    /* 0x01D4 */    uintptr_t stack[8];
    /* 0x01F4 */    s16     _1F4;
    /* 0x01F6 */    s16     _1F6;
    /* 0x01F8 */    f32     _1F8;
    /* 0x01FC */    f32     _1FC;
    /* 0x0200 */    f32     _200;
    /* 0x0204 */    f32     _204;
    /* 0x0208 */    f32     _208;
    /* 0x020C */    uintptr_t *script;
    /* 0x0210 */    struct object_t *_210;
    /* 0x0214 */    struct object_t *_214;
    /* 0x0218 */    s16    *_218;
    /* 0x021C */    f32     mtxf[4][4];
    /* 0x025C */    void   *_25C;
};  /* 0x0260 */

struct player_gfx_t
{
    /* 0x00 */  u32     state;
    /* 0x04 */  s8      head;
    /* 0x05 */  s8      eyes;
    /* 0x06 */  s8      gloves;
    /* 0x07 */  s8      wings;
    /* 0x08 */  s16     cap;
    /* 0x0A */  s8      hold;
    /* 0x0B */  u8      punch;
    /* 0x0C */  s16     rot_torso[3];
    /* 0x12 */  s16     rot_head[3];
    /* 0x18 */  f32     pos_glove[3];
    /* 0x24 */  struct object_t *object;
};  /* 0x28 */

struct player_t
{
    /* 0x00 */  u16     index;
    /* 0x02 */  u16     event;
    /* 0x04 */  u32     flag;
    /* 0x08 */  u32     particle;
    /* 0x0C */  u32     state;
    /* 0x10 */  u32     state_prev;
    /* 0x14 */  u32     ground_sfx;
    /* 0x18 */  s16     state_mode;
    /* 0x1A */  u16     state_timer;
    /* 0x1C */  u32     state_arg;
    /* 0x20 */  f32     stick_mag;
    /* 0x24 */  s16     stick_rot;
    /* 0x26 */  s16     timer_invincible;
    /* 0x28 */  u8      timer_a_down;
    /* 0x29 */  u8      timer_b_down;
    /* 0x2A */  u8      timer_col_wall;
    /* 0x2B */  u8      timer_col_floor;
    /* 0x2C */  s16     rot[3];
    /* 0x32 */  s16     rot_vel[3];
    /* 0x38 */  s16     rot_slide;
    /* 0x3A */  s16     rot_twirl;
    /* 0x3C */  f32     pos[3];
    /* 0x48 */  f32     vel[3];
    /* 0x54 */  f32     vel_f;
    /* 0x58 */  f32     vel_h[2];
    /* 0x60 */  void   *wall;
    /* 0x64 */  void   *roof;
    /* 0x68 */  void   *ground;
    /* 0x6C */  f32     height_roof;
    /* 0x70 */  f32     height_ground;
    /* 0x74 */  s16     rot_ground;
    /* 0x76 */  s16     height_water;
    /* 0x78 */  struct object_t *object_touch;
    /* 0x7C */  struct object_t *object_hold;
    /* 0x80 */  struct object_t *object_use;
    /* 0x84 */  struct object_t *object_ride;
    /* 0x88 */  struct object_t *object;
    /* 0x8C */  void   *_8C;
    /* 0x90 */  void   *world;
    /* 0x94 */  void   *camera;
    /* 0x98 */  struct player_gfx_t *gfx;
    /* 0x9C */  struct pad_t *pad;
    /* 0xA0 */  struct motion_t *motion;
    /* 0xA4 */  u32     touch;
    /* 0xA8 */  s16     coins;
    /* 0xAA */  s16     stars;
    /* 0xAC */  s8      keys;
    /* 0xAD */  s8      lives;
    /* 0xAE */  s16     health;
    /* 0xB0 */  s16     motion_height;
    /* 0xB2 */  u8      timer_power_dec;
    /* 0xB3 */  u8      timer_power_inc;
    /* 0xB4 */  u8      timer_squish;
    /* 0xB5 */  u8      timer_dither;
    /* 0xB6 */  u16     timer_cap;
    /* 0xB8 */  s16     stars_prev;
    /* 0xBC */  f32     height_peak;
    /* 0xC0 */  f32     height_sink;
    /* 0xC4 */  f32     gravity;
};  /* 0xC8 */

/* 0x80278504 */ extern void mem_dma_read(
    void *dst, const void *start, const void *end
);
/* 0x80278F2C */ extern void *mem_alloc_gfx(size_t size);
/* 0x8029ED20 */ extern struct object_t *object_spawn(
    const struct object_t *parent,
    u32 a1,
    u8 gfx,
    const uintptr_t *script
);
/* 0x802D62D8 */ extern void hud_print_int(
    s16 x, s16 y, const char *str, s32 i
);
/* 0x802D6554 */ extern void hud_print(s16 x, s16 y, const char *str);
/* 0x802D77DC */ extern void menu_print(s16 x, s16 y, const u8 *str);
/* 0x80324570 */ extern void bzero(void *, s32);
/* 0x80324610 */ extern void osInvalICache(void *, s32);
/* 0x80325D20 */ extern void osWritebackDCache(void *, s32);
/* 0x803273F0 */ extern void *memcpy(void *, const void *, size_t);
/* 0x8032D93C */ extern struct player_t *g_player_p1;
/* 0x8032DD50 */ extern u8 g_player_blink_table[8];
/* 0x8032DDC4 */ extern struct gfx_t **g_gfx_object_table;
/* 0x8032DDF8 */ extern s16 g_stage_index;
/* 0x8032DEFC */ extern struct gfx_camera_t *g_gfx_camera;
/* 0x8032DF00 */ extern struct gfx_object_t *g_gfx_object;
/* 0x8032DF08 */ extern u16 g_motion_timer;
/* 0x80331370 */ extern const u8 g_menu_kern[0x100];
/* 0x8033B06C */ extern Gfx *g_dl;
/* 0x8033B080 */ extern uintptr_t *g_player_motion_table;
/* 0x8033B3B0 */ extern struct player_gfx_t g_player_gfx_table[2];
/* 0x8033B400 */ extern void *g_mem_segment_table[0x20];
/* 0x8033BACA */ extern u16 g_world_index;
/* 0x8033C536 */ extern s16 g_camera_rot_head[2];
/* 0x80361158 */ extern struct object_t *g_object_p1;
/* 0x8037E0B4 */ extern struct gfx_t *gfx_script_compile(
    struct heap_t *heap, const uintptr_t *script
);
/* 0x004EC000 */ extern u8 seg_player_motion_start[];

#endif /* _SM64_H_ */
