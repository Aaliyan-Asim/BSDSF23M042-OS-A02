#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

// ==============================
// ANSI Color Codes
// ==============================
#define COLOR_RESET     "\033[0m"
#define COLOR_BLUE      "\033[0;34m"  // Directory
#define COLOR_GREEN     "\033[0;32m"  // Executable
#define COLOR_RED       "\033[0;31m"  // Archive files (.tar, .gz, .zip)
#define COLOR_PINK      "\033[1;35m"  // Symbolic Link
#define COLOR_REVERSE   "\033[7m"     // Special file

// ==============================
// Helper: Compare two strings for qsort
// ==============================
int compare_names(const void *a, const void *b) {
    const char *name1 = *(const char **)a;
    const char *name2 = *(const char **)b;
    return strcmp(name1, name2);
}

// ==============================
// Helper: Determine and print colorized filename
// ==============================
void print_colorized_name(const char *dirpath, const char *filename) {
    char filepath[1024];
    struct stat fileStat;

    snprintf(filepath, sizeof(filepath), "%s/%s", dirpath, filename);

    // Use lstat to detect symbolic links correctly
    if (lstat(filepath, &fileStat) == -1) {
        perror("lstat");
        printf("%s\n", filename);
        return;
    }

    const char *color = COLOR_RESET;

    // Determine color based on file type
    if (S_ISDIR(fileStat.st_mode)) {
        color = COLOR_BLUE;
    } else if (S_ISLNK(fileStat.st_mode)) {
        color = COLOR_PINK;
    } else if (S_ISCHR(fileStat.st_mode) || S_ISBLK(fileStat.st_mode) ||
               S_ISSOCK(fileStat.st_mode) || S_ISFIFO(fileStat.st_mode)) {
        color = COLOR_REVERSE;
    } else if (fileStat.st_mode & S_IXUSR) {
        color = COLOR_GREEN;
    } else if (strstr(filename, ".tar") || strstr(filename, ".gz") || strstr(filename, ".zip")) {
        color = COLOR_RED;
    }

    printf("%s%s%s", color, filename, COLOR_RESET);
}

// ==============================
// Display functions
// ==============================

// Horizontal display (-x)
void print_horizontal_listing(const char *dirpath, char **filenames, int count) {
    for (int i = 0; i < count; i++) {
        print_colorized_name(dirpath, filenames[i]);
        printf("%-20s", "");
        if ((i + 1) % 5 == 0)
            printf("\n");
    }
    printf("\n");
}

// Long display (-l)
void print_long_listing(const char *dirpath, char **filenames, int count) {
    struct stat fileStat;
    char filepath[1024];

    for (int i = 0; i < count; i++) {
        snprintf(filepath, sizeof(filepath), "%s/%s", dirpath, filenames[i]);
        if (stat(filepath, &fileStat) == 0) {
            print_colorized_name(dirpath, filenames[i]);
            printf("  %lld bytes\n", (long long)fileStat.st_size);
        }
    }
}

// Default (vertical) display
void print_vertical_listing(const char *dirpath, char **filenames, int count) {
    for (int i = 0; i < count; i++) {
        print_colorized_name(dirpath, filenames[i]);
        printf("\n");
    }
}

// ==============================
// Main program
// ==============================
int main(int argc, char *argv[]) {
    const char *dirpath = ".";
    int long_flag = 0, horiz_flag = 0;

    // Parse flags
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-l") == 0) long_flag = 1;
        else if (strcmp(argv[i], "-x") == 0) horiz_flag = 1;
        else dirpath = argv[i];
    }

    DIR *dir = opendir(dirpath);
    if (!dir) {
        perror("opendir");
        return 1;
    }

    struct dirent *entry;
    char **filenames = NULL;
    int count = 0;

    // Read all entries
    while ((entry = readdir(dir)) != NULL) {
        if (entry->d_name[0] == '.') continue; // skip hidden files
        filenames = realloc(filenames, (count + 1) * sizeof(char *));
        filenames[count] = strdup(entry->d_name);
        count++;
    }
    closedir(dir);

    // Sort filenames alphabetically
    qsort(filenames, count, sizeof(char *), compare_names);

    // Choose display mode
    if (long_flag)
        print_long_listing(dirpath, filenames, count);
    else if (horiz_flag)
        print_horizontal_listing(dirpath, filenames, count);
    else
        print_vertical_listing(dirpath, filenames, count);

    // Free memory
    for (int i = 0; i < count; i++)
        free(filenames[i]);
    free(filenames);

    return 0;
}
