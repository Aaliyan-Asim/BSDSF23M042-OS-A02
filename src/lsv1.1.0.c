/*
* Programming Assignment 02: lsv1.1.0
* This is the source file of version 1.1.0
* Read the write-up of the assignment to add the features to this base version
* Usage:
*       $ lsv1.0.0 
*       % lsv1.0.0  /home
*       $ lsv1.0.0  /home/kali/   /etc/
*/
#/*
 * lsv1.1.0 - Long Listing Version
 * Feature 2: Implementing `-l` option similar to `ls -l`
 */
#define _GNU_SOURCE
#define _XOPEN_SOURCE 700

#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <pwd.h>
#include <grp.h>
#include <time.h>
#include <getopt.h>


extern int errno;

/* Function declarations */
void print_permissions(mode_t mode, char *perm);
void print_long_listing(const char *dirpath);
void print_simple_listing(const char *dir);

int main(int argc, char *argv[])
{
    int opt;
    int long_listing = 0;

    /* Parse -l option */
    while ((opt = getopt(argc, argv, "l")) != -1)
    {
        switch (opt)
        {
        case 'l':
            long_listing = 1;
            break;
        default:
            fprintf(stderr, "Usage: %s [-l] [directory]\n", argv[0]);
            exit(EXIT_FAILURE);
        }
    }

    /* Directory to list (default current dir) */
    char *dir = (optind < argc) ? argv[optind] : ".";

    if (long_listing)
        print_long_listing(dir);
    else
        print_simple_listing(dir);

    return 0;
}

/* ===========================================================
   Simple listing (like v1.0.0)
   =========================================================== */
void print_simple_listing(const char *dir)
{
    DIR *dp = opendir(dir);
    if (dp == NULL)
    {
        fprintf(stderr, "Cannot open directory: %s\n", dir);
        return;
    }

    struct dirent *entry;
    errno = 0;

    while ((entry = readdir(dp)) != NULL)
    {
        if (entry->d_name[0] == '.') // skip hidden files
            continue;
        printf("%s\n", entry->d_name);
    }

    if (errno != 0)
        perror("readdir failed");

    closedir(dp);
}

/* ===========================================================
   Permission formatting helper
   =========================================================== */
void print_permissions(mode_t mode, char *perm)
{
    strcpy(perm, "----------");

    if (S_ISDIR(mode))  perm[0] = 'd';
    if (S_ISLNK(mode))  perm[0] = 'l';
    if (S_ISCHR(mode))  perm[0] = 'c';
    if (S_ISBLK(mode))  perm[0] = 'b';
    if (S_ISFIFO(mode)) perm[0] = 'p';
    if (S_ISSOCK(mode)) perm[0] = 's';

    if (mode & S_IRUSR) perm[1] = 'r';
    if (mode & S_IWUSR) perm[2] = 'w';
    if (mode & S_IXUSR) perm[3] = 'x';
    if (mode & S_IRGRP) perm[4] = 'r';
    if (mode & S_IWGRP) perm[5] = 'w';
    if (mode & S_IXGRP) perm[6] = 'x';
    if (mode & S_IROTH) perm[7] = 'r';
    if (mode & S_IWOTH) perm[8] = 'w';
    if (mode & S_IXOTH) perm[9] = 'x';
}

/* ===========================================================
   Long listing (-l)
   =========================================================== */
void print_long_listing(const char *dirpath)
{
    DIR *dir = opendir(dirpath);
    if (!dir)
    {
        perror("opendir");
        return;
    }

    struct dirent *entry;
    struct stat file_stat;
    char filepath[1024];

    while ((entry = readdir(dir)) != NULL)
    {
        if (entry->d_name[0] == '.') // skip hidden files
            continue;

        snprintf(filepath, sizeof(filepath), "%s/%s", dirpath, entry->d_name);

        if (lstat(filepath, &file_stat) == -1)
        {
            perror("lstat");
            continue;
        }

        char perm[11];
        print_permissions(file_stat.st_mode, perm);

        struct passwd *pw = getpwuid(file_stat.st_uid);
        struct group *gr = getgrgid(file_stat.st_gid);

        char timebuf[64];
        struct tm *timeinfo = localtime(&file_stat.st_mtime);
        strftime(timebuf, sizeof(timebuf), "%b %d %H:%M", timeinfo);

        printf("%s %2ld %-8s %-8s %8ld %s %s\n",
               perm,
               (long)file_stat.st_nlink,
               pw ? pw->pw_name : "unknown",
               gr ? gr->gr_name : "unknown",
               (long)file_stat.st_size,
               timebuf,
               entry->d_name);
    }

    closedir(dir);
}
