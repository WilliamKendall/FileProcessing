#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//file system stuff
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include <unistd.h>

typedef enum
{
    false = ( 1 == 0 ),
    true = ( ! false )
} bool;


bool isDir(char * path)
{
    struct stat st;
     if(stat(path, &st) == -1)
        return false;
    if(S_ISDIR(st.st_mode))
        return true;
    return false;
}

bool isFile(char * path)
{
    struct stat st;
     if(stat(path, &st) == -1)
        return false;
    if(S_ISREG(st.st_mode))
        return true;
    return false;
}

void listDirectory( char * path, bool recursive )
{
    printf("\nPath: %s\n\n",path);
    DIR * directory;
    struct dirent * ep;
    char * buffer =  malloc(sizeof(char) * 2048); //will error on REALLY long path names

    directory = opendir(path);
    if(directory != NULL)
    {
        printf("Directories:\n");
        while((ep = readdir(directory)))
        {
            if((ep->d_name[0] == '.' && ep->d_name[1] == 0x00) ||
            (ep->d_name[0] == '.' && ep->d_name[1] == '.' && ep->d_name[2] == 0x00))
                continue; //linux - this and previous paths
            strcpy(buffer, path);
            strcat(buffer, "/");
            strcat(buffer, ep->d_name);
            if(isDir(buffer))
                printf("   %s\n", ep->d_name);
        }

        rewinddir(directory);
        printf("\nFiles:\n");
        while((ep = readdir(directory)))
        {
            //printf("%s\n", ep->d_name);
            strcpy(buffer, path);
            strcat(buffer, "/");
            strcat(buffer, ep->d_name);
            if(isFile(buffer))
                printf("   %s\n", ep->d_name);
        }

        rewinddir(directory);
        if(recursive)
        while((ep = readdir(directory)))
        {
            if((ep->d_name[0] == '.' && ep->d_name[1] == 0x00) ||
            (ep->d_name[0] == '.' && ep->d_name[1] == '.' && ep->d_name[2] == 0x00))
                continue; //linux - this and previous paths
            strcpy(buffer, path);
            strcat(buffer, "/");
            strcat(buffer, ep->d_name);
            if(isDir(buffer))
                listDirectory(buffer, true);
        }

        closedir(directory);
    }

    free(buffer);
}



void printMenu(bool first)
{
    printf("0 - Exit\n");
    printf("1 - Select directory\n");
    if(first) return;
    printf("2 - List directory content (first level)\n");
    printf("3 - List directoy content (all levels)\n");
    printf("4 - Delete file\n");
    printf("5 - Display file (hexadecimal view)\n");
    printf("6 - Encrypt file (XOR with password)\n");
    printf("7 - Decryt file (XOR with pasword)\n");
}

int main()
{
    char input;
    bool first = true;
    char * path = malloc (sizeof(char) * 512 );
    path[0] = 0x00;

    FILE * filePtr;

    //buffers for string operations
    unsigned char * buffer;
    unsigned char * buffer2;

    int i, ii, iii; //any temp number
    while(1){
        if(path[0] != 0x00)
            printf("\nWorking directory : %s\n", path);
        printMenu(first);
        printf("\nPlease make a selection: ");
        scanf(" %c", &input); //space is to remove whitespace
        while ((getchar()) != '\n');
        switch (input - 48)
        {
            case 0:
                //exit
                free(path);
                printf("Thank you, please come again.\n");
                return 0;
            case 1:
                //select directory
                printf("Please enter a directory: ");
                scanf(" %s", path);
                if(!isDir(path))
                {
                    path[0] = 0x00;
                    first = true;
                    printf("Error: Could not open directory.\n\n");
                    break;
                }
                first = false;
                break;
        }//end switch

        if(first == true)
        {
            printf("Invalid selection. Please try again.\n");
            continue;
        }

        switch(input - 48)
        {
            case 0:
            case 1:
                break;
            case 2:
                //list directory
                listDirectory( path, false );
                break;
            case 3:
                //list directory recursive
                listDirectory( path, true );
                break;
            case 4:
                //delete file
                buffer = malloc(sizeof(char) * 512);
                buffer2 = malloc(sizeof(char) * 512);
                printf("File to delete: ");
                scanf(" %s", buffer);
                strcpy(buffer2, path);
                strcat(buffer2, "/");
                strcat(buffer2, buffer);
                if(isFile(buffer2))
                {
                    printf("Deleting file: %s\n", buffer2);
                    unlink(buffer2); //linux way of doing things
                }
                else
                    printf("%s is not a file.\n", buffer2);

                free(buffer);
                free(buffer2);
                break;
            case 5:
                //show hex
                buffer = malloc(sizeof(char) * 512);
                buffer2 = malloc(sizeof(char) * 512);
                printf("Enter filename: ");
                scanf(" %s", buffer);
                strcpy(buffer2, path);
                strcat(buffer2, "/");
                strcat(buffer2, buffer);
                if(isFile(buffer2))
                {
                    filePtr=fopen(buffer2,"rb");
                    if (!filePtr)
                    {
                        printf("Could not open file.\n");
                        goto case5_can_not_open; //I just really wanted to use a goto statment
                    }
                    //get file size
                    fseek (filePtr , 0 , SEEK_END);
                    i = ftell (filePtr);
                    rewind (filePtr);

                    free(buffer);
                    buffer = malloc(sizeof(char) * i);
                    //read file into memory
                    fread(buffer, i, 1, filePtr);
                    ii=0;
                    printf("\nHex data:\n");
                    while(ii < i)
                    {
                        if(ii%20 ==0)
                            printf("\n%.8i : ", ii);
                        printf("%02X ",buffer[ii]);
                        ii++;
                    }
                    printf("\n");
                    fclose(filePtr);
                }
                else
                    printf("%s is not a file.\n", buffer2);
case5_can_not_open:
                free(buffer);
                free(buffer2);
                break;
            case 6:
            case 7:
                //xor file <- THIS IS NOT ENCRYPTION (well, SUPER weak (unless its a one time pad))
                //xor is bidirectional, so encrypt is the same as decrypt
                buffer = malloc(sizeof(char) * 512);
                buffer2 = malloc(sizeof(char) * 512);
                printf("Enter filename: ");
                scanf(" %s", buffer);
                strcpy(buffer2, path);
                strcat(buffer2, "/");
                strcat(buffer2, buffer);
                if(isFile(buffer2))
                {
                    filePtr=fopen(buffer2,"rb");
                    if (!filePtr)
                    {
                        printf("Could not open file.\n");
                        goto case6_7_can_not_open; //I really miss it T_T
                    }
                    //get file size
                    fseek (filePtr , 0 , SEEK_END);
                    i = ftell (filePtr);
                    rewind (filePtr);

                    buffer = malloc(sizeof(char) * i);
                    //read file into memory
                    fread(buffer, i, 1, filePtr);
                    fclose(filePtr);
                    filePtr=fopen(buffer2,"wb"); //moved up because I wanted to use buffer2

                    //get password
                    printf("Please Enter Password: ");
                    scanf(" %s", buffer2);

                    //xor
                    ii = 0;
                    iii=0;
                    while(ii < i)
                    {
                        if(buffer2[iii] == 0x00)
                            iii = 0;
                        buffer[ii] ^= buffer2[iii];
                        ii++;
                        iii++;
                    }

                    //write

                    fwrite(buffer, i, 1, filePtr);
                    fclose(filePtr);
                }
                else
                    printf("%s is not a file.\n", buffer2);
case6_7_can_not_open:
                free(buffer);
                free(buffer2);
                break;
            default:
                printf("Invalid selection. Please try again.\n");
                break;

        }



    }
}
