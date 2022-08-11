/******************************************************************************/
/*                  SM64Net - An Internet framework for SM64                  */
/*                    Copyright (C) 2019 - 2022  devwizard                    */
/*        This project is licensed under the terms of the GNU General         */
/*        Public License version 2.  See LICENSE for more information.        */
/******************************************************************************/

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
    char *_dst = dst;
    const char *_src = src;
    size_t i;
    while (size >= 4)
    {
        char a = _src[0];
        char b = _src[1];
        char c = _src[2];
        char d = _src[3];
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

void mem_rd(PTR addr, void *dst, size_t size)
{
    int error = TRUE;
    uintptr_t ptr = base + (addr & 0x1FFFFFFF);
    void *data = dst;
#ifdef WIN32
    HANDLE hProcess;
    if ((hProcess = OpenProcess(PROCESS_VM_READ, FALSE, pid)) != NULL)
    {
        if (ReadProcessMemory(hProcess, (LPCVOID)ptr, data, size, NULL))
        {
            error = FALSE;
        }
        CloseHandle(hProcess);
    }
#else
    FILE *f;
    char path[PATH_MAX];
    snprintf(path, sizeof(path), "/proc/%d/mem", pid);
    if ((f = fopen(path, "rb")) != NULL)
    {
        fseek(f, ptr, SEEK_SET);
        fread(data, 1, size, f);
        fclose(f);
        error = FALSE;
    }
#endif
    if (error) eprint("could not read from process %d\n", (int)pid);
    byteswap(dst, data, size);
}

void mem_wr(PTR addr, const void *src, size_t size)
{
    int error = TRUE;
    uintptr_t ptr = base + (addr & 0x1FFFFFFF);
    void *data = byteswap(malloc(size), src, size);
#ifdef WIN32
    HANDLE hProcess;
    if ((hProcess = OpenProcess(
        PROCESS_VM_OPERATION|PROCESS_VM_WRITE, FALSE, pid
    )) != NULL)
    {
        if (WriteProcessMemory(hProcess, (LPVOID)ptr, data, size, NULL))
        {
            error = FALSE;
        }
        CloseHandle(hProcess);
    }
#else
    FILE *f;
    char path[PATH_MAX];
    snprintf(path, sizeof(path), "/proc/%d/mem", pid);
    if ((f = fopen(path, "wb")) != NULL)
    {
        fseek(f, ptr, SEEK_SET);
        fwrite(data, 1, size, f);
        fclose(f);
        error = FALSE;
    }
#endif
    free(data);
    if (error) eprint("could not write to process %d\n", (int)pid);
}

static int mem_test(
    const char *key, size_t step, uintptr_t start, uintptr_t end
)
{
    for (base = start; base < end; base += step)
    {
        PTR addr;
        for (addr = 0x80000000; addr < 0x80000200; addr += 0x80)
        {
            char buf[16];
            mem_rd(addr, buf, sizeof(buf));
            if (memcmp(key, buf, sizeof(buf))) goto next;
        }
        return TRUE;
    next:;
    }
    return FALSE;
}

static int mem_scan(const char *key, size_t step, size_t off)
{
    int error = TRUE;
#ifdef WIN32
    HANDLE hProcess;
    if ((hProcess = OpenProcess(PROCESS_QUERY_INFORMATION, FALSE, pid)) != NULL)
    {
        void *start = NULL;
        MEMORY_BASIC_INFORMATION mbi;
        while (VirtualQueryEx(
            hProcess, start, &mbi, sizeof(MEMORY_BASIC_INFORMATION)
        ))
        {
            void *end = (char *)mbi.BaseAddress + mbi.RegionSize;
            if (
                (mbi.State & MEM_COMMIT) &&
                (mbi.Protect & PAGE_READWRITE) &&
                !(mbi.Protect & PAGE_GUARD) &&
                mem_test(key, step, (uintptr_t)start+off, (uintptr_t)end)
            )
            {
                error = FALSE;
                break;
            }
            if ((start = end) == (void *)~0) break;
        }
        CloseHandle(hProcess);
    }
#else
    FILE *f;
    char path[PATH_MAX];
    snprintf(path, sizeof(path), "/proc/%d/maps", pid);
    if ((f = fopen(path, "r")) != NULL)
    {
        char str[PATH_MAX];
        while (fgets(str, sizeof(str), f) != NULL)
        {
            unsigned long start;
            unsigned long end;
            char r;
            char w;
            sscanf(str, "%lx-%lx %c%c", &start, &end, &r, &w);
            if (r == '-' || w == '-') continue;
            if (mem_test(key, step, start+off, end))
            {
                error = FALSE;
                break;
            }
        }
        fclose(f);
    }
#endif
    return error;
}

static int mem_find(const char *proc)
{
    int error = TRUE;
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
                error = FALSE;
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
            if (errno) continue;
            snprintf(path, sizeof(path), "/proc/%d/cmdline", pid);
            if ((f = fopen(path, "r")) != NULL)
            {
                char buf[PATH_MAX];
                char *str = fgets(buf, sizeof(buf), f);
                fclose(f);
                if (str != NULL && strstr(str, proc) != NULL)
                {
                    error = FALSE;
                    break;
                }
            }
        }
        closedir(dir);
    }
#endif
    return error;
}

static size_t mem_pagesize(void)
{
#ifdef WIN32
    SYSTEM_INFO si;
    GetSystemInfo(&si);
    return si.dwPageSize;
#else
    return sysconf(_SC_PAGESIZE);
#endif
}

void mem_init(const char *proc, PTR addr)
{
    size_t i;
    size_t pagesize = mem_pagesize();
    int hi = (addr >> 16) + (addr >> 15 & 1);
    char key[16];
    key[ 0] = 0x3C;
    key[ 1] = 0x1A;
    key[ 2] = hi >> 8;
    key[ 3] = hi >> 0;
    key[ 4] = 0x27;
    key[ 5] = 0x5A;
    key[ 6] = addr >> 8;
    key[ 7] = addr >> 0;
    key[ 8] = 0x03;
    key[ 9] = 0x40;
    key[10] = 0x00;
    key[11] = 0x08;
    key[12] = 0x00;
    key[13] = 0x00;
    key[14] = 0x00;
    key[15] = 0x00;
    if (mem_find(proc)) eprint("could not find process '%s'\n", proc);
    for (i = 0; i < pagesize; i += 16)
    {
        if (!mem_scan(key, pagesize, i)) return;
    }
    eprint("could not find base address\n");
}
