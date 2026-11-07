/* 100 deterministic tests for get_next_line_bonus (multi-fd interleaving). */
#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include "get_next_line_bonus.h"

static int passed, failed;

typedef struct s_stream
{
    int fd;
    char *data;
    size_t len;
    size_t pos;
    int eof_checks;
} t_stream;

static char *make_content(int id, int stream, size_t *out_len)
{
    size_t len, i;
    char *s;
    int kind = (id + stream * 7) % 12;
    if (kind == 0) len = 0;
    else if (kind == 1) len = 1;
    else if (kind == 2) len = 2;
    else if (kind == 3) len = 8192;
    else if (kind == 4) len = 8193;
    else if (kind == 5) len = 20000;
    else len = (size_t)((id * 131 + stream * 577 + id * id * 17) % 6000 + 1);
    s = malloc(len + 1);
    if (!s) return NULL;
    for (i = 0; i < len; ++i)
    {
        if (kind == 2) s[i] = '\n';
        else if (kind == 6) s[i] = 'Q';
        else if (kind == 7) s[i] = (i % 2) ? '\n' : 'a';
        else if (kind == 8) s[i] = (i % 31 == 0) ? '\n' : 'b';
        else if (kind == 9) s[i] = (i % 255 == 0) ? '\n' : 'c';
        else if (kind == 10) s[i] = (i % 4096 == 0) ? '\n' : 'd';
        else s[i] = (i % 97 == 0) ? '\n' : 'e';
    }
    if (len && (id + stream) % 3 == 0) s[len - 1] = '\n';
    s[len] = 0;
    *out_len = len;
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

/* Return 1 if the next call gives exactly the expected line/EOF. */
static int check_next(t_stream *s)
{
    char *got;
    size_t end = s->pos, n;
    got = get_next_line_bonus(s->fd);
    if (s->pos == s->len)
    {
        if (got) { free(got); return 0; }
        ++s->eof_checks;
        return 1;
    }
    while (end < s->len && s->data[end] != '\n') ++end;
    if (end < s->len) ++end;
    n = end - s->pos;
    if (!got || strlen(got) != n || memcmp(got, s->data + s->pos, n))
    {
        free(got);
        return 0;
    }
    free(got);
    s->pos = end;
    return 1;
}

static int one_test(int id)
{
    t_stream s[3] = {{0}};
    int i, k, ok = 1, active, step = 0;
    char path[] = "/tmp/gnl_bonus_XXXXXX";
    for (i = 0; i < 3; ++i) s[i].fd = -1;
    for (i = 0; i < 3; ++i)
    {
        printf("Running test %d/100\n", id + 1);
        fflush(stdout);
        s[i].data = make_content(id, i, &s[i].len);
        if (!s[i].data) { ok = 0; goto cleanup; }
        strcpy(path, "/tmp/gnl_bonus_XXXXXX");
        s[i].fd = mkstemp(path);
        if (s[i].fd < 0) { ok = 0; goto cleanup; }
        unlink(path);
        if (!write_all(s[i].fd, s[i].data, s[i].len)
            || lseek(s[i].fd, 0, SEEK_SET) != 0) { ok = 0; goto cleanup; }
    }
    /* Round-robin plus nonuniform order tests that static stash is per-fd. */
    do
    {
        active = 0;
        for (k = 0; k < 3; ++k)
        {
            i = (k + id + step) % 3;
            if (s[i].pos < s[i].len || s[i].eof_checks < 2)
            {
                if (!check_next(&s[i])) { ok = 0; goto cleanup; }
                ++active;
            }
        }
        ++step;
        if (step > 30000) { ok = 0; goto cleanup; }
    } while (active);
cleanup:
    for (i = 0; i < 3; ++i)
    {
        if (s[i].fd >= 0) close(s[i].fd);
        free(s[i].data);
    }
    return ok;
}

int main(void)
{
    int id;
    for (id = 0; id < 100; ++id)
    {
        if (one_test(id)) ++passed;
        else { ++failed; printf("FAIL bonus %03d\n", id + 1); }
    }
    printf("BONUS: %d/100 passed, %d failed\n", passed, failed);
    return failed ? 1 : 0;
}
