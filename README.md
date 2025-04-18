# SISOP IT-06

Repository ini berisi hasil pengerjaan Praktikum Sistem Operasi 2025 Modul 2

| Nama                     | Nrp        |
| ------------------------ | ---------- |
| Paundra Pujo Darmawan    | 5027241008 |
| Putri Joselina Silitonga | 5027241116 |

##Soal 1 (Putri Joselina Silitonga)

Pada soal 1 kita disuruh untuk melakukan filter, combine, dna decode untuk menghasilakn sebuah password. Di demo kemarin saya terdapat banyak kesalahan yaitu ketika saya mengetik ./action -m Filter makan semua proses akan dijalankan padahal itu salah karena seharusnya kita satu-satu dulu menjalakan filter-combine lalu struktur tree saya salah.

1. Download dan Ekstraksi
Kode Lama:

```c
char *wget_args[] = {"wget", "https://drive.google.com/uc?export=download&id=1xFn1OBJUuSdnApDseEczKhtNzyGekauK", "-O", "Clues.zip", NULL};
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
```

Kode ini mengekstrak Clues.zip langsung ke direktori saat ini tanpa menentukan folder tujuan (unzip Clues.zip) sehingga 
struktur direktori salah, menyebabkan langkah Filter gagal menemukan file di Clues/Clue*.

Kode Baru:
```c
if (argc == 1) {
    if (access("Clues", F_OK) == 0) {
        printf("Folder Clues sudah ada, skip download.\n");
        return 0;
    }
    printf("Download Clues.zip...\n");
    char *wget_args[] = {"wget", "https://drive.google.com/uc?export=download&id=1xFn1OBJUuSdnApDseEczKhtNzyGekauK", "-O", "Clues.zip", NULL};
    if (fork() == 0) {
        execve("/usr/bin/wget", wget_args, env);
        perror("Gagal download Clues.zip");
        exit(1);
    }
    wait(NULL);
    if (access("Clues.zip", F_OK) != 0) {
        fprintf(stderr, "Clues.zip tidak ada! Download gagal.\n");
        exit(1);
    }
    printf("Ekstrak Clues.zip...\n");
    char *unzip_args[] = {"unzip", "-o", "Clues.zip", "-d", "Clues", NULL};
    if (fork() == 0) {
        execve("/usr/bin/unzip", unzip_args, env);
        perror("Gagal ekstrak Clues.zip");
        exit(1);
    }
    wait(NULL);
    printf("Hapus Clues.zip...\n");
    char *rm_zip_args[] = {"rm", "-f", "Clues.zip", NULL};
    if (fork() == 0) {
        execve("/bin/rm", rm_zip_args, env);
        perror("Gagal hapus Clues.zip");
        exit(1);
    }
    wait(NULL);
    printf("Download dan ekstrak selesai!\n");
    return 0;
}
```
Perbaikannya adalah menambahkan mode default yang hanya berjalan jika tidak ada argumen. Mengekstrak Clues.zip ke folder Clues/ (unzip -o Clues.zip -d Clues), memastikan struktur jadi Clues/Clue*. Memeriksa keberadaan Clues/ untuk skip jika sudah ada, dan menghapus Clues.zip setelah ekstraksi untuk membersihkan direktori.

2. Mode Filter

Kode Lama

```c
char *mkdir_args[] = {"mkdir", "-p", "Clues/Filtered", NULL};
if (fork() == 0) {
    execve("/bin/mkdir", mkdir_args, env);
    perror("mkdir failed");
    exit(1);
}
wait(NULL);
char *filter_args[] = {"sh", "-c", "find Clues -type f -not -path 'Clues/Filtered/' | grep -E '/[0-9a-z]{1}\\.txt$|/dst\\.$' | xargs -I {} mv {} Clues/Filtered/", NULL};
if (fork() == 0) {
    execve("/bin/sh", filter_args, env);
    perror("filter failed");
    exit(1);
}
wait(NULL);
```
Masalahnya adalah menggunakan find dengan grep -E '/[0-9a-z]{1}\\.txt$|/dst\\.$' untuk memindahkan file ke Clues/Filtered. sehingga file tidak relevan masuk ke Clues/Filtered, dan struktur direktori tidak sesuai gambar soal.


Kode Baru

```
if (strcmp(argv[2], "Filter") == 0) {
    printf("Bikin folder Filtered...\n");
    char *mkdir_args[] = {"mkdir", "-p", "Clues/Filtered", NULL};
    if (fork() == 0) {
        execve("/bin/mkdir", mkdir_args, env);
        perror("Gagal bikin Clues/Filtered");
        exit(1);
    }
    wait(NULL);
    printf("Filter file...\n");
    char *filter_args[] = {"sh", "-c", 
        "find Clues/Clue* -type f \\( -name '[1-6].txt' -o -name '[a-f].txt' \\) -exec mv {} Clues/Filtered/ \\; && "
        "find Clues/Clue* -type f -name '*.txt' -not -path 'Clues/Filtered/*' -delete", NULL};
    if (fork() == 0) {
        execve("/bin/sh", filter_args, env);
        perror("Gagal filter file");
        exit(1);
    }
    wait(NULL);
    printf("Filter selesai! File ada di Clues/Filtered.\n");
}
```
Perbaikannya adalah menggunakan find Clues/Clue* -type f \( -name '[1-6].txt' -o -name '[a-f].txt' \) untuk memindahkan hanya file 1.txt sampai 6.txt dan a.txt sampai f.txt ke Clues/Filtered, dan  sesuai struktur tree di soal

3. Mode Combine
Kode Lama:
```c
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
```

Masalahnya adalah enggunakan urutan hard-coded {1,a,2,b,...,6,f} untuk menggabungkan file ke Clues/Combined.txt dan tidak menghapus file di Clues/Filtered setelah penggabungan, dan tidak menampilkan isi Clues/Combined.txt.


Kode Baru:
```c
if (strcmp(argv[2], "Combine") == 0) {
    printf("Cek file di Clues/Filtered...\n");
    char *check_args[] = {"sh", "-c", "ls Clues/Filtered/*.txt", NULL};
    int status;
    pid_t pid = fork();
    if (pid == 0) {
        execve("/bin/sh", check_args, env);
        perror("Gagal cek Clues/Filtered");
        exit(1);
    }
    wait(&status);
    if (WEXITSTATUS(status) != 0) {
        fprintf(stderr, "Tidak ada file .txt di Clues/Filtered. Jalankan Filter dulu!\n");
        exit(1);
    }
    DIR *dir = opendir("Clues/Filtered");
    if (!dir) {
        perror("Gagal buka Clues/Filtered");
        exit(1);
    }
    struct dirent *entry;
    char numbers[100][256], letters[100][256];
    int num_count = 0, letter_count = 0;
    while ((entry = readdir(dir))) {
        if (strstr(entry->d_name, ".txt")) {
            char *name = entry->d_name;
            if (strcmp(name, ".") == 0 || strcmp(name, "..") == 0) continue;
            int is_number = 1, is_letter = 1;
            for (int i = 0; name[i] != '.'; i++) {
                if (!isdigit(name[i])) is_number = 0;
                if (!isalpha(name[i])) is_letter = 0;
            }
            if (is_number) {
                snprintf(numbers[num_count], 256, "Clues/Filtered/%s", name);
                num_count++;
            } else if (is_letter) {
                snprintf(letters[letter_count], 256, "Clues/Filtered/%s", name);
                letter_count++;
            }
        }
    }
    closedir(dir);
    for (int i = 0; i < num_count - 1; i++) {
        for (int j = 0; j < num_count - i - 1; j++) {
            if (atoi(strrchr(numbers[j], '/') + 1) > atoi(strrchr(numbers[j + 1], '/') + 1)) {
                char temp[256];
                strcpy(temp, numbers[j]);
                strcpy(numbers[j], numbers[j + 1]);
                strcpy(numbers[j + 1], temp);
            }
        }
    }
    for (int i = 0; i < letter_count - 1; i++) {
        for (int j = 0; j < letter_count - i - 1; j++) {
            if (letters[j][strlen(letters[j]) - 5] > letters[j + 1][strlen(letters[j + 1]) - 5]) {
                char temp[256];
                strcpy(temp, letters[j]);
                strcpy(letters[j], letters[j + 1]);
                strcpy(letters[j + 1], temp);
            }
        }
    }
    printf("Gabungin file ke Combined.txt...\n");
    FILE *output = fopen("Clues/Combined.txt", "w");
    if (!output) {
        perror("Gagal bikin Combined.txt");
        exit(1);
    }
    int max_count = num_count > letter_count ? num_count : letter_count;
    for (int i = 0; i < max_count; i++) {
        if (i < num_count) {
            FILE *input = fopen(numbers[i], "r");
            if (input) {
                char buffer[1024];
                while (fgets(buffer, sizeof(buffer), input)) {
                    fputs(buffer, output);
                }
                fclose(input);
            }
        }
        if (i < letter_count) {
            FILE *input = fopen(letters[i], "r");
            if (input) {
                char buffer[1024];
                while (fgets(buffer, sizeof(buffer), input)) {
                    fputs(buffer, output);
                }
                fclose(input);
            }
        }
    }
    fclose(output);
    char *delete_args[] = {"rm", "-f", "Clues/Filtered/*.txt", NULL};
    if (fork() == 0) {
        execve("/bin/rm", delete_args, env);
        perror("Gagal hapus file .txt");
        exit(1);
    }
    wait(NULL);
    if (access("Clues/Combined.txt", F_OK) != 0) {
        fprintf(stderr, "Gagal bikin Combined.txt!\n");
        exit(1);
    }
    printf("Combine selesai! Combined.txt dibikin.\n");
    printf("Isi Combined.txt:\n");
    FILE *combined = fopen("Clues/Combined.txt", "r");
    if (combined) {
        char line[1024];
        while (fgets(line, sizeof(line), combined)) {
            printf("%s", line);
        }
        fclose(combined);
    } else {
        perror("Gagal buka Combined.txt");
    }
}
```
Perbaikannya adalah menggunakan dirent.h untuk membaca file di Clues/Filtered, memisahkan file angka (1.txt sampai 6.txt) dan huruf (a.txt sampai f.txt), lalu mengurutkannya secara dinamis. Menggabungkan file bergantian (angka lalu huruf) ke Clues/Combined.txt menggunakan fopen dan fgets, menghapus file di Clues/Filtered setelah penggabungan dengan rm, dan menampilkan isi Clues/Combined.txt.

4. Mode Decode
Kode Lama:

```c
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
```
Masalahnya adalah tidak menampilkan isi Clues/Decoded.txt setelah decode, sehingga sulit memeriksa  hasil. 
sehingga Tidak memenuhi kebutuhan soal untuk menampilkan hasil decode 

Kode Baru:

```c
if (strcmp(argv[2], "Decode") == 0) {
    printf("Decode Combined.txt...\n");
    FILE *input = fopen("Clues/Combined.txt", "r");
    if (!input) {
        perror("Gagal buka Combined.txt");
        exit(1);
    }
    FILE *output = fopen("Clues/Decoded.txt", "w");
    if (!output) {
        perror("Gagal bikin Decoded.txt");
        fclose(input);
        exit(1);
    }
    char buffer[1024];
    while (fgets(buffer, sizeof(buffer), input)) {
        rot13(buffer);
        fputs(buffer, output);
    }
    fclose(input);
    fclose(output);
    printf("Decode selesai! Decoded.txt dibikin.\n");
    printf("Password dari Decoded.txt:\n");
    FILE *decoded = fopen("Clues/Decoded.txt", "r");
    if (decoded) {
        char line[1024];
        while (fgets(line, sizeof(line), decoded)) {
            printf("%s", line);
        }
        fclose(decoded);
    } else {
        perror("Gagal buka Decoded.txt");
    }
}
```
Perbaikannya adalah menghasilkan Clues/Decoded.txt dengan isi yang terdekode dan menampilkan hasilnya, sesuai kebutuhan soal untuk mendapatkan "password" akhir.


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

# SOAL 4 (Putri Joselina Silitonga) 

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
