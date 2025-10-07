#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

// ==============================
// Helper: Compare two strings for qsort
// ==============================
int compare_names(const void *a, const void *b) {
    const char *name1 = *(const char **)a;
    const char *name2 = *(const char **)b;
    return strcmp(name1, name2);
}

// ==============================
// Display functions
// ==============================

// Horizontal display (-x)
void print_horizontal_listing(char **filenames, int count) {
    for (int i = 0; i < count; i++) {
        printf("%-20s", filenames[i]);
        if ((i + 1) % 5 == 0) printf("\n");
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
            printf("%-20s\t%lld bytes\n", filenames[i], (long long)fileStat.st_size);
        }
    }
}

// Default (vertical) display
void print_vertical_listing(char **filenames, int count) {
    for (int i = 0; i < count; i++) {
        printf("%s\n", filenames[i]);
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
        print_horizontal_listing(filenames, count);
    else
        print_vertical_listing(filenames, count);

    // Free memory
    for (int i = 0; i < count; i++)
        free(filenames[i]);
    free(filenames);

    return 0;
}
