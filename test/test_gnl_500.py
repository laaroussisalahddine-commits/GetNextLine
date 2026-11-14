#!/usr/bin/env python3
"""500 deterministic get_next_line tests (bonus default, mandatory optional).

Run from your get_next_line source directory:
    python3 /path/to/test_gnl_500.py --mode bonus
    python3 /path/to/test_gnl_500.py --mode mandatory
    python3 /path/to/test_gnl_500.py --mode bonus --asan

Requires Python 3 and cc. Uses only Python's standard library.
"""
import argparse
import os
import random
import re
import shutil
import subprocess
import sys
import tempfile
from pathlib import Path

SIZES = [1, 2, 3, 7, 42, 1024, 9999, 10000000]
INVALID_SIZES = [0]
SEED = 133742
HARNESS = r'''
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include "GNL_HEADER"

void *__real_malloc(size_t n);
static int allocation_count;
static int fail_at;
void *__wrap_malloc(size_t n)
{
    ++allocation_count;
    if (fail_at > 0 && allocation_count == fail_at)
        return NULL;
    return __real_malloc(n);
}

static void print_line(int index, char *line)
{
    size_t i;
    if (!line)
    {
        printf("%d|NULL\n", index);
        return;
    }
    printf("%d|", index);
    i = 0;
    while (line[i])
    {
        printf("%02x", (unsigned char)line[i]);
        ++i;
    }
    putchar('\n');
    free(line);
}

static int normal_mode(int argc, char **argv)
{
    int fds[8];
    int active[8];
    int n, remaining, i, count;
    char *line;

    n = argc - 2;
    if (n < 1 || n > 8)
        return 3;
    for (i = 0; i < n; ++i)
    {
        fds[i] = open(argv[i + 2], O_RDONLY);
        if (fds[i] < 0)
            return (perror("open"), 3);
        active[i] = 1;
    }
    remaining = n;
    count = 0;
    while (remaining && count++ < 6000)
    {
        for (i = 0; i < n; ++i)
        {
            if (!active[i])
                continue;
            line = GNL(fds[i]);
            if (!line)
            {
                active[i] = 0;
                --remaining;
            }
            print_line(i, line);
        }
    }
    for (i = 0; i < n; ++i)
        close(fds[i]);
    return remaining ? 4 : 0;
}

int main(int argc, char **argv)
{
    char *line;
    if (argc < 2)
        return 3;
    if (!strcmp(argv[1], "run"))
        return normal_mode(argc, argv);
    if (!strcmp(argv[1], "stdin"))
    {
        int iterations = 0;
        int done = 0;
        do {
            line = GNL(STDIN_FILENO);
            done = (line == NULL);
            print_line(0, line);
            ++iterations;
        } while (!done && iterations < 6000);
        return iterations == 6000 ? 4 : 0;
    }
    if (!strcmp(argv[1], "fault"))
    {
        int fd;
        if (argc != 3)
            return 3;
        fd = open(argv[2], O_RDONLY);
        if (fd < 0)
            return 3;
        fail_at = atoi(getenv("GNL_FAIL_AT") ? getenv("GNL_FAIL_AT") : "0");
        line = GNL(fd);
        fail_at = 0;
        fprintf(stderr, "MALLOC_COUNT:%d\n", allocation_count);
        print_line(0, line);
        close(fd);
        return 0;
    }
    if (!strcmp(argv[1], "invalid_fd"))
    {
        print_line(0, GNL(-1));
        print_line(0, GNL(123456));
        return 0;
    }
    return 3;
}
'''


def expected_lines(data):
    if not data:
        return []
    chunks = data.split(b'\n')
    out = [part + b'\n' for part in chunks[:-1]]
    if chunks[-1]:
        out.append(chunks[-1])
    return out


def expected_trace(streams):
    lines = [expected_lines(s) for s in streams]
    positions = [0] * len(lines)
    alive = [True] * len(lines)
    trace = []
    while any(alive):
        for i in range(len(lines)):
            if not alive[i]:
                continue
            if positions[i] < len(lines[i]):
                trace.append(f'{i}|{lines[i][positions[i]].hex()}')
                positions[i] += 1
            else:
                trace.append(f'{i}|NULL')
                alive[i] = False
    return ('\n'.join(trace) + '\n').encode('ascii')


def payload(rng, size, variation):
    # Keep BUFFER_SIZE=1 tests inexpensive: joining repeatedly is often O(n^2).
    cap = 180 if size <= 3 else (650 if size <= 7 else 9000)
    boundary = max(1, abs(size))
    alphabet = b'abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789 _-\r\t'
    def letters(n):
        return bytes(rng.choice(alphabet) for _ in range(n))
    kind = variation % 12
    if kind == 0:
        return b''
    if kind == 1:
        return b'\n'
    if kind == 2:
        return b'X'
    if kind == 3:
        return b'\n\n\n\n'
    if kind == 4:
        return b'Hello\nWorld\nLast_without_newline'
    if kind == 5:
        return letters(min(cap, boundary - 1))
    if kind == 6:
        return letters(min(cap, boundary)) + b'\n'
    if kind == 7:
        return letters(min(cap, boundary + 1)) + b'\nZ'
    if kind == 8:
        return letters(min(cap, 2 * boundary + 1))
    if kind == 9:
        return b''.join(letters(rng.randint(0, 13)) + b'\n' for _ in range(rng.randint(3, 30)))
    if kind == 10:
        return b'\n'.join(letters(rng.randint(0, 50)) for _ in range(rng.randint(1, 12)))
    result = letters(rng.randint(0, cap))
    if result:
        result = result[:len(result)//2] + b'\n' + result[len(result)//2:]
    return result


def run_program(command, *, input_data=None, env=None, timeout=8):
    try:
        return subprocess.run(command, input=input_data, stdout=subprocess.PIPE,
                              stderr=subprocess.PIPE, env=env, timeout=timeout)
    except subprocess.TimeoutExpired:
        raise RuntimeError(f'TIMEOUT after {timeout}s')


def main():
    parser = argparse.ArgumentParser(description='Run 500 deterministic GNL tests')
    parser.add_argument('--mode', choices=['bonus', 'mandatory'], default='bonus')
    parser.add_argument('--asan', action='store_true', help='Compile with ASan+UBSan (recommended)')
    parser.add_argument('--src', default='.', help='Folder with your GNL .c and .h files')
    parser.add_argument('--verbose', action='store_true')
    args = parser.parse_args()
    src = Path(args.src).resolve()
    if args.mode == 'bonus':
        sources = ['get_next_line_bonus.c', 'get_next_line_utils_bonus.c']
        header, func = 'get_next_line_bonus.h', 'get_next_line_bonus'
    else:
        sources = ['get_next_line.c', 'get_next_line_utils.c']
        header, func = 'get_next_line.h', 'get_next_line'
    missing = [name for name in sources + [header] if not (src / name).is_file()]
    if missing:
        print('ERROR: Missing files:', ', '.join(missing))
        print('Use --src /path/to/your/project if needed.')
        return 2
    if not shutil.which('cc'):
        print('ERROR: cc not found')
        return 2

    rng = random.Random(SEED)
    passed = 0
    failures = []
    compilation = {}
    with tempfile.TemporaryDirectory(prefix='gnl_500_') as directory:
        work = Path(directory)
        harness = work / 'gnl_harness.c'
        harness.write_text(HARNESS.replace('GNL_HEADER', header).replace('GNL(', func + '('), encoding='utf-8')

        def executable(bs):
            if bs in compilation:
                return compilation[bs]
            name = 'default' if bs is None else str(bs).replace('-', 'neg')
            out = work / ('gnl_' + name)
            cmd = ['cc', '-std=c99', '-Wall', '-Wextra', '-Werror', '-g', '-O0',
                   '-fno-omit-frame-pointer', '-I', str(src)]
            if args.asan:
                cmd += ['-fsanitize=address,undefined']
            if bs is not None:
                cmd += [f'-DBUFFER_SIZE={bs}']
            cmd += [str(src / x) for x in sources] + [str(harness),
                    '-Wl,--wrap=malloc', '-o', str(out)]
            build = run_program(cmd, timeout=35)
            if build.returncode:
                print('COMPILATION FAIL:', ' '.join(cmd))
                print(build.stderr.decode('utf-8', errors='replace'))
                sys.exit(2)
            compilation[bs] = str(out)
            return str(out)

        def check(title, bs, mode, streams=None, fault_index=None):
            nonlocal passed
            n = passed + len(failures) + 1
            exe = executable(bs)
            env = os.environ.copy()
            if args.asan:
                env['ASAN_OPTIONS'] = 'detect_leaks=1:halt_on_error=1'
                env['UBSAN_OPTIONS'] = 'halt_on_error=1'
            files = []
            if streams is not None:
                for i, data in enumerate(streams):
                    f = work / f'input_{n}_{i}.txt'
                    f.write_bytes(data)
                    files.append(str(f))
            if mode == 'run':
                command = [exe, 'run', *files]
                wanted = expected_trace(streams)
                input_data = None
            elif mode == 'stdin':
                command = [exe, 'stdin']
                wanted = expected_trace(streams)
                input_data = streams[0]
            elif mode == 'invalid':
                command = [exe, 'invalid_fd']
                wanted = b'0|NULL\n0|NULL\n'
                input_data = None
            elif mode == 'invalid_bs':
                command = [exe, 'run', *files]
                wanted = ('0|NULL\n' * len(streams)).encode('ascii')
                input_data = None
            elif mode == 'fault':
                command = [exe, 'fault', *files]
                input_data = None
                # First measure how many malloc calls happen in the first successful call.
                baseline = run_program(command, env=env)
                count_match = re.search(rb'MALLOC_COUNT:(\d+)', baseline.stderr)
                lines = expected_lines(streams[0])
                correct_baseline = f'0|{lines[0].hex()}\n'.encode() if lines else b'0|NULL\n'
                if baseline.returncode or not count_match or baseline.stdout != correct_baseline:
                    failures.append((n, title + ' (baseline)', bs, 'Baseline broken',
                                     baseline.stderr.decode(errors='replace')[:1200],
                                     baseline.stdout[:200], correct_baseline[:200]))
                    return
                allocation_total = int(count_match.group(1))
                if not allocation_total:
                    failures.append((n, title, bs, 'malloc wrapper did not intercept allocations', '', b'', b''))
                    return
                env['GNL_FAIL_AT'] = str(1 + ((fault_index - 1) % allocation_total))
                wanted = b'0|NULL\n'
            else:
                raise AssertionError(mode)
            try:
                result = run_program(command, input_data=input_data, env=env)
                ok = result.returncode == 0 and result.stdout == wanted
                if not ok:
                    failures.append((n, title, bs, f'exit={result.returncode}',
                                     result.stderr.decode(errors='replace')[:1500],
                                     result.stdout[:200], wanted[:200]))
                else:
                    passed += 1
            except RuntimeError as e:
                failures.append((n, title, bs, str(e), '', b'', wanted[:200]))
            if args.verbose and n % 25 == 0:
                print(f'Checked {n}/500 ...')
            elif n % 100 == 0:
                print(f'Checked {n}/500 ...')

        # 320 single FD tests: many patterns, buffer boundaries, invalid sizes.
        for i in range(320):
            if i < 10:
                bs = 0
                data = b'Hello\nworld\n'
                check('invalid BUFFER_SIZE', bs, 'invalid_bs', [data])
            elif i < 18:
                bs = SIZES[(i - 10) % len(SIZES)]
                check('invalid file descriptors', bs, 'invalid')
            else:
                bs = SIZES[(i - 18) % len(SIZES)] if i % 13 else None
                data = payload(rng, bs if bs is not None else 1024, i)
                check(f'single fd variant={i % 12} bytes={len(data)}', bs, 'run', [data])

        # 120 multi-FD tests. Mandatory has one static stash, so use single-FD tests instead.
        for i in range(120):
            bs = SIZES[i % len(SIZES)]
            if args.mode == 'bonus':
                count = 2 + (i % 4)
                streams = [payload(rng, bs, i * 7 + j) for j in range(count)]
                check(f'interleaved {count} fds', bs, 'run', streams)
            else:
                check('mandatory one fd', bs, 'run', [payload(rng, bs, i * 7)])

        # 40 tests of stdin, which is a pipe (not a regular file).
        for i in range(40):
            bs = SIZES[i % len(SIZES)]
            data = payload(rng, bs, i * 11)
            check(f'stdin pipe bytes={len(data)}', bs, 'stdin', [data])

        # 20 actual, injected malloc failures; fail indices are guaranteed reached.
        for i in range(20):
            bs = SIZES[i % 6]  # small sizes: several mallocs per line
            data = b'ABCDEFGHIJKLMN\nSecond line\n'
            check('forced malloc failure', bs, 'fault', [data], fault_index=i + 1)

    print('\n' + '=' * 52)
    print(f'MODE: {args.mode} | SEED: {SEED} | ASAN: {"ON" if args.asan else "OFF"}')
    print(f'RESULT: {passed}/500 PASS | {len(failures)} FAIL')
    print('=' * 52)
    for number, title, bs, reason, stderr, actual, expected in failures[:15]:
        print(f'\nFAIL TEST #{number}: {title} (BUFFER_SIZE={bs})')
        print('Reason:', reason)
        print('Expected:', repr(expected))
        print('Actual:  ', repr(actual))
        if stderr:
            print('stderr:', stderr)
    if len(failures) > 15:
        print(f'\n... plus {len(failures) - 15} other failures')
    return 0 if not failures else 1


if __name__ == '__main__':
    sys.exit(main())
