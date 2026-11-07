/* 100 deterministic tests for get_next_line (mandatory). */
#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/types.h>
#include "get_next_line.h"

static int passed, failed;

static char *make_content(int id, size_t *out_len)
{
    size_t len, i, p = 0;
    char *s;
    /* IDs 0..99 exercise empty, short, long, missing newline, many lines,
       varying newline placement, and binary-independent byte patterns. */
    if (id == 0) len = 0;
    else if (id == 1) len = 1;
    else if (id == 2) len = 2;
    else if (id == 3) len = 3;
    else if (id == 4) len = 10;
    else if (id == 5) len = 1000;
    else if (id == 6) len = 2000;
    else if (id == 7) len = 256;
    else if (id == 8) len = 257;
    else if (id == 9) len = 512;
    else len = (size_t)((id * 137 + id * id * 31) % 500 + 1);
    s = malloc(len + 1);
    if (!s) return NULL;
    for (i = 0; i < len; ++i)
    {
        if (id == 2 || id == 12 || id == 22) s[i] = '\n';
        else if (id == 3 || id == 13 || id == 23) s[i] = 'Z';
        else if (id % 10 == 0) s[i] = (i % 2) ? '\n' : 'a';
        else if (id % 10 == 1) s[i] = (i % 17 == 0) ? '\n' : 'b';
        else if (id % 10 == 2) s[i] = (i % 97 == 0) ? '\n' : 'c';
        else if (id % 10 == 3) s[i] = (i % 1024 == 0) ? '\n' : 'd';
        else if (id % 10 == 4) s[i] = (i % 4096 == 0) ? '\n' : 'e';
        else if (id % 10 == 5) s[i] = 'f';
        else if (id % 10 == 6) s[i] = (i % 3 == 0) ? '\n' : 'g';
        else if (id % 10 == 7) s[i] = (i % 7 == 0) ? '\n' : 'h';
        else if (id % 10 == 8) s[i] = (i % 257 == 0) ? '\n' : 'i';
        else s[i] = (i % 31 == 0) ? '\n' : 'j';
    }
    /* Include both final-newline and no-final-newline cases. */
    if (len && id % 4 == 0) s[len - 1] = '\n';
    if (len && id % 4 == 1) s[len - 1] = 'X';
    s[len] = '\0';
    p = len;
    *out_len = p;
    return s;
}

static int write_all(int fd, const char *s, size_t len)
{
    ssize_t n;
    while (len)
    {
        n = write(fd, s, len);
        if (n <= 0) return 0;
        s += n;
        len -= (size_t)n;
    }
    return 1;
}

static int check_fd(int fd, const char *data, size_t len)
{
    size_t pos = 0, end, n, calls = 0;
    char *got;
    while (pos < len)
    {
        end = pos;
        while (end < len && data[end] != '\n') ++end;
        if (end < len) ++end;
        n = end - pos;
        got = get_next_line(fd);
        if (!got || strlen(got) != n || memcmp(got, data + pos, n))
        {
            free(got);
            return 0;
        }
        free(got);
        pos = end;
        ++calls;
        if (calls > len + 1) return 0;
    }
    got = get_next_line(fd);
    if (got) { free(got); return 0; }
    got = get_next_line(fd);
    if (got) { free(got); return 0; }
    return 1;
}

int main(void)
{
    int id, fd, ok;
    size_t len;
    char path[] = "/tmp/gnl_mandatory_XXXXXX";
    char *data;
    for (id = 0; id < 100; ++id)
    {
        printf("Running test %d/100\n", id + 1);
        fflush(stdout);
        data = make_content(id, &len);
        if (!data) { fprintf(stderr, "Out of memory\n"); return 2; }
        strcpy(path, "/tmp/gnl_mandatory_XXXXXX");
        fd = mkstemp(path);
        if (fd < 0) { perror("mkstemp"); free(data); return 2; }
        unlink(path);
        ok = write_all(fd, data, len) && lseek(fd, 0, SEEK_SET) == 0;
        if (ok) ok = check_fd(fd, data, len);
        if (ok) ++passed;
        else { ++failed; printf("FAIL mandatory %03d (content length %zu)\n", id + 1, len); }
        close(fd);
        free(data);
    }
    printf("MANDATORY: %d/100 passed, %d failed\n", passed, failed);
    return failed ? 1 : 0;
}
