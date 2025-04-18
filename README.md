# SISOP IT-06

Repository ini berisi hasil pengerjaan Praktikum Sistem Operasi 2025 Modul 2

| Nama                     | Nrp        |
| ------------------------ | ---------- |
| Paundra Pujo Darmawan    | 5027241008 |
| Putri Joselina Silitonga | 5027241116 |

## Soal 2 (Paundra Pujo Darmawan)

Pada soal ini, kita ditugaskan untuk melakukan operasi decrypt menggunakan base64. Terdapat beberapa fungsi yang digunakan untuk melakukan beberapa perintah. Dibawah ini adalah penjelasan untuk tiap tiap perintah yang ada.

Pada awal program dijalankan, program akan melakukan pengunduhan terhadap file zip yang telah disiapkan oleh praktikan dan melakukan unzip dimana kita dapat menggunakan `execve` untuk menjalankan command dari os kita. Dimana fungsi ini terdapat pada `download_file()`.

1. decrypt

Pada perintah ini, kita diminta untuk membuat sebuah direktori `quarantine` yang akan digunakan untuk mengarantina file file yang sebelumnya telah terencrypt. Dimana operasi ini berjalan pada background atau biasa disebut dengan daemon process. Dimana akan membuat sebuah direktori baru bernama `quarantine`. Pada kode yang saya tulis, saya melakukan fork dua kali untuk memastikan bahwa daemon process telah berjalan dengan baik dan menyimpah process id kedalam `/tmp/daemon.pid` dimana yang akan digunakan pada perintah `shutdown` yang akan saya jelaskan di bagian berikutnya.

2. quarantine 

Pada perintah ini, kita diharuskan untuk memindahkan file file yang sudah kita download, pada direktori starter_kit, dan pada direktori tersebut, masih ada beberapa file yang terencrypt. 
```
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
```
Dengan melakukan traversal kedalam direktori `starter_kit`, kita menggunakan beberapa fungsi untuk mengecek file yang memiliki indikasi adalah base64, pada fungsi `is_base64(const char *filename)`, dan melakukan re-check lagi di fungsi `decode_base64(char *str)` pada potongan kode dibawah ini.
```
if (plain[0] > 126 || plain[0] < 32) {
        free(plain);
        return NULL;
    }
```
Dimana pada potongan kode diatas, kita memvalidasi ulang apakah file tersebut benar merupakan base64 atau bukan. Dan dengan fungsi `quarantine(char *f1, char* f2)`, kita dapat memindahkan file pada direktori `starter_kit` ke direktori `quarantine`.

3. return

Pada perintah kali ini, melakukan pemindahan kembali dari file file yang di decrypt pada direktori `quarantine` kembali ke direktori `starter_kit`.

4. eradicate

Pada perintah kali ini, akan melakukan penghapusan seluruh file pada direktori `quarantine` jika ada.

5. shutdown

Pada perintah ini, akan melakukan kill pada daemon process yang telah kita mulai pada perintah `decrypt`.

# Soal 3 (Paundra Pujo Darmawan)

gcc malware.c -I/home/pujo/vcpkg/packages/miniz_x64-linux/include /home/pujo/vcpkg/packages/miniz_x64-linux/lib/libminiz.a -o malware

## SOAL 4 (Putri Joselina Silitonga) 

Pada soal nomor 4, kita diminta untuk membuat program yang dapat menjalankan lima perintah utama, yaitu: list, daemon, stop, fail, dan revert.
Program ini berfungsi untuk memantau dan mengontrol proses pengguna.

Namun, setelah dilakukan demo, ditemukan kesalahan pada bagian log, yaitu PID (Process ID) dari proses yang dimonitor tidak tercatat atau tidak muncul dalam log.
Masalah ini menunjukkan bahwa ada bagian kode yang perlu diperbaiki, khususnya pada proses pencatatan log agar PID tercatat dengan benar.

Maka dari itu, diperlukan perbaikan pada keseluruhan kode, terutama untuk memastikan setiap perintah (list, daemon, stop, fail, dan revert) bekerja dengan baik dan mencatat PID dengan benar di log file.

**1. Fungsi write_log**
```c
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
            t->tm_hour, t->tm_min, t->tm_sec,
            process, status);

    fclose(fp);
}
```
Ini merupakan kode lama yang tidak menampilkan PID di debugmon.lognya sehingga membutuhkan perbaikan menjadi kode dibawah ini:

```c
void write_log(const char *process, const char *status) {
    FILE *fp = fopen("debugmon.log", "a");
    if (!fp) {
        printf("Gagal buka file log!\n");
        return;
    }
    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    fprintf(fp, "[%02d:%02d:%04d]-[%02d:%02d:%02d]_PID:%d_%s_%s\n",
            t->tm_mday, t->tm_mon + 1, t->tm_year + 1900,
            t->tm_hour, t->tm_min, t->tm_sec, getpid(), process, status);
    fclose(fp);
}
```
Menambahkan getpid() untuk mencatat PID sesuai dengan format 

**[DD:MM:YYYY]-[HH:MM:SS]_PID:<pid>_<process>_<status>**

**2.Fungsi Fail**

```c
void save_failed_user(const char *user) {
    FILE *fp = fopen("failed_users.log", "a");
    if (!fp) {
        printf("Gagal buka file failed_users.log!\n");
        return;
    }
    fprintf(fp, "%s\n", user);
    fclose(fp);
}
```
Fungsi fail gunanya untuk menyimpan nama pengguna ke failed_users.log saat perintah fail dijalankan.

**3. Fungsi Main**
```c
int main(int argc, char *argv[]) {
    if (argc < 3) {
        return 1; 
    }
    char *command = argv[1];
    char *user = argv[2];
    char *envp[] = {NULL};
    pid_t pid;
    ...
}
```
Fungsi ini gunanya untuk memeriksa argumen baris perintah  lalu menjalankan perintah yang sesuai (list, daemon, stop, fail, revert).Tujuannya untuk mengatur alur program berdasarkan input pengguna, menggunakan fork() dan execve() untuk menjalankan perintah sistem.

**4. Fungsi List**
```c
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
}
```
Fungsi List untuk membuat proses anak dengan fork(), menjalankan ps -u <user> untuk menampilkan proses pengguna, mencatat status, lalu menunggu proses selesai.
Tujuan: Memantau proses aktif pengguna, dengan log mencakup PID (mis., PID:1234_list_RUNNING).

**5. Fungsi Daemon**
```c
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
}
```

Fungsi Daemon adalah untuk membuat proses daemon yang berjalan terus-menerus, mencatat status setiap 10 detik, dan menampilkan PID daemon.

**6. Fungsi Stop**
```c
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
}
```
Fungsi stop untuk menjalankan pkill guna menghentikan daemon, mencatat status, dan mengonfirmasi penghentian.

**7. Fungsi Fail**
```c
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
}
```

Fungsi fail digunakan untuk mematikan semua proses pengguna dengan killall, mencatat pengguna ke failed_users.log, dan mengonfirmasi dan dengan log mencakup PID dan status FAILED.


**8. Fungsi Revert**
```c
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
}
} else {
    printf("Perintah tidak dikenal: %s\n", command);
    return 1;
}
```
Fungsi revert untuk memeriksa akses root, menjalankan daemon sebagai pengguna dengan su, dan mengonfirmasi pemulihan.
