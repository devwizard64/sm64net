/******************************************************************************
 *                  SM64Net - An Internet framework for SM64                  *
 *                    Copyright (C) 2019 - 2022  devwizard                    *
 *        This project is licensed under the terms of the GNU General         *
 *        Public License version 2.  See LICENSE for more information.        *
 ******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#ifdef WIN32
#include <windows.h>
#include <tlhelp32.h>
#else
#include <errno.h>
#include <dirent.h>
#endif

#ifndef PATH_MAX
#define PATH_MAX 4096
#endif

#include <types.h>

static uintptr_t base;
static pid_t pid;

static void *byteswap(void *dst, const void *src, size_t size)
{
    u8 *_dst = dst;
    const u8 *_src = src;
    uint i;
    while (size >= 4)
    {
        u8 a = _src[0];
        u8 b = _src[1];
        u8 c = _src[2];
        u8 d = _src[3];
        _dst[0] = d;
        _dst[1] = c;
        _dst[2] = b;
        _dst[3] = a;
        _dst += 4;
        _src += 4;
        size -= 4;
    }
    for (i = 0; i < size; i++) _dst[i] = _src[i^3];
    return dst;
}

void mem_read(PTR addr, void *dst, size_t size)
{
    int error = true;
    void *data = dst;
#ifdef WIN32
    HANDLE hProcess;
    if ((hProcess = OpenProcess(PROCESS_ALL_ACCESS, false, pid)) != NULL)
    {
        ReadProcessMemory(
            hProcess, (LPCVOID *)(base + (addr & 0x1FFFFFFF)), data, size, NULL
        );
        CloseHandle(hProcess);
        error = false;
    }
#else
    char path[PATH_MAX];
    FILE *f;
    snprintf(path, sizeof(path), "/proc/%d/mem", pid);
    if ((f = fopen(path, "rb")) != NULL)
    {
        fseek(f, base + (addr & 0x1FFFFFFF), SEEK_SET);
        fread(data, 1, size, f);
        fclose(f);
        error = false;
    }
#endif
    if (error) eprint("could not read from process %d\n", pid);
    byteswap(dst, data, size);
}

void mem_write(PTR addr, const void *src, size_t size)
{
    int error = true;
    void *data = byteswap(malloc(size), src, size);
#ifdef WIN32
    HANDLE hProcess;
    if ((hProcess = OpenProcess(PROCESS_ALL_ACCESS, false, pid)) != NULL)
    {
        WriteProcessMemory(
            hProcess, (LPCVOID *)(base + (addr & 0x1FFFFFFF)), data, size, NULL
        );
        CloseHandle(hProcess);
        error = false;
    }
#else
    char path[PATH_MAX];
    FILE *f;
    snprintf(path, sizeof(path), "/proc/%d/mem", pid);
    if ((f = fopen(path, "wb")) != NULL)
    {
        fseek(f, base + (addr & 0x1FFFFFFF), SEEK_SET);
        fwrite(data, 1, size, f);
        fclose(f);
        error = false;
    }
#endif
    free(data);
    if (error) eprint("could not write to process %d\n", pid);
}

static void mem_init_pid(const char *proc)
{
    int error = true;
#ifdef WIN32
    HANDLE hSnapshot;
    if ((hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0)) != NULL)
    {
        PROCESSENTRY32 entry;
        entry.dwSize = sizeof(PROCESSENTRY32);
        if (Process32First(hSnapshot, &entry)) do
        {
            if (strstr(entry.szExeFile, proc) != NULL)
            {
                pid = entry.th32ProcessID;
                error = false;
                break;
            }
        }
        while (Process32Next(hSnapshot, &entry));
        CloseHandle(hSnapshot);
    }
#else
    DIR *dir;
    if ((dir = opendir("/proc")) != NULL)
    {
        struct dirent *dirent;
        while ((dirent = readdir(dir)) != NULL)
        {
            char path[PATH_MAX];
            FILE *f;
            errno = 0;
            pid = strtol(dirent->d_name, NULL, 0);
            if (errno != 0) continue;
            snprintf(path, sizeof(path), "/proc/%d/cmdline", pid);
            if ((f = fopen(path, "r")) != NULL)
            {
                char buf[PATH_MAX];
                char *str = fgets(buf, lenof(buf), f);
                fclose(f);
                if (str != NULL && strstr(str, proc) != NULL)
                {
                    error = false;
                    break;
                }
            }
        }
        closedir(dir);
    }
#endif
    if (error) eprint("could not find process '%s'\n", proc);
}

static int mem_test(const u8 *key, size_t size, uintptr_t start, uintptr_t end)
{
    for (base = start; base < end; base += size)
    {
        PTR addr;
        for (addr = 0x80000000; addr < 0x80000200; addr += 0x80)
        {
            u8 buf[0x10];
            mem_read(addr, buf, sizeof(buf));
            if (memcmp(key, buf, sizeof(buf)) != 0) goto next;
        }
        return true;
    next:;
    }
    return false;
}

static void mem_init_addr(PTR addr)
{
    int error = true;
    uint hi = (addr >> 16) + (addr >> 15 & 1);
    u8 key[0x10] =
    {
        0x3C, 0x1A, hi   >> 8, hi   >> 0,
        0x27, 0x5A, addr >> 8, addr >> 0,
        0x03, 0x40, 0x00, 0x08,
        0x00, 0x00, 0x00, 0x00,
    };
#ifdef WIN32
    SYSTEM_INFO si;
    HANDLE hProcess;
    GetSystemInfo(&si);
    if ((hProcess = OpenProcess(PROCESS_ALL_ACCESS, false, pid)) != NULL)
    {
        MEMORY_BASIC_INFORMATION mbi;
        void *start = NULL;
        while (VirtualQueryEx(hProcess, start, &mbi, sizeof(mbi)) > 0)
        {
            void *end = (char *)mbi.BaseAddress + mbi.RegionSize;
            if (mem_test(key, si.dwPageSize, (uintptr_t)start, (uintptr_t)end))
            {
                error = false;
                break;
            }
            if ((start = end) == (void *)~0) break;
        }
        CloseHandle(hProcess);
    }
#else
    long size = sysconf(_SC_PAGESIZE);
    char path[PATH_MAX];
    FILE *f;
    snprintf(path, sizeof(path), "/proc/%d/maps", pid);
    if ((f = fopen(path, "r")) != NULL)
    {
        char str[PATH_MAX];
        while (fgets(str, lenof(str), f) != NULL)
        {
            unsigned long int start;
            unsigned long int end;
            char r;
            char w;
            sscanf(str, "%lx-%lx %c%c", &start, &end, &r, &w);
            if (r == '-' || w == '-') continue;
            if (mem_test(key, size, start, end))
            {
                error = false;
                break;
            }
        }
        fclose(f);
    }
#endif
    if (error) eprint("could not find base address\n");
}

void mem_init(const char *proc, PTR addr)
{
    mem_init_pid(proc);
    mem_init_addr(addr);
}
