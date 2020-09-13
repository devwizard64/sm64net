/******************************************************************************
 *                  SM64Net - An Internet framework for SM64                  *
 *                   Copyright (C) 2019, 2020  devwizard                      *
 *      This project is licensed under the GNU General Public License         *
 *      version 2.  See LICENSE for more information.                         *
 ******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#ifdef WIN32
#include <windows.h>
#include <tlhelp32.h>
#endif

#include "types.h"

#include "assert.h"

static const u8 mem_key[] =
{
    0x3C, 0x1A, 0x80, 0x32,
    0x27, 0x5A, 0x76, 0x50,
    0x03, 0x40, 0x00, 0x08,
    0x00, 0x00, 0x00, 0x00,
};

static int       mem_pid;
static uintptr_t mem_addr;

static void mem_byteswap(void *dst, const void *src, size_t size)
{
    u8 *_dst = dst;
    const u8 *_src = src;
    do
    {
        _dst[0] = _src[3];
        _dst[1] = _src[2];
        _dst[2] = _src[1];
        _dst[3] = _src[0];
        _dst += 4;
        _src += 4;
        size -= 4;
    }
    while (size > 0);
}

bool mem_read(u32 addr, void *dst, size_t size)
{
    void *data;
#ifdef WIN32
    HANDLE hProcess;
#else
    FILE *f;
    char fn[0x100];
#endif
    data = malloc(size);
#ifdef WIN32
    hProcess = OpenProcess(PROCESS_ALL_ACCESS, false, mem_pid);
    ReadProcessMemory(
        hProcess, (LPCVOID *)(mem_addr + (addr & 0x1FFFFFFF)),
        data, size, NULL
    );
    CloseHandle(hProcess);
#else
    snprintf(fn, sizeof(fn), "/proc/%d/mem", mem_pid);
    f = fopen(fn, "rb");
    if (f == NULL)
    {
        fprintf(stderr, "error: could not read process %d\n", mem_pid);
        free(data);
        return true;
    }
    fseek(f, mem_addr + (addr & 0x1FFFFFFF), SEEK_SET);
    fread(data, 1, size, f);
    fclose(f);
#endif
    mem_byteswap(dst, data, size);
    free(data);
    return false;
}

bool mem_write(u32 addr, const void *src, size_t size)
{
    void *data;
#ifdef WIN32
    HANDLE hProcess;
#else
    FILE *f;
    char fn[0x100];
#endif
    data = malloc(size);
    mem_byteswap(data, src, size);
#ifdef _WIN32
    hProcess = OpenProcess(PROCESS_ALL_ACCESS, false, mem_pid);
    WriteProcessMemory(
        hProcess, (LPCVOID *)(mem_addr + (addr & 0x1FFFFFFF)),
        data, size, NULL
    );
    CloseHandle(hProcess);
    free(data);
#else
    snprintf(fn, sizeof(fn), "/proc/%d/mem", mem_pid);
    f = fopen(fn, "wb");
    if (f == NULL)
    {
        fprintf(stderr, "error: could not write process %d\n", mem_pid);
        free(data);
        return true;
    }
    fseek(f, mem_addr + (addr & 0x1FFFFFFF), SEEK_SET);
    fwrite(data, 1, size, f);
    fclose(f);
#endif
    free(data);
    return false;
}

static bool str_sub(const char *a, const char *b)
{
    while (strlen(a) >= strlen(b))
    {
        if (strcmp(a, b) == 0)
        {
            return true;
        }
        a++;
    }
    return false;
}

static bool mem_init_pid(const char *proc)
{
#ifdef WIN32
    HANDLE snapshot;
    PROCESSENTRY32 entry;
    snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    entry.dwSize = sizeof(PROCESSENTRY32);
    if (Process32First(snapshot, &entry))
    {
        do
        {
            if (str_sub(entry.szExeFile, proc))
            {
                mem_pid = entry.th32ProcessID;
                CloseHandle(snapshot);
                return false;
            }
        }
        while (Process32Next(snapshot, &entry));
    }
    CloseHandle(snapshot);
#else
    for (mem_pid = 0; mem_pid < 0x10000; mem_pid++)
    {
        char fn[0x100];
        snprintf(fn, sizeof(fn), "/proc/%d/mem", mem_pid);
        if (access(fn, R_OK) != -1)
        {
            FILE *f;
            char  cmd[1024];
            snprintf(fn, sizeof(fn), "/proc/%d/cmdline", mem_pid);
            f = fopen(fn, "rb");
            if (f == NULL)
            {
                fprintf(
                    stderr, "error: /proc/%d/cmdline does not exist (?)\n",
                    mem_pid
                );
                return true;
            }
            fread(cmd, 1, sizeof(cmd), f);
            fclose(f);
            cmd[sizeof(cmd) - 1] = 0x00;
            if (str_sub(cmd, proc))
            {
                return false;
            }
        }
    }
#endif
    fprintf(stderr, "error: could not find process '%s'\n", proc);
    return true;
}

static bool mem_init_addr(void)
{
    u32 i;
    for (i = 0; i < 0x10000; i++)
    {
        u8 buf[0x200];
        mem_addr = i << 16;
        assert(mem_read(0, buf, sizeof(buf)));
        if
        (
            memcmp(buf + 0x000, mem_key, sizeof(mem_key)) == 0 &&
            memcmp(buf + 0x080, mem_key, sizeof(mem_key)) == 0 &&
            memcmp(buf + 0x100, mem_key, sizeof(mem_key)) == 0 &&
            memcmp(buf + 0x180, mem_key, sizeof(mem_key)) == 0
        )
        {
            return false;
        }
    }
    fprintf(stderr, "error: could not find base address\n");
    return true;
}

bool mem_init(const char *proc)
{
    return mem_init_pid(proc) || mem_init_addr();
}
