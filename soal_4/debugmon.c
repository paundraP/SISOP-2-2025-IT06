#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

void write_log(const char *process, const char *status) {
    FILE *fp = fopen("debugmon.log", "a");
    if (!fp) {
        printf("Gagal buka file log!\n");
        return;
    }
    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    fprintf(fp, "[%02d:%02d:%04d]-[%02d:%02d:%02d]_%s_%s\n",
            t->tm_mday, t->tm_mon + 1, t->tm_year + 1900,
            t->tm_hour, t->tm_min, t->tm_sec, process, status);
    fclose(fp);
}

void save_failed_user(const char *user) {
    FILE *fp = fopen("failed_users.log", "a");
    if (!fp) {
        printf("Gagal buka file failed_users.log!\n");
        return;
    }
    fprintf(fp, "%s\n", user);
    fclose(fp);
}

int main(int argc, char *argv[]) {
    if (argc < 3) {
        return 1; 
    }

    char *command = argv[1];
    char *user = argv[2];
    char *envp[] = {NULL};
    pid_t pid;

    if (strcmp(command, "list") == 0) {
        pid = fork();
        if (pid == 0) {
            char *args[] = {"ps", "-u", user, NULL};
            write_log("list", "RUNNING");
            execve("/bin/ps", args, envp);
            perror("Gagal menjalankan ps");
            exit(1);
        }
        wait(NULL);

    } else if (strcmp(command, "daemon") == 0) {
        pid = fork();
        if (pid == 0) {
            write_log("daemon", "RUNNING");
            while (1) {
                sleep(10);
                write_log("daemon_monitor", "RUNNING");
            }
            exit(0);
        }
        printf("Daemon dimulai untuk user %s (PID: %d)\n", user, pid);
        write_log("daemon_start", "RUNNING");

    } else if (strcmp(command, "stop") == 0) {
        pid = fork();
        if (pid == 0) {
            char *args[] = {"pkill", "-f", "./debugmon daemon", NULL};
            write_log("stop", "RUNNING");
            execve("/usr/bin/pkill", args, envp);
            perror("Gagal menjalankan pkill");
            exit(1);
        }
        wait(NULL);
        printf("Pengawasan untuk user %s dihentikan\n", user);

    } else if (strcmp(command, "fail") == 0) {
        pid = fork();
        if (pid == 0) {
            char *args[] = {"killall", "-u", user, NULL};
            write_log("fail", "FAILED");
            execve("/usr/bin/killall", args, envp);
            perror("Gagal menjalankan killall");
            exit(1);
        }
        wait(NULL);
        save_failed_user(user);
        printf("Semua proses untuk user %s digagalkan\n", user);

    } else if (strcmp(command, "revert") == 0) {
        if (getuid() != 0) {
            printf("Perintah revert hanya bisa dijalankan oleh root!\n");
            return 1;
        }

    
        pid = fork();
        if (pid == 0) {
            char *args[] = {"su", user, "-c", "./debugmon daemon", NULL};
            write_log("revert", "RUNNING");
            execve("/bin/su", args, envp);
            perror("Gagal menjalankan su untuk revert");
            exit(1);
        }
        wait(NULL);
        printf("Akses untuk user %s dikembalikan\n", user);

    } else {
        printf("Perintah tidak dikenal: %s\n", command);
        return 1;
    }

    return 0;
}
