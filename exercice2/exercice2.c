#include <stdio.h>
#include <stdlib.h>

#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <errno.h>
#include <fcntl.h>
#include <string.h>

#ifdef unix
#include <unistd.h>
#include <sys/stat.h>
#define O_BINARY 0
#define IO_LEN (1 << 30)
#endif

#if defined(THINK_C) || defined(__MWERKS__)
#include <unix.h>
#define IO_LEN (1 << 14)
#endif

#ifndef FALSE
#define FALSE (0)
#define TRUE (1)
#endif

#ifdef sun
extern char *sys_errlist[];
char *strerror(int err)
{
    return sys_errlist[err];
}
#endif

#include "../quantification/quantification.h"

#define MAGIC_PGM "P5\n"
#define MAGIC_PPM "P6\n"

static int match_key(int fd, char *key)
{
    char buf[80];

    read(fd, buf, strlen(key));
    if (strncmp(buf, key, strlen(key)) != 0)
        return FALSE;
    else
        return TRUE;
}

static void skip_comment(int fd, char code, char *c)
{
    while (*c == code)
    {
        while ((read(fd, c, 1) == 1) && (*c != '\n'))
            ;
        read(fd, c, 1);
    }
}

static void read_header_line(int fd, char *buf)
{
    int i;

    i = 1;
    while ((read(fd, &buf[i], 1) == 1) && (buf[i] != '\n') && (buf[i] != '\r') && (i < 79))
        i++;
    buf[i] = 0;
}

static int get_pgm_header(int fd, char *magic, int *width, int *height)
{
    char buf[80];

    if (!match_key(fd, magic))
        return FALSE;
    read(fd, buf, 1);
    skip_comment(fd, '#', buf);
    read_header_line(fd, buf);
    sscanf(buf, "%d %d", width, height);
    read(fd, buf, 1);
    skip_comment(fd, '#', buf);
    read_header_line(fd, buf);
    return TRUE;
}

static int open_read(char *filename)
{
    int fd;

    if ((fd = open(filename, O_BINARY | O_RDONLY)) < 0)
        fprintf(stderr, "can't reset file `%s': %s\n", filename, strerror(errno));
    return fd;
}

static int open_read_pixmap(char *filename, char *magic, int *width, int *height)
{
    int fd;

    if ((fd = open_read(filename)) < 0)
        return fd;
    if (!get_pgm_header(fd, magic, width, height))
    {
        fprintf(stderr, "can't read header of %s\n", filename);
        return -1;
    }
    return fd;
}

static unsigned char *alloc_pixmap(long size)
{
    unsigned char *data;

    if ((data = (unsigned char *)malloc(size)) == NULL)
    {
        fprintf(stderr, "malloc error\n");
        return NULL;
    }
    return data;
}

static void load_data(int fd, unsigned char *data, long size)
{
    char *buffer;
    int count;

    buffer = (char *)data;
    while (size > 0)
    {
        count = IO_LEN;
        if (count > size)
            count = size;
        read(fd, buffer, count);
        buffer += count;
        size -= count;
    }
}

static void load_chunky(int fd, unsigned char *R_data, unsigned char *G_data, unsigned char *B_data, int width, int height)
{
    unsigned char *buffer, *buf, *buf_R, *buf_G, *buf_B;
    int count;

    buffer = alloc_pixmap(3L * width);
    buf_R = R_data;
    buf_G = G_data;
    buf_B = B_data;
    for (; height > 0; height--)
    {
        load_data(fd, buffer, 3L * width);
        count = width;
        buf = buffer;
        while (count-- > 0)
        {
            *buf_R++ = *buf++;
            *buf_G++ = *buf++;
            *buf_B++ = *buf++;
        }
    }
    free(buffer);
}

unsigned char *load_pixmap(char *filename, int *width, int *height)
{
    int fd;
    long size;
    unsigned char *data;

    if ((fd = open_read_pixmap(filename, MAGIC_PGM, width, height)) < 0)
        return NULL;
    size = (long)*width * *height;
    data = alloc_pixmap(size);
    if (data != NULL)
        load_data(fd, data, size);
    close(fd);
    return data;
}

static void put_header_line(int fd, char *buf)
{
    write(fd, buf, strlen(buf));
}

static void put_header_info(int fd, char *mark, char *filename)
{
    char buf[80];
    time_t now;

    sprintf(buf, "%sTitle: %s\n", mark, filename);
    put_header_line(fd, buf);
    now = time(NULL);
    sprintf(buf, "%sCreationDate: %s", mark, ctime(&now));
    put_header_line(fd, buf);
    sprintf(buf, "%sCreator: pixmap_io, P. Chassignet\n", mark);
    put_header_line(fd, buf);
}

static void put_pgm_header(int fd, char *magic, int width, int height, char *filename)
{
    char buf[80];

    put_header_line(fd, magic);
    put_header_info(fd, "# ", filename);
    sprintf(buf, "%d %d\n255\n", width, height);
    put_header_line(fd, buf);
}

static int open_write(char *filename)
{
    int fd;

#if defined(THINK_C) || defined(__MWERKS__)
    if ((fd = open(filename, O_BINARY | O_CREAT | O_TRUNC | O_RDWR)) < 0)
#else
    if ((fd = open(filename, O_BINARY | O_CREAT | O_TRUNC | O_RDWR, S_IREAD | S_IWRITE)) < 0)
#endif
        fprintf(stderr, "can't rewrite file `%s': %s\n", filename, strerror(errno));
    return fd;
}

static void store_data(int fd, unsigned char *data, long size)
{
    char *buffer;
    int count;

    buffer = (char *)data;
    while (size > 0)
    {
        count = IO_LEN;
        if (count > size)
            count = size;
        write(fd, buffer, count);
        buffer += count;
        size -= count;
    }
}

void store_pixmap(char *filename, unsigned char *data, int width, int height)
{
    int fd;

    if ((fd = open_write(filename)) < 0)
        return;
    put_pgm_header(fd, MAGIC_PGM, width, height, filename);
    store_data(fd, data, (long)width * height);
    close(fd);
}

int abs(int a)
{
    if (a < 0)
        a = -a;
    return a;
}

int init_array(int array[], int size)
{
    for (int k = 0; k < size; k++)
    {
        array[k] = 0;
    }
}

int calcul_histogramme(int hist[], unsigned char *data, int width)
{
    //Calcul de l'histogramme
    for (int i = 0; i < 512; i++)
    {
        for (int j = 0; j < 512; j++)
        {
            for (int k = 0; k < 255; k++)
            {
                if (*(data + j * width + i) == k)
                {
                    hist[k] = hist[k] + 1;
                }
            }
        }
    }
}

void get_full_interval(int hist[], int m, int interval[])
{

    int pixels_per_inter = (int)((512 * 512) / m);
    int val = 0;
    int j = 0;

    //Découpe des intervalles
    for (int i = 0; i < 255; i++)
    {
        if (val > pixels_per_inter)
        {
            val = 0;
            interval[j] = i;
            j++;
        }
        val = hist[i] + val;
    }
}

int get_interval_value(int value, int hist[], int m, int interval[])
{
    int best = 0;
    int somme = 0;
    int ancienne_somme = 0;

    for (int i = 0; i <= m; i++)
    {
        ancienne_somme = abs(value - (interval[i]));
        if (ancienne_somme < somme)
        {
            best = interval[i];
        }
        somme = ancienne_somme;
    }
    return best;
}

void printHistogram(int *hist)
{
    int i, j;
    for (i = 0; i < 255; i += 5)
    {
        printf("[%d] ", i);
        int count = hist[i] % 500;
        for (j = 0; j < count; j++)
            printf("*");
        printf("\n");
    }
}

/* Quantification PGM */
int main(int argc, char **argv)
{

    unsigned char *data;
    int width, height;

    int hist[256];

    init_array(hist, 255);

    if ((argc > 1) && ((data = load_pixmap(argv[1], &width, &height)) != NULL))
    {

        int m = 100;
        int interval[m];

        init_array(interval, m);

        calcul_histogramme(hist, data, width);

        // Trouve les divisions d'intervalles pour m divisions en fonction de l'histogramme
        get_full_interval(hist, m, interval);

        for (int i = 0; i < 512; i++)
        {
            for (int j = 0; j < 512; j++)
            {
                for (int k = 0; k < 255; k++)
                {
                    if (*(data + j * width + i) == k)
                    {
                        // Changer chaque pixel dans le bon niveau de gris
                        *(data + j * width + i) = get_interval_value(*(data + j * width + i), hist, m, interval);
                    }
                }
            }
        }
        init_array(hist, 255);
        calcul_histogramme(hist, data, width);
        printHistogram(hist);
        store_pixmap("test.pgm", data, width, height);
    }
    return 0;
}