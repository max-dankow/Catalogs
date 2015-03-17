#include <stdio.h>
#include <dirent.h>

int main(int argc, char **argv)
{
    if (argc == 1)
    {
        perror("Wrong parameters.");
        return 1;
    }

    DIR *current_dir = opendir(argv[1]);

    if (current_dir == NULL)
    {
        perror("Can't open catalog.");
        return 1;
    }

    struct dirent *entry;

    while (1)
    {
        entry = readdir(current_dir);

        if (entry == NULL)
            break;

        printf("%d - %s [%d] %d\n", entry->d_ino, entry->d_name, entry->d_type, entry->d_reclen);

        //int readdir_r(current_dir, sub, struct dirent **result);


    }



    closedir(current_dir);
    return 0;
}

