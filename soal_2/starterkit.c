#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <memory.h>

char base46_map[] = {'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P',
    'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z', 'a', 'b', 'c', 'd', 'e', 'f',
    'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v',
    'w', 'x', 'y', 'z', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '+', '/'};

// https://drive.google.com/file/d/1_5GxIGfQr3mNKuavJbte_AoRkEQLXSKS/view

void download_file() {
    struct stat st = {0};
    if (stat("starter_kit", &st) == -1) {
        // Directory does not exist, create it
        printf("Directory 'starter_kit' does not exist. Creating now...\n");
        if (mkdir("starter_kit", 0700) == -1) {
            perror("Failed to create directory");
            return;
        }
    } else {
        printf("Directory 'starter_kit' already exists.\n");
        return;
    }

    char *file_id = "1_5GxIGfQr3mNKuavJbte_AoRkEQLXSKS";
    char *url = "https://drive.google.com/uc?export=download&id=";
    char *full_url = malloc(strlen(url) + strlen(file_id) + 1);
    strcpy(full_url, url);
    strcat(full_url, file_id);

    char *argv_curl[] = {
        "curl", 
        "-L",
        full_url, 
        "-o", 
        "file.zip",
        NULL
    };
    
    if (fork() == 0) {
        execve("/usr/bin/curl", argv_curl, NULL);
    } else {
        wait(NULL);
    }

    char *argv_unzip[] = {
        "unzip", 
        "file.zip", 
        "-d", 
        "starter_kit",
        NULL
    };

    if (fork() == 0) {
        execve("/usr/bin/unzip", argv_unzip, NULL);
    } else {
        wait(NULL);
    }
    remove("file.zip");
}

char* decode_base64(char *str) {
    char counts = 0;
    char buffer[4];
    char* plain = malloc(strlen(str) * 3 / 4);
    int i = 0, p = 0;

    for(i = 0; str[i] != '\0'; i++) {
        char k;
        for(k = 0 ; k < 64 && base46_map[k] != str[i]; k++);
        buffer[counts++] = k;
        if(counts == 4) {
            plain[p++] = (buffer[0] << 2) + (buffer[1] >> 4);
            if(buffer[2] != 64)
                plain[p++] = (buffer[1] << 4) + (buffer[2] >> 2);
            if(buffer[3] != 64)
                plain[p++] = (buffer[2] << 6) + buffer[3];
            counts = 0;
        }
    }

    plain[p] = '\0';    /* string padding character */
    return plain;
}

void quarantine() {
    
}


int main() {
    printf("decoded base64: %s\n", decode_base64("cGF1bmRyYXB1am8="));
    
    return 0;
}