#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <dirent.h>

#define MAXFILES 500
#define MAXLINE 1000
#define MAX_PATH 1024
#define MAX_OUTPUT 1024

char files_dirs[MAXFILES][MAXLINE] = {0};

int parse_command(char *filepath) {
    char command[2 * MAX_PATH + 10];
    char output[MAX_OUTPUT];
    FILE *fp;

    filepath[strcspn(filepath, "\n")] = '\0';

    if (strchr(filepath, '\'')) {
        fprintf(stderr, "Error: filename contains single quote (not supported in this example)\n");
        return 1;
    }

    snprintf(command, sizeof(command), "file '%s'", filepath);

    fp = popen(command, "r");
    if (fp == NULL) {
        perror("popen failed");
        return 1;
    }

    if (fgets(output, sizeof(output), fp) == NULL) {
        fprintf(stderr, "No output from 'file' command\n");
        pclose(fp);
        return 1;
    }

    pclose(fp);

    char *colon = strchr(output, ':');
    if (colon != NULL && *(colon + 1) == ' ') {
        char *filetype = colon + 2;
        filetype[strcspn(filetype, "\n")] = '\0';

        // printf("Filetype: %s\n", filetype);

        if (strstr(filetype, "text") || strstr(filetype, "ASCII")) {
            printf("%s → Text file\n", filepath);
        } else if (strstr(filetype, "executable")) {
            printf("%s → File executable\n", filepath);
        }
    } else {
        printf("Could not parse output.\n");
    }
    return 0;
}

int list_current_dir(char arr[MAXFILES][MAXLINE], const char *directory) {
    DIR *dir;
    struct dirent *entry;
    dir = opendir(directory);
    if (!dir) {
        printf("Error with opening\n");
        return -1;
    }
    int i = 0;
    while ((entry = readdir(dir)) != NULL) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue;
        }
        if (i >= MAXFILES) {
            printf("Too many files (limit: %d)\n", MAXFILES);
            break;
        }
        strncpy(arr[i], entry->d_name, MAXLINE - 1);
        arr[i][MAXLINE - 1] = '\0';
        parse_command(arr[i]);
        i++;
    }
    closedir(dir);
    return i;
}

int main() {
    char dir[MAXLINE];
    printf("Enter directory to parse\n");
    printf("> ");
    scanf("%999s", dir);
    int len = list_current_dir(files_dirs, dir);
    /*
    for (int i = 0; i < len; i++) {
        printf("- %s\n", files_dirs[i]);
    }
    */
    return 0;
}