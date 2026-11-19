*This project has been created as part of the 42 curriculum by slaarous.*

# Get Next Line

A C function that reads a file descriptor **one line at a time**, while preserving any characters that belong to the next line. This repository includes the **mandatory** implementation and the **bonus** implementation for handling multiple file descriptors independently.

## Description

The goal of **Get Next Line (GNL)** is to implement:

```c
char *get_next_line(int fd);
```

Each call returns the next available line from `fd`, including the trailing newline (`\n`) when present. If the last line does not end with a newline, that final line is still returned. The function returns `NULL` at the end of the input or when an error occurs. The returned string is dynamically allocated, so **the caller must free it**.

The project covers:

- File descriptors and the `read()` system call.
- Static variables and data retained between function calls.
- Dynamic allocation, ownership, and error handling.
- Buffered reading and handling lines longer than `BUFFER_SIZE`.
- Managing independent read states for multiple file descriptors (bonus).

## Algorithm: Explanation and Justification

### Mandatory implementation

The mandatory solution uses a **static pointer named `stash`** to retain unread characters between calls to `get_next_line()`.

1. **Validate the input.** Reject a negative file descriptor or a non-positive `BUFFER_SIZE`.
2. **Read only when needed.** If `stash` does not already contain a newline, `read_file()` allocates a temporary buffer of `BUFFER_SIZE + 1` bytes and calls `read()`.
3. **Accumulate the data.** After null-terminating the newly read bytes, `get_join()` combines them with `stash`. The old allocation and temporary buffer are released as part of this operation.
4. **Stop at the right moment.** Reading stops once a newline is available, `read()` reports end-of-file, or an error occurs. Data after a newline is not discarded.
5. **Extract one line.** `extract_line()` allocates a string containing the first line, including `\n` if present.
6. **Keep the remainder.** `update_stash()` removes the returned line from the saved data, keeping any remaining characters for the next call. If nothing remains, it frees the stash.
7. **Return the result.** The caller receives the allocated line and is responsible for calling `free()` on it.

For example, if one read obtains:

```text
Hello\nWorld\n
```

The first call returns `"Hello\n"` and keeps `"World\n"` in `stash`. The next call can return `"World\n"` without reading those bytes again.

**Why this algorithm?** The stash preserves data across calls while allowing the function to return exactly one line at a time. Reading in configurable chunks avoids having to read an entire file in advance. The temporary buffer is reused conceptually for each read, and the persistent stash holds only data that has not yet been returned.

**Trade-off:** Repeated string concatenation copies the accumulated data again and again. For very long lines, especially with a small `BUFFER_SIZE`, this may take quadratic time in the line length. The approach is nevertheless simple, clear, and appropriate for practising memory management and buffered input.

### Bonus implementation: multiple file descriptors

The bonus uses **one static variable**, an array of pointers:

```c
static char *stash[MAX_INDEX];
```

Each file descriptor has its own slot: `stash[fd]`. Calling the function on `fd = 3`, then `fd = 4`, then `fd = 3` again preserves the separate unread data for each descriptor. The algorithm for reading, extracting, and updating a line is otherwise the same as the mandatory implementation.

The implementation checks that `fd` is within the configured array range (`0 <= fd < MAX_INDEX`, with a default `MAX_INDEX` of `1024`). This is a deliberate fixed-size design choice: it makes per-descriptor state straightforward to access, but descriptors outside that range are not supported.

### Memory and edge cases

- `BUFFER_SIZE` can be smaller or larger than the line being read.
- Empty input returns `NULL`.
- A final line without `\n` is returned before subsequent calls return `NULL`.
- Read errors and allocation failures must not leave owned allocations behind.
- The implementation uses `read()`, `malloc()`, and `free()` rather than a pre-existing line-reading library.

The project is intended for text input; behavior with binary data is not guaranteed by the subject.

## Instructions

### Files

Mandatory:

```text
get_next_line.c
get_next_line_utils.c
get_next_line.h
```

Bonus:

```text
get_next_line_bonus.c
get_next_line_utils_bonus.c
get_next_line_bonus.h
```

### Compilation

Compile the mandatory version with a small program of your own (for example, `main.c`):

```bash
cc -Wall -Wextra -Werror -D BUFFER_SIZE=42 \
   get_next_line.c get_next_line_utils.c main.c -o gnl
```

To compile the bonus version:

```bash
cc -Wall -Wextra -Werror -D BUFFER_SIZE=42 \
   get_next_line_bonus.c get_next_line_utils_bonus.c main.c -o gnl_bonus
```

The `main.c` file must call the corresponding function (`get_next_line()` for mandatory or `get_next_line_bonus()` for bonus). The subject also requires compilation without `-D BUFFER_SIZE`; both headers provide a default value.

### Minimal usage example

Here is an example for the mandatory version. Save it as `main.c`:

```c
#include "get_next_line.h"
#include <fcntl.h>
#include <stdio.h>

int main(int argc, char **argv)
{
    int     fd;
    char    *line;

    if (argc != 2)
        return (1);
    fd = open(argv[1], O_RDONLY);
    if (fd < 0)
        return (perror("open"), 1);
    while ((line = get_next_line(fd)) != NULL)
    {
        printf("%s", line);
        free(line);
    }
    close(fd);
    return (0);
}
```

Compile using the mandatory command above and run:

```bash
./gnl example.txt
```

For bonus usage, change the included header to `get_next_line_bonus.h` and call `get_next_line_bonus(fd)` instead; then use the bonus compilation command.

For debugging memory errors, you can additionally compile with `-g -fsanitize=address,undefined` (on a supported compiler) and run the resulting program.

## Resources

### References

- [Linux `read(2)` manual](https://man7.org/linux/man-pages/man2/read.2.html) — `read()` return values, EOF, and errors.
- [Linux `malloc(3)` manual](https://man7.org/linux/man-pages/man3/malloc.3.html) — dynamic memory allocation and deallocation.
- [GNU C Library Manual: Memory Allocation](https://www.gnu.org/software/libc/manual/html_node/Memory-Allocation.html) — managing heap memory in C.
- The **42 Get Next Line subject (v14.3)** — project requirements, allowed functions, and expected behavior.

### AI usage

AI was consulted for **theoretical explanations and discussion** of C concepts relevant to this project, including static storage duration, file descriptors, EOF handling, memory ownership, and allocation/error edge cases.

