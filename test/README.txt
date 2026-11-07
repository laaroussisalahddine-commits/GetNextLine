Place the two tester .c files at the root of your get_next_line repository.

MANDATORY:
cc -Wall -Wextra -Werror -I. -D BUFFER_SIZE=42 tester_mandatory.c get_next_line.c get_next_line_utils.c -o test_mandatory
./test_mandatory

BONUS:
cc -Wall -Wextra -Werror -I. -D BUFFER_SIZE=42 tester_bonus.c get_next_line_bonus.c get_next_line_utils_bonus.c -o test_bonus
./test_bonus

Repeat with BUFFER_SIZE=1, 2, 7, 1024, 4096 (and 100000).
If using address sanitizer, add -fsanitize=address -g to the cc command.
Each tester has 100 numbered test scenarios. The bonus tester opens and interleaves three files per scenario.
Note: file descriptor validity/error paths, invalid BUFFER_SIZE, malloc-failure injection, and embedded NUL content are not included in these 100 scenarios.
