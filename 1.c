#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <libgen.h> 
#include <dirent.h>
#include <errno.h>
#include <sys/stat.h>

char* scriptName;

typedef struct dirInfo{
    size_t max_size;
    char* dir_path;
    struct dirInfo *next;
}dirInfo_t;

dirInfo_t *head = NULL;


int counter = -1;
void PrintError(const char *scr_name, const char *msg_err, const char *f_name)
{
    fprintf(stderr, "%s: %s %s\n", scr_name, msg_err, (f_name) ? f_name : "");
} 


void push(dirInfo_t **head, size_t max_size, char* path, int counter) {
	dirInfo_t *tmp = (dirInfo_t*) malloc((counter +1 )*sizeof(dirInfo_t));
    tmp->max_size = max_size;
    tmp->dir_path =  (char*)malloc(sizeof(char)*(strlen(path) + 1));
    strcpy(tmp->dir_path, path);
    tmp->dir_path = path;
	tmp->next = (*head);
	(*head) = tmp;
}


void SortDir()
{
    dirInfo_t *new_root = NULL;
    while ( head != NULL )
    {
        dirInfo_t *node = head;
        head = head->next;

        if ( new_root == NULL || node->max_size > new_root->max_size )
        {
            node->next = new_root;
            new_root = node;
        }
        else
        {
            dirInfo_t *current = new_root;
            while ( current->next != NULL && !( node->max_size > current->next->max_size ) )
            {                   
                current = current->next;
            }                

            node->next = current->next;
            current->next = node;
        }
    }
    while (new_root) {
        printf("%s %ld\n", new_root->dir_path, new_root->max_size);
        new_root = new_root->next;
    }
    free(new_root);
}



void ShowDir(char *dir_name)
{
    struct stat st_stat;
    struct dirent *dir_entry, *rderr;
    char *fullpath;

    size_t max_size = 0;
    char* dir_path = dir_name;

    DIR *dir = opendir(dir_name);
    if (!dir) 
    {
        PrintError(scriptName, strerror(errno), dir_name);      
        return;
    }
    
    while(dir_entry = readdir(dir))
    {
        fullpath = (char*)malloc(sizeof(char)*(strlen(dir_name)+strlen(dir_entry->d_name)+2));
        strcpy(fullpath, dir_name);
        strcat(fullpath, "/");
        strcat(fullpath, dir_entry->d_name);
        
        if(dir_entry->d_type == DT_REG)
        {
            if(stat(fullpath, &st_stat) == 0)
            {
                max_size += st_stat.st_size;
            }
            else
            {
                fprintf(stderr, "%s : %s : %s\n", scriptName, strerror(errno), fullpath);
                continue;
            } 
        }
        else
        if(dir_entry->d_type == DT_DIR && strcmp(".", dir_entry->d_name) && strcmp("..", dir_entry->d_name))
        {
            ShowDir(fullpath);
        }
        errno = 0;    
    }
    if (errno != 0) {
        PrintError(scriptName, strerror(errno), fullpath);
    }
    if(closedir(dir) == -1){
        PrintError(scriptName, strerror(errno), fullpath); 
        free(fullpath); 
    } else
    {
        counter++;
        push(&head,max_size, dir_path,counter);
    }
}

int main(int argc, char* argv[])
{
    scriptName = basename(argv[0]);
    if(argc != 2)
    {
        fprintf(stderr,"%s: Not enough arguments\n",scriptName);
        return 0;
    }

    char *dir_name = realpath(argv[1], NULL);

    if(dir_name == NULL)
    {
        fprintf(stderr,"%s: There no such directory\n",scriptName);
        return 0;
    }

    // scriptName = basename("./1.exe");
    // char *dir_name = realpath("/etc", NULL);


    ShowDir(dir_name);
    SortDir();
    return 0;
}