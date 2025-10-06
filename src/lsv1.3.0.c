/*
 * ls-v1.3.0 — Horizontal Column Display (-x Option)
 * Builds on v1.2.0, adds -x flag for horizontal layout.
 */
#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <errno.h>

extern int errno;

// Forward declarations
void print_column_listing(const char *dirpath);
void print_horizontal_listing(const char *dirpath);
void print_long_listing(const char *dirpath); // placeholder for compatibility

int main(int argc, char *argv[]) {
    int opt;
    int mode = 0;  // 0 = default (down-across), 1 = long (-l), 2 = horizontal (-x)

    // Parse command-line options
    while ((opt = getopt(argc, argv, "lx")) != -1) {
        switch (opt) {
            case 'l':
                mode = 1;
                break;
            case 'x':
                mode = 2;
                break;
            default:
                fprintf(stderr, "Usage: %s [-l | -x] [directory]\n", argv[0]);
                exit(EXIT_FAILURE);
        }
    }

    const char *dir = (optind < argc) ? argv[optind] : ".";

    if (mode == 1)
        print_long_listing(dir);
    else if (mode == 2)
        print_horizontal_listing(dir);
    else
        print_column_listing(dir);

    return 0;
}

/* -------------------- Default (Down-Then-Across) -------------------- */
void print_column_listing(const char *dirpath) {
    DIR *dir = opendir(dirpath);
    if (!dir) {
        perror("opendir");
        return;
    }

    struct dirent *entry;
    int count = 0, capacity = 50;

    char **filenames = malloc(capacity * sizeof(char *));
    if (!filenames) {
        perror("malloc");
        closedir(dir);
        return;
    }

    int maxlen = 0;
    while ((entry = readdir(dir)) != NULL) {
        if (entry->d_name[0] == '.')
            continue;

        if (count >= capacity) {
            capacity *= 2;
            filenames = realloc(filenames, capacity * sizeof(char *));
            if (!filenames) {
                perror("realloc");
                closedir(dir);
                return;
            }
        }

        filenames[count] = strdup(entry->d_name);
        if (!filenames[count]) {
            perror("strdup");
            closedir(dir);
            return;
        }

        int len = strlen(entry->d_name);
        if (len > maxlen)
            maxlen = len;
        count++;
    }
    closedir(dir);

    if (count == 0) {
        free(filenames);
        return;
    }

    struct winsize w;
    int term_width = 80;
    if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &w) == 0)
        term_width = w.ws_col;

    int spacing = 2;
    int col_width = maxlen + spacing;
    int num_cols = term_width / col_width;
    if (num_cols < 1)
        num_cols = 1;
    int num_rows = (count + num_cols - 1) / num_cols;

    for (int r = 0; r < num_rows; r++) {
        for (int c = 0; c < num_cols; c++) {
            int index = c * num_rows + r;
            if (index < count)
                printf("%-*s", col_width, filenames[index]);
        }
        printf("\n");
    }

    for (int i = 0; i < count; i++)
        free(filenames[i]);
    free(filenames);
}

/* -------------------- Horizontal (Row-Major, -x) -------------------- */
void print_horizontal_listing(const char *dirpath) {
    DIR *dir = opendir(dirpath);
    if (!dir) {
        perror("opendir");
        return;
    }

    struct dirent *entry;
    int count = 0, capacity = 50;

    char **filenames = malloc(capacity * sizeof(char *));
    if (!filenames) {
        perror("malloc");
        closedir(dir);
        return;
    }

    int maxlen = 0;
    while ((entry = readdir(dir)) != NULL) {
        if (entry->d_name[0] == '.')
            continue;

        if (count >= capacity) {
            capacity *= 2;
            filenames = realloc(filenames, capacity * sizeof(char *));
            if (!filenames) {
                perror("realloc");
                closedir(dir);
                return;
            }
        }

        filenames[count] = strdup(entry->d_name);
        if (!filenames[count]) {
            perror("strdup");
            closedir(dir);
            return;
        }

        int len = strlen(entry->d_name);
        if (len > maxlen)
            maxlen = len;
        count++;
    }
    closedir(dir);

    if (count == 0) {
        free(filenames);
        return;
    }

    struct winsize w;
    int term_width = 80;
    if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &w) == 0)
        term_width = w.ws_col;

    int spacing = 2;
    int col_width = maxlen + spacing;
    int current_width = 0;

    for (int i = 0; i < count; i++) {
        int len = strlen(filenames[i]);
        if (current_width + col_width > term_width) {
            printf("\n");
            current_width = 0;
        }
        printf("%-*s", col_width, filenames[i]);
        current_width += col_width;
    }
    printf("\n");

    for (int i = 0; i < count; i++)
        free(filenames[i]);
    free(filenames);
}

/* -------------------- Placeholder for Long Listing -------------------- */
void print_long_listing(const char *dirpath) {
    printf("Long listing (-l) not implemented in this version.\n");
}
