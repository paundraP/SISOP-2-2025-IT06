#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <memory.h>
#include <fcntl.h>
#include <errno.h>
#include <signal.h>
#include <time.h>
#include <dirent.h> 
#include <stdbool.h>
#include <ctype.h>


char base64_map[] = {'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P',
    'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z', 'a', 'b', 'c', 'd', 'e', 'f',
    'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v',
    'w', 'x', 'y', 'z', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '+', '/'};

// https://drive.google.com/file/d/1_5GxIGfQr3mNKuavJbte_AoRkEQLXSKS/view

void write_log(const char *message) {
    FILE *log_file = fopen("activity.log", "a");
    if (!log_file) return;

    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    char timestamp[64];
    strftime(timestamp, sizeof(timestamp), "[%d-%m-%Y][%H:%M:%S]", t);

    fprintf(log_file, "%s - %s\n", timestamp, message);
    fclose(log_file);
}

bool is_base64(const char *filename) {
    size_t len = strlen(filename);

    // Must be a multiple of 4 and not empty
    if (len == 0 || len % 4 != 0) return false;

    int padding = 0;
    for (size_t i = 0; i < len; ++i) {
        char c = filename[i];

        if (isalnum(c) || c == '+' || c == '/') {
            continue;
        } else if (c == '=') {
            padding++;

            if (i < len - 2) return false;
        } else {
            return false;
        }
    }

    return padding <= 2;
}

void get_filename_only(const char *filename, char *output) {
    char *dot = strrchr(filename, '.');
    if (dot) {
        size_t len = dot - filename;
        strncpy(output, filename, len);
        output[len] = '\0';
    } else {
        strcpy(output, filename);
    }
}

void handle_signal(int sig) {
    if (sig == SIGTERM) {
        printf("Daemon process terminated.\n");
        exit(0);
    }
}

void download_file() {
    struct stat st = {0};
    if (stat("starter_kit", &st) == -1) {
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

// https://github.com/elzoughby/Base64
// Saya mendapatkan source yang bagus untuk belajar dan menerapkan bagaimana konsep encoding dan decoding base64.
// Dengan sedikit adjustment pada pengecekan apakah sebuah file merupakan base64 dengan mengecek apakah decode nya printable character
char* decode_base64(char *str) {
    char counts = 0;
    char buffer[4];
    char* plain = malloc(strlen(str) * 3 / 4 + 1);
    int i = 0, p = 0;

    for(i = 0; str[i] != '\0'; i++) {
        char k;
        for(k = 0 ; k < 64 && base64_map[k] != str[i]; k++);
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

    plain[p] = '\0';

    // check kalau hasil decode nya printable atau ngga, kalau ngga berarti bukan base64
    if (plain[0] > 126 || plain[0] < 32) {
        // free(plain);
        return NULL;
    }

    return plain;
}

void start_daemon() {
    pid_t pid;
    char msg[256];

    pid = fork();
    if (pid < 0) {
        perror("fork failed");
        exit(EXIT_FAILURE);
    }

    if (pid > 0) {
        printf("Daemon started in background with PID %d\n", pid);
        exit(EXIT_SUCCESS);
    }
    
    umask(0);

    if (setsid() < 0) {
        perror("setsid failed");
        exit(EXIT_FAILURE);
    }

    pid = fork();
    if (pid < 0) {
        perror("fork failed");
        exit(EXIT_FAILURE);
    }

    if (pid > 0) {
        exit(EXIT_SUCCESS);
    }
    
    char cwd[1024];
    if (getcwd(cwd, sizeof(cwd)) != NULL) {
        printf("Current working directory: %s\n", cwd);
    } else {
        perror("getcwd failed");
    }

    close(STDIN_FILENO);
    close(STDOUT_FILENO);
    close(STDERR_FILENO);
    
    FILE *pid_file = fopen("/tmp/daemon.pid", "w");
    if (pid_file) {
        fprintf(pid_file, "%d\n", getpid());
        fclose(pid_file);
    } else {
        perror("failed to store the pid");
        exit(EXIT_FAILURE);
    }

    if (mkdir("quarantine", 0755) == -1) {
        int fd = open("/tmp/daemon_error.log", O_WRONLY | O_CREAT | O_APPEND, 0644);
        if (fd != -1) {
            dprintf(fd, "Failed to create 'quarantine' directory in current working directory: %s\n", strerror(errno));
            close(fd);
        }
        return;
    }

    sprintf(msg, "Successfully started decryption process with PID %d", getpid());
    write_log(msg);

    signal(SIGTERM, handle_signal);

    while (1) {
        sleep(60);
    }
}

void shutdown_daemon() {
    char msg[256];
    FILE *fp = fopen("/tmp/daemon.pid", "r");
    if (fp == NULL) {
        perror("Could not open PID file");
        return;
    }

    pid_t pid;
    fscanf(fp, "%d", &pid);
    fclose(fp);

    sprintf(msg, "%s - %d", "Successfully shut off decryption process with", pid);
    if (kill(pid, SIGTERM) == 0) {
        printf("Daemon process with PID %d has been terminated.\n", getpid());
        write_log(msg);
    } else {
        perror("Failed to terminate daemon process");
    }

    remove("/tmp/daemon.pid");
}


void quarantine(char *f1, char* f2) {
    char src_path[256], dest_path[256], msg[256];

    snprintf(src_path, sizeof(src_path), "starter_kit/%s", f1);
    snprintf(dest_path, sizeof(dest_path), "quarantine/%s", f2);

    if (rename(src_path, dest_path) == 0) {
        printf("File moved and renamed to: %s\n", dest_path);
    } else {
        perror("Failed to quarantine file");
    }

    sprintf(msg, "%s - %s", f2, "Successfully moved to quarantine directory.");
    write_log(msg);
}


void return_file(char *filename) {
    char src_path[256], dest_path[256], msg[256];

    snprintf(src_path, sizeof(src_path), "quarantine/%s", filename);
    snprintf(dest_path, sizeof(dest_path), "starter_kit/%s", filename);

    if (rename(src_path, dest_path) == 0) {
        printf("File returned to starter_kit: %s\n", dest_path);
    } else {
        perror("Failed to return file");
    }

    sprintf(msg, "%s - %s", filename, "Successfully returned to starter kit directory.");
    write_log(msg);
}

void eradicate(const char *dir_path) {
    char msg[1024];
    DIR *dir = opendir(dir_path);
    if (dir == NULL) {
        perror("opendir");
        return;
    }

    struct dirent *entry;
    char file_path[1024];

    while ((entry = readdir(dir)) != NULL) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue;
        }

        snprintf(file_path, sizeof(file_path), "%s/%s", dir_path, entry->d_name);
        sprintf(msg, "%s - %s", file_path, "Successfully deleted.");
        // Use stat() to determine if it's a regular file
        struct stat st;
        if (stat(file_path, &st) == 0) {
            if (S_ISREG(st.st_mode)) {  // Check if it's a regular file
                if (unlink(file_path) == -1) {
                    perror("unlink");
                } else {
                    printf("Deleted: %s\n", file_path);
                    write_log(msg);
                }
            }
        } else {
            perror("stat");
        }
    }

    closedir(dir);
}


void command() {
    printf("Usage: \n");
    printf("./starterkit --decrypt: Untuk membuat directory quarantine\n");
    printf("./starterkit --quarantine: Untuk memindahkan file dari directory starter_kit ke quarantine dan melakukan decoding pada filename yang terindikasi mengandung base64\n");
    printf("./starterkit --return: Untuk mengembalikan file yang ada di directory quarantine kembali ke starter_kit\n");
    printf("./starterkit --eradicate: Untuk menghapus seluruh file yang ada di directory quarantine\n");
    printf("./starterkit --shutdown: Untuk mematikan proses decrypt yang sudah dijalankan\n"); 
}


int main(int argc, char *argv[]) {
    DIR *d;
    struct dirent *dir;
    char *tmp;
    if (argc < 2) {
        command();
        return 1;
    }
    
    download_file();
    if (strcmp(argv[1], "--decrypt") == 0) {
        start_daemon();

        while(1) {
            sleep(10);
        }
    }else if (strcmp(argv[1], "--quarantine") == 0) {
        d = opendir("starter_kit");
        if (d) {
            while ((dir = readdir(d)) != NULL) {
                if (strcmp(dir->d_name, ".") == 0 || strcmp(dir->d_name, "..") == 0)
                    continue;
    
                char filename_only[256];
                get_filename_only(dir->d_name, filename_only);
                if (is_base64(filename_only) == true) {
                    tmp = decode_base64(filename_only);
                    if (tmp != NULL) {
                        quarantine(filename_only, tmp);
                    }else {
                        quarantine(dir->d_name, dir->d_name);
                    }
                }else {
                    quarantine(dir->d_name, dir->d_name);
                }
            }
            closedir(d);
        }
    } else if (strcmp(argv[1], "--return") == 0) {
        d = opendir("quarantine");
        if (d) {
            while ((dir = readdir(d)) != NULL) {
                return_file(dir->d_name);
            }
            closedir(d);
        }
    } else if (strcmp(argv[1], "--eradicate") == 0) {
        eradicate("quarantine");
    } else if (strcmp(argv[1], "--shutdown") == 0) {
        shutdown_daemon();
    }
    return 0;
}