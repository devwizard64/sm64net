/******************************************************************************
 *                  SM64Net - An Internet framework for SM64                  *
 *                    Copyright (C) 2019 - 2021  devwizard                    *
 *        This project is licensed under the terms of the GNU General         *
 *        Public License version 2.  See LICENSE for more information.        *
 ******************************************************************************/

static const char *const str_stage_table[] =
{
    NULL,       /*  1                                   */
    NULL,       /*  2                                   */
    NULL,       /*  3                                   */
    "BBH",      /*  4  Big Boo's Haunt                  */
    "CCM",      /*  5  Cool, Cool Mountain              */
    "Lobby",    /*  6  Castle Inside                    */
    "HMC",      /*  7  Hazy Maze Cave                   */
    "SSL",      /*  8  Shifting Sand Land               */
    "BoB",      /*  9  Bob-omb Battlefield              */
    "SL",       /* 10  Snowman's Land                   */
    "WDW",      /* 11  Wet-Try Dorld                    */
    "JRB",      /* 12  Jolly Roger Bay                  */
    "THI Huge", /* 13  Tiny-Huge Island                 */
    "TTC",      /* 14  Tick Tock Cock                   */
    "RR",       /* 15  Rainbow Ride                     */
    "Outside",  /* 16  Castle Grounds                   */
    "BitDW",    /* 17  Bowser in the Dark World         */
    "VCutM",    /* 18  Vanish Cap under the Moat        */
    "BitFS",    /* 19  Bowser in the Fire Sea           */
    "SA",       /* 20  The Secret Aquarium              */
    "BitS",     /* 21  Bowser in the Sky                */
    "LLL",      /* 22  Lethal Lava Land                 */
    "DDD",      /* 23  Dire, Dire Docks                 */
    "WF",       /* 24  Whomp's Fortress                 */
    "The End",  /* 25  The End                          */
    "Crtyrd",   /* 26  Castle Courtyard                 */
    "PSS",      /* 27  The Princess's Secret Slide      */
    "CotMC",    /* 28  Cavern of the Metal Cap          */
    "TotWC",    /* 29  Tower of the Wing Cap            */
    "BitDW A",  /* 30  Bowser in the Dark World (Arena) */
    "WMotR",    /* 31  Wing Mario over the Rainbow      */
    NULL,       /* 32                                   */
    "BitFS A",  /* 33  Bowser in the Fire Sea (Arena)   */
    "BitS A",   /* 34  Bowser in the Sky (Arena)        */
    NULL,       /* 35                                   */
    "TTM",      /* 36  Tall, Tall Mountain              */
    NULL,       /* 37                                   */
    NULL,       /* 38                                   */
};

static s16 location_y = 0;
static u8  location_start = 1;
static u8  location_index = 1;
static u8  location_draw = false;

static void location_np_update_self(unused NET_PL *np, PLAYER *pl)
{
    if (pl->cont->down & L_TRIG) location_draw ^= true;
    if (location_draw)
    {
        if (pl->cont->down & D_JPAD)
        {
            if (location_start > 1) location_start--;
        }
        if (pl->cont->down & U_JPAD)
        {
            if (location_start < NP_LEN) location_start++;
        }
        location_y = 20 + 18*9;
        dprintf(320-8 - 12*9, location_y, "%2d online", location_index);
        location_index = 1;
    }
}

static void location_np_update_peer(NET_PL *np)
{
    if (location_draw && np->np_timer > 0)
    {
        if (location_index >= location_start && location_y >= 20)
        {
            const char *str = str_stage_table[np->np_stage-1];
            switch (np->np_stage << 4 | np->np_scene)
            {
                case  5 << 4 | 2: str = "CCM Sld";  break;
                case  8 << 4 | 2: str = "Pyramid";  break;
                case  6 << 4 | 2: str = "Upstairs"; break;
                case  6 << 4 | 3: str = "Basement"; break;
                case 12 << 4 | 2: str = "JRB Ship"; break;
                case 13 << 4 | 2: str = "THI Tiny"; break;
                case 13 << 4 | 3: str = "Wiggler";  break;
                case 22 << 4 | 2: str = "Volcano";  break;
                case 36 << 4 | 2:
                case 36 << 4 | 3:
                case 36 << 4 | 4: str = "TTM Sld";  break;
            }
            if (str == NULL) str = "???";
            dprintf(8, location_y, "%2d", location_index);
            dprint(8 + 12*3, location_y, str);
            location_y -= 18;
        }
        location_index++;
    }
}
