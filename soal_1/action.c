#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <string.h>

void rot13(char *str) {
    for (int i = 0; str[i] != '\0'; i++) {
        if ((str[i] >= 'A' && str[i] <= 'Z') || (str[i] >= 'a' && str[i] <= 'z')) {
            char base = (str[i] >= 'a') ? 'a' : 'A';
            str[i] = base + (str[i] - base + 13) % 26;
        }
    }
}

int main(int argc, char *argv[]) {
    char *wget_args[] = {"wget", "https://drive.google.com/uc?export=download&id=1xFn1OBJUuSdnApDseEczKhtNzyGekauK", "-O", "Clues.zip", NULL};
    char *env[] = {NULL};
    if (fork() == 0) {
        execve("/usr/bin/wget", wget_args, env);
        perror("wget failed");
        exit(1);
    }
    wait(NULL);

    char *unzip_args[] = {"unzip", "Clues.zip", NULL};
    if (fork() == 0) {
        execve("/usr/bin/unzip", unzip_args, env);
        perror("unzip failed");
        exit(1);
    }
    wait(NULL);

    char *mkdir_args[] = {"mkdir", "-p", "Clues/Filtered", NULL};
    if (fork() == 0) {
        execve("/bin/mkdir", mkdir_args, env);
        perror("mkdir failed");
        exit(1);
    }
    wait(NULL);

    char *filter_args[] = {"sh", "-c", "find Clues -type f -not -path 'Clues/Filtered/'*| grep -E '/[0-9a-z]{1}\\.txt$|/dst\\.$' | xargs -I {} mv {} Clues/Filtered/", NULL};
    if (fork() == 0) {
        execve("/bin/sh", filter_args, env);
        perror("filter failed");
        exit(1);
    }
    wait(NULL);


    char *files[] = {
        "Clues/Filtered/1.txt", "Clues/Filtered/a.txt",
        "Clues/Filtered/2.txt", "Clues/Filtered/b.txt",
        "Clues/Filtered/3.txt", "Clues/Filtered/c.txt",
        "Clues/Filtered/4.txt", "Clues/Filtered/d.txt",
        "Clues/Filtered/5.txt", "Clues/Filtered/e.txt",
        "Clues/Filtered/6.txt", "Clues/Filtered/f.txt",
        NULL
    };
    char *cat_args[] = {
        "cat",
        "Clues/Filtered/1.txt", "Clues/Filtered/a.txt",
        "Clues/Filtered/2.txt", "Clues/Filtered/b.txt",
        "Clues/Filtered/3.txt", "Clues/Filtered/c.txt",
        "Clues/Filtered/4.txt", "Clues/Filtered/d.txt",
        "Clues/Filtered/5.txt", "Clues/Filtered/e.txt",
        "Clues/Filtered/6.txt", "Clues/Filtered/f.txt",
        NULL
    };
    int i;

  
    for (i = 0; files[i] != NULL; i++) {
        if (access(files[i], F_OK) != 0) {
            fprintf(stderr, "File %s tidak ditemukan. Pastikan file ada di Clues/Filtered.\n", files[i]);
            exit(1);
        }
    }

   
    if (access("Clues/Filtered/dst.", F_OK) == 0) {
        cat_args[12] = "Clues/Filtered/dst.";
        cat_args[13] = NULL;
    }

 
    if (fork() == 0) {
        freopen("Clues/Combined.txt", "w", stdout);
        execve("/bin/cat", cat_args, env);
        perror("cat failed");
        exit(1);
    }
    wait(NULL);

    
    if (argc == 3 && strcmp(argv[1], "-m") == 0 && strcmp(argv[2], "decode") == 0) {
        FILE *input = fopen("Clues/Combined.txt", "r");
        if (input == NULL) {
            perror("Gagal membuka Combined.txt");
            exit(1);
        }

        FILE *output = fopen("Clues/Decoded.txt", "w");
        if (output == NULL) {
            perror("Gagal membuat Decoded.txt");
            fclose(input);
            exit(1);
        }

        char buffer[1024];
        while (fgets(buffer, sizeof(buffer), input) != NULL) {
            rot13(buffer); 
            fputs(buffer, output); 
        }

        fclose(input);
        fclose(output);
    }

    return 0;
}
