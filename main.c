#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <linux/limits.h>
#include "Hash-Table.h"

static const int LINK_NONE = 0;
static const int LINK_ALWAYS = 1;
static const int LINK_ONCE = 2;


int process_file(char *path)
{
    int count = 0, last = 0;
    char ch;
    FILE *current_file = fopen(path, "r");

    if (current_file == NULL)
    {
        fprintf(stderr, "Can't open file: %s\n", path);
        return -1;
    }

    while(1)
    {
        ch = fgetc(current_file);

        if (!((ch >= '0' && ch <='9') ||
              (ch >= 'a' && ch <='z') ||
              (ch >= 'A' && ch <='Z') ||
              (ch =='_')))
        {
            if (last != 0)
            {
                count++;
            }

            last = 0;
        }
        else
        {
            last++;
        }

        if (ch == EOF)
        {
            break;
        }
    }

    fclose(current_file);
    return count;
}

void print_tabulation(int number)
{
    for (int i = 0; i < number;  ++i)
    {
        printf("|    ");
    }
}

void analise_entry(char *path, char *entry_name, int level,
                   int max_deep, int link_flag, struct Table **visited)
{
    struct stat entry_info;
    char entry_path[PATH_MAX + 1];

    sprintf(entry_path, "%s/%s", path, entry_name);

    if (link_flag == LINK_ONCE)
    {
        char *real_path = malloc(PATH_MAX + 1);

        if (realpath(entry_path, real_path) == NULL)
        {
            fprintf(stderr, "Can't get real path: %s\n", entry_path);
            return;
        }

        void *ptr;

        if (link_flag == LINK_ONCE && find_element(*visited, real_path, ptr) == 0)
        {
            printf("\033[35mFile was already processed:\n%s\033[0m\n", real_path);
            return;
        }

        *visited = add_element(*visited, real_path, NULL);
    }

    if (lstat(entry_path, &entry_info) != 0)
    {
        fprintf(stderr, "Can't open file: %s\n", entry_path);
        return;
    }

    print_tabulation(level);

    if (S_ISDIR(entry_info.st_mode))
    {
        printf("\033[32m%s (DIR)\033[0m\n", entry_name);

        if (level + 1 < max_deep || max_deep == 0)
            process_catalog(entry_path, level + 1, max_deep, link_flag, visited);
    }

    if (S_ISREG(entry_info.st_mode))
    {
        printf("\033[34m%s (FILE)\033[0m", entry_name);
        printf("\033[33m %d words\n\033[0m", process_file(entry_path));
    }

    if (S_ISLNK(entry_info.st_mode))
    {
        printf("\033[31m%s (LINK)\033[0m", entry_name);

        if (link_flag != LINK_NONE)
        {
            char target_name[PATH_MAX + 1];

            if(readlink(entry_path, target_name, PATH_MAX ) != -1)
            {
                printf( "\033[31m -> %s\033[0m\n", target_name);
                analise_entry(path, target_name, level, max_deep, link_flag, visited);
            }
            else
            {
                fprintf(stderr, "Can't resolve link: %s\n", entry_path);
            }

        }
        else
        {
            printf("\n");
        }
    }
}

void process_catalog(char *path, int level, int max_deep,
                     int link_flag, struct Table **visited)
{
    DIR *current_dir = opendir(path);

    if (current_dir == NULL)
    {
        fprintf(stderr, "Can't open catalog.\n");
        return;
    }

    struct dirent *entry;

    while (1)
    {
        entry = readdir(current_dir);

        if (entry == NULL)
        {
            break;
        }

        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
        {
            continue;
        }

        analise_entry(path, entry->d_name, level, max_deep, link_flag, visited);
    }

    closedir(current_dir);
}

int main(int argc, char **argv)
{
    if (argc == 1)
    {
        fprintf(stderr, "Wrong parameters.\n");
        return 1;
    }

    int max_deep = -1, link_flag = LINK_ALWAYS;
    int current_arg_index = 2;

    while (current_arg_index < argc)
    {
        if (strcmp(argv[current_arg_index], "-s") == 0)
        {
            if (link_flag != LINK_ALWAYS)
            {
                fprintf(stderr, "Wrong parameters.\n");
                return 1;
            }

            link_flag = LINK_NONE;
            current_arg_index++;
            continue;
        }

        if (strcmp(argv[current_arg_index], "-c") == 0)
        {
            if (link_flag != LINK_ALWAYS)
            {
                fprintf(stderr, "Wrong parameters.\n");
                return 1;
            }

            link_flag = LINK_ONCE;
            current_arg_index++;
            continue;
        }

        if (strcmp(argv[current_arg_index], "-r") == 0)
        {
            if (current_arg_index + 1 >= argc)
            {
                fprintf(stderr, "Wrong input.\n");
                return 1;
            }

            char *err_ptr = argv[current_arg_index + 1];
            max_deep = strtod(argv[current_arg_index + 1], &err_ptr);

            if (strcmp(err_ptr, "") == 0)
            {
                current_arg_index += 2;
            }
            else
            {
                fprintf(stderr, "Wrong input.\n");
                return 1;
            }

            continue;
        }
        else
        {
            fprintf(stderr, "Wrong input.\n");
            return 1;
        }
    }

    struct Table *visited_entry = create_table(100, 73);
    process_catalog(argv[1], 0, max_deep, link_flag, &visited_entry);
    dispose_table(&visited_entry, 1);
    printf("All done. Success.\n");
    return 0;
}

