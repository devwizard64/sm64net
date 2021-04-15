/******************************************************************************
 *                  SM64Net - An Internet framework for SM64                  *
 *                    Copyright (C) 2019 - 2021  devwizard                    *
 *        This project is licensed under the terms of the GNU General         *
 *        Public License version 2.  See LICENSE for more information.        *
 ******************************************************************************/

#ifndef _SM64_H_
#define _SM64_H_

#include <types.h>

#define OS_MESG_NOBLOCK         0x00
#define OS_MESG_BLOCK           0x01

#define OS_READ                 0x00
#define OS_WRITE                0x01
#define OS_MESG_PRI_NORMAL      0x00
#define OS_MESG_PRI_HIGH        0x01

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

#define G_T_GFX         0x000A
#define G_T_BILLBOARD   0x001A
#define G_T_SCALE       0x001C
#define G_T_CALLBACK    0x012A

#define G_F_ENABLED     0x0001

#define G_R_BACKGROUND  0x00
#define G_R_OPA_SURF    0x01
#define G_R_OPA_DECAL   0x02
#define G_R_OPA_INTER   0x03
#define G_R_SPRITE      0x04
#define G_R_XLU_SURF    0x05
#define G_R_XLU_DECAL   0x06
#define G_R_XLU_INTER   0x07

#ifndef __ASSEMBLER__

enum
{
    /* 0x00 */  PLAYER_EYE_BLINK,
    /* 0x01 */  PLAYER_EYE_OPEN,
    /* 0x02 */  PLAYER_EYE_HALF,
    /* 0x03 */  PLAYER_EYE_CLOSED,
    /* 0x04 */  PLAYER_EYE_LEFT,
    /* 0x05 */  PLAYER_EYE_RIGHT,
    /* 0x06 */  PLAYER_EYE_UP,
    /* 0x07 */  PLAYER_EYE_DOWN,
    /* 0x08 */  PLAYER_EYE_DEAD,
};

enum
{
    /* 0x00 */  PLAYER_CAP_NONE,
    /* 0x01 */  PLAYER_CAP_VANISH,
    /* 0x02 */  PLAYER_CAP_METAL,
    /* 0x03 */  PLAYER_CAP_VANISH_METAL,
};

typedef void *OSMesg;
typedef struct
{
    void *_[5];
}
OSMesgQueue;

typedef struct
{
    void *_[6];
}
OSIoMesg;

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

struct arena_t
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
    /* 0x08 */  s16     frame;
    /* 0x0A */  s16     joint;
    /* 0x0C */  s16    *val;
    /* 0x10 */  u16    *tbl;
    /* 0x14 */  size_t  size;
};  /* 0x18 */

struct g_t
{
    /* 0x00 */  s16     type;
    /* 0x02 */  s16     flag;
    /* 0x04 */  struct g_t *prev;
    /* 0x08 */  struct g_t *next;
    /* 0x0C */  struct g_t *parent;
    /* 0x10 */  struct g_t *child;
};  /* 0x14 */

struct gc_t
{
    /* 0x00 */  struct g_t g;
    /* 0x14 */  void *(*callback)(int, struct g_t *, void *);
    /* 0x18 */  int     arg;
};  /* 0x1C */

struct gg_t
{
    /* 0x00 */  struct g_t g;
    /* 0x14 */  Gfx    *gfx;
};  /* 0x18 */

struct g_billboard_t
{
    /* 0x00 */  struct gg_t g;
    /* 0x18 */  s16     pos[3];
};  /* 0x1E */

struct g_scale_t
{
    /* 0x00 */  struct gg_t g;
    /* 0x18 */  f32     scale;
};  /* 0x1C */

struct go_motion_t
{
    /* 0x00 0x38 */ s16     index;
    /* 0x02 0x3A */ s16     height;
    /* 0x04 0x3C */ struct motion_t *motion;
    /* 0x08 0x40 */ s16     frame;
    /* 0x0A 0x42 */ u16     timer;
    /* 0x0C 0x44 */ s32     frame_amt;
    /* 0x10 0x48 */ s32     frame_vel;
};  /* 0x14 0x4C */

struct go_t
{
    /* 0x00 */  struct g_t g;
    /* 0x14 */  struct g_t *list;
    /* 0x18 */  s8      world_index;
    /* 0x19 */  s8      gfx_index;
    /* 0x1A */  s16     rot[3];
    /* 0x20 */  f32     pos[3];
    /* 0x2C */  f32     scale[3];
    /* 0x38 */  struct go_motion_t motion;
    /* 0x4C */  void   *_4C;
    /* 0x50 */  f32   (*mtxf)[4][4];
    /* 0x54 */  f32     pos_sfx[3];
};  /* 0x60 */

struct g_camera_t
{
    /* 0x00 */  struct gc_t g;
    /* 0x1C */  f32     pos[3];
    /* 0x28 */  f32     obj[3];
    /* 0x34 */  f32   (*mtxf)[4][4];
    /* 0x38 */  s16     rot_z_m;
    /* 0x3A */  s16     rot_z_p;
};  /* 0x3C */

struct object_t
{
    /* 0x0000 */    struct go_t g;
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
                        s8      s8[4];
                        u8      u8[4];
                        s16     s16[2];
                        u16     u16[2];
                        s32     s32;
                        u32     u32;
                        void   *ptr;
                    }
                    mem[80];
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
    /* 0x05 */  s8      eye;
    /* 0x06 */  s8      glove;
    /* 0x07 */  s8      wing;
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
    /* 0x26 */  s16     invincible;
    /* 0x28 */  u8      timer_a;
    /* 0x29 */  u8      timer_b;
    /* 0x2A */  u8      timer_wall;
    /* 0x2B */  u8      timer_floor;
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
    /* 0xA8 */  s16     coin;
    /* 0xAA */  s16     star;
    /* 0xAC */  s8      key;
    /* 0xAD */  s8      life;
    /* 0xAE */  s16     power;
    /* 0xB0 */  s16     motion_height;
    /* 0xB2 */  u8      timer_hurt;
    /* 0xB3 */  u8      timer_heal;
    /* 0xB4 */  u8      timer_squish;
    /* 0xB5 */  u8      timer_dither;
    /* 0xB6 */  u16     timer_cap;
    /* 0xB8 */  s16     star_prev;
    /* 0xBC */  f32     height_peak;
    /* 0xC0 */  f32     height_sink;
    /* 0xC4 */  f32     gravity;
};  /* 0xC8 */

/* 0x80278504 */ extern void mem_dma(
    void *dst, const void *start, const void *end
);
/* 0x80278F2C */ extern void *gfx_alloc(size_t size);
/* 0x8029ED20 */ extern struct object_t *object_create(
    const struct object_t *parent,
    u32 arg,
    u8 gfx,
    const uintptr_t *script
);
/* 0x802D62D8 */ extern void print_int(
    s16 x, s16 y, const char *str, int i
);
/* 0x802D6554 */ extern void print(s16 x, s16 y, const char *str);
/* 0x802D77DC */ extern void message_print(s16 x, s16 y, const u8 *str);
/* 0x803225A0 */ extern void osCreateMesgQueue(OSMesgQueue *, OSMesg *, s32);
/* 0x80322800 */ extern s32 osRecvMesg(OSMesgQueue *, OSMesg *, s32);
/* 0x803243B0 */ extern void osInvalDCache(void *, s32);
/* 0x80324460 */ extern s32 osPiStartDma(
    OSIoMesg *, s32, s32, u32, void *, u32, OSMesgQueue *
);
/* 0x80324570 */ extern void bzero(void *, s32);
/* 0x80324610 */ extern void osInvalICache(void *, s32);
/* 0x80325D20 */ extern void osWritebackDCache(void *, s32);
/* 0x803273F0 */ extern void *memcpy(void *, const void *, size_t);
/* 0x8032D93C */ extern struct player_t *player_p1;
/* 0x8032DD50 */ extern u8 player_blink[8];
/* 0x8032DDC4 */ extern struct g_t **world_gfx_table;
/* 0x8032DDF8 */ extern s16 world_stage;
/* 0x8032DEFC */ extern struct g_camera_t *gfx_camera;
/* 0x8032DF00 */ extern struct g_object_t *gfx_object;
/* 0x8032DF08 */ extern u16 gfx_frame;
/* 0x80331370 */ extern const u8 message_kern[0x100];
/* 0x8033B06C */ extern Gfx *video_gfx;
/* 0x8033B080 */ extern uintptr_t *ft_motion_player;
/* 0x8033B3B0 */ extern struct player_gfx_t player_gfx_table[2];
/* 0x8033B400 */ extern uintptr_t segment_table[0x20];
/* 0x8033BACA */ extern u16 world_index;
/* 0x8033C536 */ extern s16 camera_rot_head[2];
/* 0x80361158 */ extern struct object_t *object_p1;
/* 0x8037E0B4 */ extern struct g_t *script_g_main(
    struct arena_t *arena, const uintptr_t *script
);
/* 0x004EC000 */ extern u8 data_motion_player_start[];

#endif /* __ASSEMBLER__ */

#endif /* _SM64_H_ */
