#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <errno.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <linux/limits.h>

int process_file(char *path)
{
    int count = 0, last = 0;
    char ch;
    FILE *current_file = fopen(path, "r");

    if (current_file == NULL)
    {
        fprintf(stderr, "Can't open file: %s\n", path);
        return;
    }

    while(1)
    {
        ch=fgetc(current_file);

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

void tabulation(int number)
{
    for (int i = 0; i < number;  ++i)
    {
        printf("|    ");
    }
}

void analise_entry(char *path, char *entry_name, int level, int deep, int link_flag)
{
    struct stat entry_info;
    char entry_path[PATH_MAX + 1];
    sprintf(entry_path, "%s/%s", path, entry_name);

    if (lstat(entry_path, &entry_info) != 0)
    {
        fprintf(stderr, "Can't open file: %s\n", entry_path);
        return;
    }

    tabulation(level);

    if (S_ISDIR(entry_info.st_mode))
    {
        printf("\033[32m%s (DIR)\033[0m\n", entry_name);

        if (level + 1 < deep || deep == 0)
            process_catalog(entry_path, level + 1, deep, link_flag);
    }

    if (S_ISREG(entry_info.st_mode))
    {
        printf("\033[34m%s (FILE)\033[0m", entry_name);
        printf("\033[33m %d words\n\033[0m", process_file(entry_path));
    }

    if (S_ISLNK(entry_info.st_mode))
    {
        printf("\033[31m%s (LINK)\033[0m", entry_name);

        if (link_flag == 1)
        {
            char target_name[PATH_MAX + 1];

            if(readlink(entry_path, target_name, PATH_MAX ) != -1)
            {
                printf( "\033[31m -> %s\033[0m\n", target_name);
                analise_entry(path, target_name, level, deep, link_flag);
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

void process_catalog(char *path, int level, int deep, int link_flag)
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

        analise_entry(path, entry->d_name, level, deep, link_flag);
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

    int deep = -1, link_flag = 1;
    int current_arg_index = 2;

    while (current_arg_index < argc)
    {
        if (strcmp(argv[current_arg_index], "-s") == 0)
        {
            link_flag = 0;
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
            deep = strtod(argv[current_arg_index + 1], &err_ptr);

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

    process_catalog(argv[1], 0, deep, link_flag);
    return 0;
}

