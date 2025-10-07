#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <unistd.h>
#include <pwd.h>
#include <grp.h>
#include <time.h>
#include <errno.h>

extern int errno;

void print_column_listing(const char *dirpath);
void print_horizontal_listing(const char *dirpath);
void print_long_listing(const char *dirpath);
void print_permissions(mode_t mode);

int main(int argc, char *argv[]) {
    const char *dir = ".";
    int long_flag = 0, horizontal_flag = 0;

    // Argument parsing
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-l") == 0)
            long_flag = 1;
        else if (strcmp(argv[i], "-x") == 0)
            horizontal_flag = 1;
        else
            dir = argv[i];
    }

    if (long_flag)
        print_long_listing(dir);
    else if (horizontal_flag)
        print_horizontal_listing(dir);
    else
        print_column_listing(dir);

    return 0;
}

/* ===========================================================
 * Column (down-then-across) display
 * =========================================================== */
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
        if (entry->d_name[0] == '.') continue;

        if (count >= capacity) {
            capacity *= 2;
            filenames = realloc(filenames, capacity * sizeof(char *));
        }

        filenames[count] = strdup(entry->d_name);
        int len = strlen(entry->d_name);
        if (len > maxlen) maxlen = len;
        count++;
    }
    closedir(dir);

    struct winsize w;
    int term_width = 80;
    if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &w) == 0)
        term_width = w.ws_col;

    int spacing = 2;
    int col_width = maxlen + spacing;
    int num_cols = term_width / col_width;
    if (num_cols < 1) num_cols = 1;

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

/* ===========================================================
 * Horizontal (across-then-down) display
 * =========================================================== */
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
        if (entry->d_name[0] == '.') continue;
        if (count >= capacity) {
            capacity *= 2;
            filenames = realloc(filenames, capacity * sizeof(char *));
        }
        filenames[count] = strdup(entry->d_name);
        int len = strlen(entry->d_name);
        if (len > maxlen) maxlen = len;
        count++;
    }
    closedir(dir);

    struct winsize w;
    int term_width = 80;
    if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &w) == 0)
        term_width = w.ws_col;

    int spacing = 2;
    int col_width = maxlen + spacing;
    int num_cols = term_width / col_width;
    if (num_cols < 1) num_cols = 1;

    for (int i = 0; i < count; i++) {
        printf("%-*s", col_width, filenames[i]);
        if ((i + 1) % num_cols == 0)
            printf("\n");
    }
    printf("\n");

    for (int i = 0; i < count; i++)
        free(filenames[i]);
    free(filenames);
}

/* ===========================================================
 * Long (-l) listing
 * =========================================================== */
void print_long_listing(const char *dirpath) {
    DIR *dir = opendir(dirpath);
    if (!dir) {
        perror("opendir");
        return;
    }

    struct dirent *entry;
    struct stat st;
    char path[1024];

    while ((entry = readdir(dir)) != NULL) {
        if (entry->d_name[0] == '.') continue;

        snprintf(path, sizeof(path), "%s/%s", dirpath, entry->d_name);
        if (stat(path, &st) == -1) {
            perror("stat");
            continue;
        }

        print_permissions(st.st_mode);
        printf(" %2ld", st.st_nlink);

        struct passwd *pw = getpwuid(st.st_uid);
        struct group *gr = getgrgid(st.st_gid);
        printf(" %-8s %-8s", 
               pw ? pw->pw_name : "?", 
               gr ? gr->gr_name : "?");

        printf(" %8ld", st.st_size);

        char timebuf[80];
        strftime(timebuf, sizeof(timebuf), "%b %d %H:%M", localtime(&st.st_mtime));
        printf(" %s %s\n", timebuf, entry->d_name);
    }
    closedir(dir);
}

/* ===========================================================
 * File permissions helper
 * =========================================================== */
void print_permissions(mode_t mode) {
    char perms[11];
    perms[0] = S_ISDIR(mode) ? 'd' : '-';
    perms[1] = (mode & S_IRUSR) ? 'r' : '-';
    perms[2] = (mode & S_IWUSR) ? 'w' : '-';
    perms[3] = (mode & S_IXUSR) ? 'x' : '-';
    perms[4] = (mode & S_IRGRP) ? 'r' : '-';
    perms[5] = (mode & S_IWGRP) ? 'w' : '-';
    perms[6] = (mode & S_IXGRP) ? 'x' : '-';
    perms[7] = (mode & S_IROTH) ? 'r' : '-';
    perms[8] = (mode & S_IWOTH) ? 'w' : '-';
    perms[9] = (mode & S_IXOTH) ? 'x' : '-';
    perms[10] = '\0';
    printf("%s", perms);
}
