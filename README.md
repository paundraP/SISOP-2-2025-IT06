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