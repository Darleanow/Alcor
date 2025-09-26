#include "shell.h"
#include <lib/io/io.h>
#include <lib/standard/standard.h>
#include <lib/string/string.h>

#define MAX_ARGS 16
#define MAX_INPUT 256

static char input_buffer[MAX_INPUT];
static int input_pos = 0;

static int tokenize(char *input, char **args) {
  int argc = 0;
  char *token = input;

  while (*token && argc < MAX_ARGS - 1) {
    while (*token == ' ' || *token == '\t') {
      token++;
    }

    if (*token == '\0')
      break;

    args[argc++] = token;

    while (*token && *token != ' ' && *token != '\t') {
      token++;
    }

    if (*token) {
      *token = '\0';
      token++;
    }
  }

  args[argc] = NULL;
  return argc;
}

// Will be upgraded to an AST sooner or later
static void execute_command(char **args, int argc) {
  if (argc == 0)
    return;

  char *cmd = args[0];

  if (strcmp(cmd, "help") == 0) {
    printf("Available commands:\n");
    printf("  help    - Show this help\n");
    printf("  clear   - Clear screen\n");
    printf("  echo    - Print arguments\n");
    printf("  reboot  - Restart system\n");
  } else if (strcmp(cmd, "clear") == 0) {
    //    vga_console_clear();
  } else if (strcmp(cmd, "echo") == 0) {
    for (int i = 1; i < argc; i++) {
      printf("%s", args[i]);
      if (i < argc - 1)
        printf(" ");
    }
    printf("\n");
  } else if (strcmp(cmd, "reboot") == 0) {
    printf("Rebooting...\n");
    // TODO
  } else {
    printf("Unknown command: %s\n", cmd);
    printf("Type 'help' for available commands.\n");
  }
}

void shell_handle_char(char c) {
  if (c == '\n') {
    input_buffer[input_pos] = '\0';
    printf("\n");

    if (input_pos > 0) {
      char *args[MAX_ARGS];
      int argc = tokenize(input_buffer, args);
      execute_command(args, argc);
    }

    input_pos = 0;
    printf("shell> ");
  } else if (c == '\b') {
    // Backspace
    if (input_pos > 0) {
      input_pos--;
      printf("\b \b");
    }
  } else if (c >= 32 && c <= 126 && input_pos < MAX_INPUT - 1) {
    input_buffer[input_pos++] = c;
    printf("%c", c);
  }
}

void shell_init(void) {
  printf("Alcor Shell v1.0\n");
  printf("Type 'help' for available commands.\n");
  printf("devel> ");
}
