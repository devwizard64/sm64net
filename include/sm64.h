/******************************************************************************
 *                  SM64Net - An Internet framework for SM64                  *
 *                    Copyright (C) 2019 - 2022  devwizard                    *
 *        This project is licensed under the terms of the GNU General         *
 *        Public License version 2.  See LICENSE for more information.        *
 ******************************************************************************/

#ifndef __SM64_H__
#define __SM64_H__

#include <types.h>

#define OS_MESG_NOBLOCK         0
#define OS_MESG_BLOCK           1

#define OS_READ                 0
#define OS_WRITE                1
#define OS_MESG_PRI_NORMAL      0
#define OS_MESG_PRI_HIGH        1

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

#define S_TYPE_EMPTY            0x000A
#define S_TYPE_BILLBOARD        0x001A
#define S_TYPE_SCALE            0x001C
#define S_TYPE_CALLBACK         0x012A

#define S_FLAG_ENABLED          0x0001

#define S_LAYER_BACKGROUND      0
#define S_LAYER_OPA_SURF        1
#define S_LAYER_OPA_DECAL       2
#define S_LAYER_OPA_INTER       3
#define S_LAYER_TEX_EDGE        4
#define S_LAYER_XLU_SURF        5
#define S_LAYER_XLU_DECAL       6
#define S_LAYER_XLU_INTER       7

#define COLLISION_PUNCH     1
#define COLLISION_KICK      2
#define COLLISION_JUMP      3
#define COLLISION_POUND     4
#define COLLISION_KNOCK     5
#define COLLISION_BUMP      6

#define PL_EYE_BLINK        0
#define PL_EYE_OPEN         1
#define PL_EYE_HALF         2
#define PL_EYE_CLOSED       3
#define PL_EYE_LEFT         4
#define PL_EYE_RIGHT        5
#define PL_EYE_UP           6
#define PL_EYE_DOWN         7
#define PL_EYE_DEAD         8

#define PL_CAP_NONE         0
#define PL_CAP_VANISH       1
#define PL_CAP_METAL        2
#define PL_CAP_VANISH_METAL 3

#ifndef __ASSEMBLER__

#define K0_TO_K1(x)     ((u32)(x)|0xA0000000)
#define K1_TO_K0(x)     ((u32)(x)&0x9FFFFFFF)
#define K0_TO_PHYS(x)   ((u32)(x)&0x1FFFFFFF)
#define K1_TO_PHYS(x)   ((u32)(x)&0x1FFFFFFF)
#define KDM_TO_PHYS(x)  ((u32)(x)&0x1FFFFFFF)
#define PHYS_TO_K0(x)   ((u32)(x)|0x80000000)
#define PHYS_TO_K1(x)   ((u32)(x)|0xA0000000)

#define /* 0x00A0 */    o_pos_x         mem[ 6].f32
#define /* 0x00A4 */    o_pos_y         mem[ 7].f32
#define /* 0x00A8 */    o_pos_z         mem[ 8].f32
#define /* 0x0134 */    o_colflag       mem[43].u32
#define /* 0x0190 */    o_colarg        mem[66].u32

typedef void *OSMesg;
typedef struct
{
    void *_[5];
}
OSMesgQueue;

typedef struct
{
    void *_[5];
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

typedef s16 vecs[3];
typedef f32 vecf[3];
typedef f32 mtxf[4][4];

typedef struct controller
{
    /* 0x00 */  s16     stick_x;
    /* 0x02 */  s16     stick_y;
    /* 0x04 */  f32     x;
    /* 0x08 */  f32     y;
    /* 0x0C */  f32     d;
    /* 0x10 */  u16     held;
    /* 0x12 */  u16     down;
    /* 0x14 */  void   *status;
    /* 0x18 */  void   *pad;
}   /* 0x1C */
CONTROLLER;

typedef struct arena
{
    /* 0x00 */  size_t  size;
    /* 0x04 */  size_t  used;
    /* 0x08 */  u8     *start;
    /* 0x0C */  u8     *free;
}   /* 0x10 */
ARENA;

typedef struct file_table
{
    /* 0x00 */  uint    len;
    /* 0x04 */  u8     *src;
    /* 0x08 */  struct
                {
                    /* 0x00 */  uint    start;
                    /* 0x04 */  uint    size;
                }
                table[1];
}   /* 0x08 */
FILE_TABLE;

typedef struct file
{
    /* 0x00 */  struct file_table *table;
    /* 0x04 */  u8     *src;
    /* 0x08 */  u8     *buf;
}   /* 0x0C */
FILE;

typedef struct camera
{
    /* 0x00 */  u8      mode;
}
CAMERA;

typedef uintptr_t S_SCRIPT;
typedef uintptr_t O_SCRIPT;

typedef struct anime
{
    /* 0x00 */  s16     flag;
    /* 0x02 */  s16     waist;
    /* 0x04 */  s16     start;
    /* 0x06 */  s16     end;
    /* 0x08 */  s16     frame;
    /* 0x0A */  s16     joint;
    /* 0x0C */  s16    *val;
    /* 0x10 */  u16    *tbl;
    /* 0x14 */  size_t  size;
}   /* 0x18 */
ANIME;

typedef struct skeleton
{
    /* 0x00 */  s16     index;
    /* 0x02 */  s16     waist;
    /* 0x04 */  struct anime *anime;
    /* 0x08 */  s16     frame;
    /* 0x0A */  u16     timer;
    /* 0x0C */  s32     frame_amt;
    /* 0x10 */  s32     frame_vel;
}   /* 0x14 */
SKELETON;

typedef struct shape
{
    /* 0x00 */  s16     type;
    /* 0x02 */  s16     flag;
    /* 0x04 */  struct shape *prev;
    /* 0x08 */  struct shape *next;
    /* 0x0C */  struct shape *parent;
    /* 0x10 */  struct shape *child;
}   /* 0x14 */
SHAPE;

typedef struct shape_callback
{
    /* 0x00 */  SHAPE   s;
    /* 0x14 */  void *(*callback)(int mode, struct shape *shape, void *data);
    /* 0x18 */  int     arg;
}   /* 0x1C */
SHAPE_CALLBACK;

typedef struct shape_camera
{
    /* 0x00 */  SHAPE_CALLBACK s;
    /* 0x1C */  vecf    eye;
    /* 0x28 */  vecf    look;
    /* 0x34 */  mtxf   *mf;
    /* 0x38 */  s16     rz_m;
    /* 0x3A */  s16     rz_p;
}   /* 0x3C */
SHAPE_CAMERA;

typedef struct shape_gfx
{
    /* 0x00 */  SHAPE   s;
    /* 0x14 */  const Gfx *gfx;
}   /* 0x18 */
SHAPE_GFX;

typedef struct shape_billboard
{
    /* 0x00 */  SHAPE_GFX s;
    /* 0x18 */  vecs    pos;
}   /* 0x1E */
SHAPE_BILLBOARD;

typedef struct shape_scale
{
    /* 0x00 */  SHAPE_GFX s;
    /* 0x18 */  f32     scale;
}   /* 0x1C */
SHAPE_SCALE;

typedef struct shape_object
{
    /* 0x00 */  SHAPE   s;
    /* 0x14 */  struct shape *list;
    /* 0x18 */  s8      scene;
    /* 0x19 */  s8      shape;
    /* 0x1A */  vecs    rot;
    /* 0x20 */  vecf    pos;
    /* 0x2C */  vecf    scale;
    /* 0x38 */  SKELETON skeleton;
    /* 0x4C */  void   *_4C;
    /* 0x50 */  mtxf   *mf;
    /* 0x54 */  vecf    view;
}   /* 0x60 */
SHAPE_OBJECT;

typedef struct obj_list
{
    /* 0x00 */  SHAPE_OBJECT s;
    /* 0x60 */  struct obj_list *next;
    /* 0x64 */  struct obj_list *prev;
}   /* 0x68 */
OBJ_LIST;

typedef struct object
{
    /* 0x000 */ OBJ_LIST list;
    /* 0x068 */ struct object *parent;
    /* 0x06C */ struct object *child;
    /* 0x070 */ u32     collision;
    /* 0x074 */ s16     flag;
    /* 0x076 */ s16     col_len;
    /* 0x078 */ struct object *obj_col[4];
    /* 0x088 */ union
                {
                    s8      s8[4];
                    u8      u8[4];
                    s16     s16[2];
                    u16     u16[2];
                    s32     s32;
                    u32     u32;
                    f32     f32;
                    void   *ptr;
                }
                mem[80];
    /* 0x1C8 */ void   *_1C8;
    /* 0x1CC */ const O_SCRIPT *pc;
    /* 0x1D0 */ uint    stack_index;
    /* 0x1D4 */ void   *stack[8];
    /* 0x1F4 */ s16     _1F4;
    /* 0x1F6 */ s16     _1F6;
    /* 0x1F8 */ f32     col_hit_r;
    /* 0x1FC */ f32     col_hit_h;
    /* 0x200 */ f32     col_dmg_r;
    /* 0x204 */ f32     col_dmg_h;
    /* 0x208 */ f32     col_offset;
    /* 0x20C */ const O_SCRIPT *script;
    /* 0x210 */ struct object *_210;
    /* 0x214 */ struct object *_214;
    /* 0x218 */ s16    *_218;
    /* 0x21C */ mtxf    mf;
    /* 0x25C */ void   *_25C;
}   /* 0x260 */
OBJECT;

typedef struct pl_shape
{
    /* 0x00 */  u32     state;
    /* 0x04 */  s8      head;
    /* 0x05 */  s8      eye;
    /* 0x06 */  s8      glove;
    /* 0x07 */  s8      wing;
    /* 0x08 */  s16     cap;
    /* 0x0A */  s8      hold;
    /* 0x0B */  u8      punch;
    /* 0x0C */  vecs    torso;
    /* 0x12 */  vecs    neck;
    /* 0x18 */  vecf    hand;
    /* 0x24 */  struct object *obj;
}   /* 0x28 */
PL_SHAPE;

typedef struct player
{
    /* 0x00 */  u16     index;
    /* 0x02 */  u16     event;
    /* 0x04 */  u32     flag;
    /* 0x08 */  u32     particle;
    /* 0x0C */  u32     state;
    /* 0x10 */  u32     state_prev;
    /* 0x14 */  u32     ground_se;
    /* 0x18 */  s16     mode;
    /* 0x1A */  u16     timer;
    /* 0x1C */  u32     arg;
    /* 0x20 */  f32     stick_d;
    /* 0x24 */  s16     stick_ry;
    /* 0x26 */  s16     invincible;
    /* 0x28 */  u8      timer_a;
    /* 0x29 */  u8      timer_b;
    /* 0x2A */  u8      timer_wall;
    /* 0x2B */  u8      timer_floor;
    /* 0x2C */  vecs    rot;
    /* 0x32 */  vecs    rot_vel;
    /* 0x38 */  s16     slide_ry;
    /* 0x3A */  s16     twirl_ry;
    /* 0x3C */  vecf    pos;
    /* 0x48 */  vecf    vel;
    /* 0x54 */  f32     vel_f;
    /* 0x58 */  f32     vel_h[2];
    /* 0x60 */  struct map_face *wall;
    /* 0x64 */  struct map_face *roof;
    /* 0x68 */  struct map_face *ground;
    /* 0x6C */  f32     roof_y;
    /* 0x70 */  f32     ground_y;
    /* 0x74 */  s16     ground_ry;
    /* 0x76 */  s16     water;
    /* 0x78 */  struct object *obj_col;
    /* 0x7C */  struct object *obj_hold;
    /* 0x80 */  struct object *obj_use;
    /* 0x84 */  struct object *obj_ride;
    /* 0x88 */  struct object *obj;
    /* 0x8C */  void   *_8C;
    /* 0x90 */  struct scene *scene;
    /* 0x94 */  struct pl_camera *camera;
    /* 0x98 */  struct pl_shape *shape;
    /* 0x9C */  struct controller *cont;
    /* 0xA0 */  struct file *file_anime;
    /* 0xA4 */  u32     collision;
    /* 0xA8 */  s16     coin;
    /* 0xAA */  s16     star;
    /* 0xAC */  s8      key;
    /* 0xAD */  s8      life;
    /* 0xAE */  s16     power;
    /* 0xB0 */  s16     waist;
    /* 0xB2 */  u8      hurt;
    /* 0xB3 */  u8      heal;
    /* 0xB4 */  u8      squish;
    /* 0xB5 */  u8      alpha;
    /* 0xB6 */  u16     timer_cap;
    /* 0xB8 */  s16     star_prev;
    /* 0xBC */  f32     peak;
    /* 0xC0 */  f32     sink;
    /* 0xC4 */  f32     gravity;
}   /* 0xC8 */
PLAYER;

/* 0x8024D578 */ extern void pl_collision_bound(
    PLAYER *pl, OBJECT *obj, float radius
);
/* 0x8024D998 */ extern int pl_collision_attacked(
    PLAYER *pl, OBJECT *obj
);
/* 0x80278504 */ extern void mem_dma(
    void *dst, const void *start, const void *end
);
/* 0x80278F2C */ extern void *gfx_alloc(size_t size);
/* 0x8029ED20 */ extern OBJECT *obj_create(
    OBJECT *parent,
    u32 arg,
    u8 shape,
    const O_SCRIPT *script
);
/* 0x802D62D8 */ extern void dprintf(int x, int y, const char *fmt, int value);
/* 0x802D6554 */ extern void dprint(int x, int y, const char *str);
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

/* 0x8032D93C */ extern PLAYER *mario;
/* 0x8032DD50 */ extern u8 pl_shape_blink[8];
/* 0x8032DDC4 */ extern SHAPE **shape_table;
/* 0x8032DDF8 */ extern s16 stage_index;
/* 0x8032DEFC */ extern SHAPE_CAMERA *shape_camera;
/* 0x8032DF00 */ extern SHAPE_OBJECT *shape_object;
/* 0x8032DF08 */ extern u16 shape_timer;
/* 0x80331370 */ extern u8 message_kern[0x100];

/* 0x8033B06C */ extern Gfx *video_gfx;
/* 0x8033B350 */ extern SHAPE_OBJECT shape_object_mirror;
/* 0x8033B3B0 */ extern PL_SHAPE pl_shape_table[2];
/* 0x8033B400 */ extern uintptr_t segment_table[0x20];
/* 0x8033BACA */ extern u16 scene_index;
/* 0x8033C536 */ extern s16 camera_rot_head[2];
/* 0x80361158 */ extern OBJECT *object_mario;
/* 0x80361160 */ extern OBJECT *object;

/* 0x8037E0B4 */ extern SHAPE *s_script_main(
    ARENA *arena, const S_SCRIPT *script
);

#endif /* __ASSEMBLER__ */

#endif /* __SM64_H__ */
