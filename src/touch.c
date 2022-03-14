/* Copyright (C) 2022 Sabadi László-Zsolt - All Rights Reserved
 * You may use, distribute and modify this code under the
 * terms of the GNU AGPL v3.0 license.
 *
 * You should have received a copy of the GNU AGPL v3.0 license with
 * this file. If not, please write to: pliexe, or visit : https://github.com/Pliexe/CmdUtils/blob/master/LICENSE
 */

#include <stdio.h>
#include <stdlib.h>
#include <Windows.h>
#include <io.h>
#include <errno.h>
#include <string.h>

#define MAX_INPUT 5000 + 1

void substr(char *dest, char *from, int start, int length)
{
    int i = 0;
    while (i < length)
    {
        dest[i] = from[start + i];
        i++;
    }
    dest[i] = '\0';
}

void appendToPath(char *dest, char *subpath, char *execPath)
{
    int i = 0, size, offset;
    while (execPath[i] != '\0')
    {
        dest[i] = execPath[i];
        i++;
    }

    dest[i] = '\\';
    size = i + 1;
    i = 0;
    offset = (subpath[0] == '.' ? ((subpath[1] == '/' || subpath[0] == '\\') ? 2 : 0) : ((subpath[1] == '/' || subpath[0] == '\\') ? 1 : 0));

    while (subpath[i + offset] != '\0')
    {
        if (subpath[i + offset] == '/')
            dest[i + size] = '\\';
        else
            dest[i + size] = subpath[i + offset];
        i++;
    }

    dest[i + size] = '\0';
}

void strcpyPath(char *dest, char *source)
{
    int i = 0;
    while (source[i] != '\0')
    {
        dest[i] = source[i] == '/' ? '\\' : source[i];
        i++;
    }
    dest[i] = '\0';
}

int main(int argc, char **argv)
{
    int i;
    char *path = calloc(MAX_PATH, sizeof(char));
    char workingDir[MAX_PATH];
    _getcwd(workingDir, MAX_PATH);

    if (argc < 2)
    {
        printf("No arguments provided! Usage: touch [filename]");
        exit(1);
    }

    char overwrite = 0;
    char *input = NULL;

    for (i = 1; i < argc; i++)
    {
        int length = strlen(argv[i]);

        if (length > 1)
        {
            if (argv[i][0] == '-')
            {
                if (length < 15)
                {
                    char *param = calloc(length, sizeof(char));
                    substr(param, argv[i], 1, strlen(argv[i]) - 1);

                    if (strcmp(param, "-overwrite") == 0 || strcmp(param, "o") == 0)
                        overwrite = 1;
                    else if (strcmp(param, "-input") == 0 || strcmp(param, "i") == 0)
                    {
                        if (i + 1 < argc)
                        {
                            input = calloc(MAX_INPUT, sizeof(char));
                            i++;
                            int offset = 0;
                            char checker = argv[i][0] == '"' ? '"' : '-';

                            while (i < argc && i < MAX_INPUT && argv[i][0] != checker)
                            {
                                int x = 0;
                                while (x < strlen(argv[i]))
                                {
                                    input[offset] = argv[i][x];
                                    x++;
                                    offset++;
                                };
                                input[offset] = ' ';
                                x++;
                                i++;
                                offset++;
                            }
                        }
                    }
                    free(param);
                }
            }
            else
            {
                if (argv[i][1] == ':')
                {
                    strcpyPath(path, argv[i]);
                }
                else
                    appendToPath(path, argv[i], workingDir);
            }
        }
    }

    if (_access(path, 6) == 0)
    {
        if (GetFileAttributes(path) == FILE_ATTRIBUTE_DIRECTORY)
        {
            printf("\nThe path provided is a directory!\n ");
            exit(5);
        }
        else
        {
            if (!overwrite)
            {
                printf("\nFile alreay exists! Use --overwrite to overwrite path.\n ");
                exit(2);
            }
        }
    }
    else
    {
        switch (errno)
        {
        case EACCES:
            printf("\nAccess denied!\n ");
            exit(3);
            break;
        case EINVAL:
            printf("\nInvalid argument.\n ");
            exit(4);
            break;
        }
    }

    FILE *fp = fopen(path, "w");
    if (fp == NULL)
    {
        printf("Failed to open file for writing!\n ");
        exit(10);
    }

    if (input != NULL)
        fprintf(fp, input);

    fclose(fp);

    int startIndex = 0;
    int pathLength = strlen(path);
    printf("\nCreated file named ");
    for (i = pathLength - 1; i >= 0; i--)
        if (path[i] == '\\')
        {
            startIndex = i;
            break;
        }

    char *filename = calloc(pathLength - startIndex + 1, sizeof(char));
    substr(filename, path, startIndex + 1, pathLength - startIndex);
    printf("%s!\n ", filename);

    free(path);
    free(filename);
    if (input != NULL)
        free(input);

    return 0;
}