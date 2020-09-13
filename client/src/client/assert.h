/******************************************************************************
 *                  SM64Net - An Internet framework for SM64                  *
 *                   Copyright (C) 2019, 2020  devwizard                      *
 *      This project is licensed under the GNU General Public License         *
 *      version 2.  See LICENSE for more information.                         *
 ******************************************************************************/

#ifndef _ASSERT_H_
#define _ASSERT_H_

#include "types.h"

#define assert_main(s)                                                         \
{                                                                              \
    if (errno != 0)                                                            \
    {                                                                          \
        fprintf(stderr, "error: " s "\n");                                     \
        return EXIT_FAILURE;                                                   \
    }                                                                          \
}

#define assert_msg(c, s)                                                       \
{                                                                              \
    if (c)                                                                     \
    {                                                                          \
        fprintf(stderr, "error: " s " (%s)\n", strerror(errno));               \
        return true;                                                           \
    }                                                                          \
}

#define assert(c)                                                              \
{                                                                              \
    if (c)                                                                     \
    {                                                                          \
        return true;                                                           \
    }                                                                          \
}

#endif /* _ASSERT_H_ */
