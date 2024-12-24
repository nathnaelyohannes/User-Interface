#include <stdio.h>
#include <string.h>
#include "document.h"

int init_document(Document *doc, const char *name) {

    if (doc == NULL || name == NULL || strlen(name) > MAX_STR_SIZE) {

        return FAILURE;
    }

    strcpy(doc->name, name);
    doc->number_of_paragraphs = 0;
    return SUCCESS;
}

int reset_document(Document *doc) {

    if (doc == NULL) {

        return FAILURE;
    }

    doc->number_of_paragraphs = 0;
    return SUCCESS;
}

int print_document(Document *doc) {

    int i, j;
    
    if (doc == NULL) {

        return FAILURE;
    }

    printf("Document name: \"%s\"\n", doc->name);
    printf("Number of Paragraphs: %d\n", doc->number_of_paragraphs);

    for (i = 0; i < doc->number_of_paragraphs; i++) {

        for (j = 0; j < doc->paragraphs[i].number_of_lines; j++) {

            printf("%s\n", doc->paragraphs[i].lines[j]);
        }
        
        if (i < doc->number_of_paragraphs - 1) {
            printf("\n");
        }
    }

    return SUCCESS;
}

int add_paragraph_after(Document *doc, int paragraph_number) {

    int i;

    if (doc == NULL || doc->number_of_paragraphs >= MAX_PARAGRAPHS || paragraph_number > doc->number_of_paragraphs) {
        return FAILURE;
    }

    for (i = doc->number_of_paragraphs; i > paragraph_number; i--) {
        doc->paragraphs[i] = doc->paragraphs[i - 1];
    }

    doc->paragraphs[paragraph_number].number_of_lines = 0;
    doc->number_of_paragraphs++;
    return SUCCESS;
}

int add_line_after(Document *doc, int paragraph_number, int line_number, const char *new_line) {

    Paragraph *paragraph;
    int i;

    if (doc == NULL || new_line == NULL || paragraph_number <= 0 || paragraph_number > doc->number_of_paragraphs ||
        line_number < 0 || line_number > doc->paragraphs[paragraph_number - 1].number_of_lines ||
        doc->paragraphs[paragraph_number - 1].number_of_lines >= MAX_PARAGRAPH_LINES) {
        return FAILURE;
    }

    paragraph = &doc->paragraphs[paragraph_number - 1];

    for (i = paragraph->number_of_lines; i > line_number; i--) {
        strcpy(paragraph->lines[i], paragraph->lines[i - 1]);
    }

    strncpy(paragraph->lines[line_number], new_line, MAX_STR_SIZE);
    paragraph->lines[line_number][MAX_STR_SIZE] = '\0';
    paragraph->number_of_lines++;

    return SUCCESS;
}

int append_line(Document *doc, int paragraph_number, const char *new_line) {

    Paragraph *paragraph;

    if (doc == NULL || new_line == NULL || paragraph_number <= 0 || paragraph_number > doc->number_of_paragraphs ||
        doc->paragraphs[paragraph_number - 1].number_of_lines >= MAX_PARAGRAPH_LINES) {
        return FAILURE;
    }

    paragraph = &doc->paragraphs[paragraph_number - 1];
    strncpy(paragraph->lines[paragraph->number_of_lines], new_line, MAX_STR_SIZE);
    paragraph->lines[paragraph->number_of_lines][MAX_STR_SIZE] = '\0';
    paragraph->number_of_lines++;
    return SUCCESS;
}

int remove_line(Document *doc, int paragraph_number, int line_number) {
    int i;
    Paragraph *paragraph;

    if (doc == NULL || paragraph_number <= 0 || paragraph_number > doc->number_of_paragraphs ||
        line_number <= 0 || line_number > doc->paragraphs[paragraph_number - 1].number_of_lines) {
        return FAILURE;
    }

    paragraph = &doc->paragraphs[paragraph_number - 1];
    for (i = line_number - 1; i < paragraph->number_of_lines - 1; i++) {
        strcpy(paragraph->lines[i], paragraph->lines[i + 1]);
    }
    paragraph->number_of_lines--;
    return SUCCESS;
}

int load_file(Document *doc, const char *filename) {

    FILE *file;
    int current_paragraph = 0;
    int paragraph_added = 0;
    char line[MAX_STR_SIZE + 1];

    if (doc == NULL || filename == NULL) {
        return FAILURE;
    }

    file = fopen(filename, "r");
    if (file == NULL) {
        return FAILURE;
    }

    add_paragraph_after(doc, current_paragraph);
    paragraph_added = 1;

    while (fgets(line, MAX_STR_SIZE + 1, file) != NULL) {
        line[strcspn(line, "\n")] = '\0';

        if (strlen(line) == 0 || isspace(line[0])) {
            if (paragraph_added) {
                current_paragraph++;
                if (current_paragraph >= MAX_PARAGRAPHS) {
                    fclose(file);
                    return FAILURE;
                }
                add_paragraph_after(doc, current_paragraph);
                paragraph_added = 0;
            }
        } else {
            append_line(doc, current_paragraph + 1, line);
            paragraph_added = 1;
        }
    }

    fclose(file);
    return SUCCESS;
}



int save_document(Document *doc, const char *filename) {

    FILE *file;
    int i, j;

    if (doc == NULL || filename == NULL) {
        return FAILURE;
    }

    file = fopen(filename, "w");
    if (file == NULL) {
        return FAILURE;
    }

    for (i = 0; i < doc->number_of_paragraphs; i++) {
        for (j = 0; j < doc->paragraphs[i].number_of_lines; j++) {
            fprintf(file, "%s\n", doc->paragraphs[i].lines[j]);
        }
        if (i < doc->number_of_paragraphs - 1) {
            fprintf(file, "\n");
        }
    }
    fclose(file);
    return SUCCESS;
}


int replace_text(Document *doc, const char *target, const char *replacement) {
    int i, j;
    char buffer[MAX_STR_SIZE + 1];
    
    if (doc == NULL || target == NULL || replacement == NULL) {
        return FAILURE;
    }

    for (i = 0; i < doc->number_of_paragraphs; i++) {
        for (j = 0; j < doc->paragraphs[i].number_of_lines; j++) {
            char *position = doc->paragraphs[i].lines[j];
            while ((position = strstr(position, target)) != NULL) {

                strncpy(buffer, doc->paragraphs[i].lines[j], position - doc->paragraphs[i].lines[j]);
                buffer[position - doc->paragraphs[i].lines[j]] = '\0';

                strncat(buffer, replacement, MAX_STR_SIZE - strlen(buffer));

                strncat(buffer, position + strlen(target), MAX_STR_SIZE - strlen(buffer));

                strncpy(doc->paragraphs[i].lines[j], buffer, MAX_STR_SIZE);
                doc->paragraphs[i].lines[j][MAX_STR_SIZE] = '\0';

                position += strlen(replacement);
            }
        }
    }
    return SUCCESS;
}


int highlight_text(Document *doc, const char *target) {
    char highlighted[MAX_STR_SIZE + 1];

    if (doc == NULL || target == NULL) {
        return FAILURE;
    }

    snprintf(highlighted, MAX_STR_SIZE, "[%s]", target);

    return replace_text(doc, target, highlighted);
}



int remove_text(Document *doc, const char *target) {
    if (doc == NULL || target == NULL) {
        return FAILURE;
    }

    return replace_text(doc, target, "");
}

