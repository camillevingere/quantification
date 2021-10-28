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

/* Quantification de Floyd-Steinberg */
int main(int argc, char **argv)
{

    unsigned char *data;
    int width, height;
    int ancien_pixel, nouveau_pixel, erreur_quantification = 0;

    int n = 50;

    if ((argc > 1) && ((data = load_pixmap(argv[1], &width, &height)) != NULL))
    {
        for (int i = 1; i < 512; i++)
        {
            for (int j = 1; j < 512; j++)
            {
                ancien_pixel = *(data + j * width + i);
                nouveau_pixel = (*(data + j * width + i) / n) * n;
                *(data + j * width + i) = (*(data + j * width + i) / n) * n;
                erreur_quantification = ancien_pixel - nouveau_pixel;
                *(data + (j + 1) * width + i) = *(data + (j + 1) * width + i) + 7 / 16 * erreur_quantification;
                *(data + (j - 1) * width + (i + 1)) = *(data + (j - 1) * width + (i + 1)) + 3 / 16 * erreur_quantification;
                *(data + j * width + (i + 1)) = *(data + j * width + (i + 1)) + 5 / 16 * erreur_quantification;
                *(data + (j + 1) * width + (i + 1)) = *(data + (j + 1) * width + (i + 1)) + 1 / 16 * erreur_quantification;
            }
        }
        store_pixmap("floyd.pgm", data, width, height);
    }
    return 0;
}