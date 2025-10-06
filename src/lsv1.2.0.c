/*
 * ls-v1.2.0 — Column Display (Down Then Across)
 * Based on previous versions of ls.
 * Implements multi-column output that adapts to terminal width.
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

void print_column_listing(const char *dirpath);

int main(int argc, char *argv[]) {
    const char *dir = (argc > 1) ? argv[1] : ".";
    print_column_listing(dir);
    return 0;
}

void print_column_listing(const char *dirpath) {
    DIR *dir = opendir(dirpath);
    if (!dir) {
        perror("opendir");
        return;
    }

    struct dirent *entry;
    int count = 0, capacity = 50;

    // Dynamically store filenames
    char **filenames = malloc(capacity * sizeof(char *));
    if (!filenames) {
        perror("malloc");
        closedir(dir);
        return;
    }

    int maxlen = 0;
    while ((entry = readdir(dir)) != NULL) {
        if (entry->d_name[0] == '.')
            continue;  // Skip hidden files

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

    // Get terminal width
    struct winsize w;
    int term_width = 80; // fallback
    if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &w) == 0)
        term_width = w.ws_col;

    int spacing = 2;
    int col_width = maxlen + spacing;
    int num_cols = term_width / col_width;
    if (num_cols < 1)
        num_cols = 1;

    int num_rows = (count + num_cols - 1) / num_cols;

    // Print in "down then across" order
    for (int r = 0; r < num_rows; r++) {
        for (int c = 0; c < num_cols; c++) {
            int index = c * num_rows + r;
            if (index < count)
                printf("%-*s", col_width, filenames[index]);
        }
        printf("\n");
    }

    // Free memory
    for (int i = 0; i < count; i++)
        free(filenames[i]);
    free(filenames);
}
