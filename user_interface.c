#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "document.h"
#include "sysexits.h"

#define EX_OSERR 71
#define EX_USAGE 64

void process_command(Document *doc, char *command) {
    int paragraph_num, line_num;
    char line[MAX_STR_SIZE + 1], filename[MAX_STR_SIZE + 1];
    char target[MAX_STR_SIZE + 1], replacement[MAX_STR_SIZE + 1];

    while (*command == ' ' || *command == '\t') command++;
    if (strlen(command) == 0 || command[0] == '#') return;

    if (sscanf(command, "add_paragraph_after %d", &paragraph_num) == 1) {
        if (paragraph_num < 0) {
            printf("Invalid Command\n");
        } else if (add_paragraph_after(doc, paragraph_num) == FAILURE) {
            printf("add_paragraph_after failed\n");
        }
    } else if (strncmp(command, "add_paragraph_after", 19) == 0) {
        printf("Invalid Command\n");
    } else if (sscanf(command, "add_line_after %d %d *%[^\n]", &paragraph_num, &line_num, line) == 3) {
        if (paragraph_num <= 0 || line_num < 0) {
            printf("Invalid Command\n");
        } else if (add_line_after(doc, paragraph_num, line_num, line) == FAILURE) {
            printf("add_line_after failed\n");
        }
    } else if (strncmp(command, "add_line_after", 14) == 0) {
        printf("Invalid Command\n");
    } else if (sscanf(command, "append_line %d *%[^\n]", &paragraph_num, line) == 2) {
        if (paragraph_num <= 0) {
            printf("Invalid Command\n");
        } else if (append_line(doc, paragraph_num, line) == FAILURE) {
            printf("append_line failed\n");
        }
    } else if (strncmp(command, "append_line", 11) == 0) {
        printf("Invalid Command\n");
    } else if (sscanf(command, "remove_line %d %d", &paragraph_num, &line_num) == 2) {
        if (paragraph_num <= 0 || line_num <= 0) {
            printf("Invalid Command\n");
        } else if (remove_line(doc, paragraph_num, line_num) == FAILURE) {
            printf("remove_line failed\n");
        }
    } else if (strncmp(command, "remove_line", 11) == 0) {
        printf("Invalid Command\n");
    } else if (strcmp(command, "print_document") == 0) {
        print_document(doc);
    } else if (strncmp(command, "print_document", 14) == 0) {
        printf("Invalid Command\n");
    } else if (strcmp(command, "quit") == 0 || strcmp(command, "exit") == 0) {
        exit(0);
    } else if (strncmp(command, "quit", 4) == 0 || strncmp(command, "exit", 4) == 0) {
        printf("Invalid Command\n");
    } else if (sscanf(command, "load_file %s", filename) == 1) {
        if (load_file(doc, filename) == FAILURE) {
            printf("load_file failed\n");
        }
    } else if (strncmp(command, "load_file", 9) == 0) {
        printf("Invalid Command\n");
    } else if (sscanf(command, "save_document %s", filename) == 1) {
        if (save_document(doc, filename) == FAILURE) {
            printf("save_document failed\n");
        }
    } else if (strncmp(command, "save_document", 13) == 0) {
        printf("Invalid Command\n");
    } else if (sscanf(command, "replace_text \"%[^\"]\" \"%[^\"]\"", target, replacement) == 2) {
        if (replace_text(doc, target, replacement) == FAILURE) {
            printf("replace_text failed\n");
        }
    } else if (strncmp(command, "replace_text", 12) == 0) {
        printf("Invalid Command\n");
    } else if (sscanf(command, "highlight_text \"%[^\"]\"", target) == 1) {
        highlight_text(doc, target);
    } else if (strncmp(command, "highlight_text", 14) == 0) {
        printf("Invalid Command\n");
    } else if (sscanf(command, "remove_text \"%[^\"]\"", target) == 1) {
        remove_text(doc, target);
    } else if (strncmp(command, "remove_text", 11) == 0) {
        printf("Invalid Command\n");
    } else if (strcmp(command, "reset_document") == 0) {
        reset_document(doc);
    } else if (strncmp(command, "reset_document", 14) == 0) {
        printf("Invalid Command\n");
    } else {
        printf("Invalid Command\n");
    }
}

int main(int argc, char *argv[]) {
    Document doc;
    char command[MAX_STR_SIZE + 1];

    if (init_document(&doc, "main_document") == FAILURE) {
        fprintf(stderr, "Failed to initialize document.\n");
        return EX_OSERR;
    }

    if (argc == 1) {
        printf("> ");
        while (fgets(command, sizeof(command), stdin)) {
            command[strcspn(command, "\n")] = 0;
            process_command(&doc, command);
            printf("> ");
        }
    } else if (argc == 2) {
        FILE *file = fopen(argv[1], "r");
        if (file == NULL) {
            fprintf(stderr, "%s cannot be opened.\n", argv[1]);
            return EX_OSERR;
        }
        while (fgets(command, sizeof(command), file) != NULL) {
            command[strcspn(command, "\n")] = 0;
            process_command(&doc, command);
        }
        fclose(file);
    } else {
        fprintf(stderr, "Usage: user_interface\nUsage: user_interface <filename>\n");
        return EX_USAGE;
    }

    return 0;
}
