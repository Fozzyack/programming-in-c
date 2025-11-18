#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>

#include "common.h"
#include "parse.h"

int create_db_header(int fd, struct db_header_t **header_out) {

    struct db_header_t *header = calloc(1, sizeof(struct db_header_t));
    if (header == NULL) {
        printf("Malloc Failed to create DB Header\n");
        return STATUS_ERROR;
    }
    header->version = 0x1;
    header->count = 0;
    header->magic = HEADER_MAGIC;
    header->filesize = sizeof(struct db_header_t);

    *header_out = header;
    return STATUS_SUCCESS;
}

int validate_db_header(int fd, struct db_header_t **header_out) {
    if (fd < 0) {
        printf("Recieved bad fd from user\n");
        return STATUS_ERROR;
    }

    struct db_header_t *header = calloc(1, sizeof(struct db_header_t));
    if (header == NULL) {
        printf("Malloc Failed to create DB Header\n");
        return STATUS_ERROR;
    }

    if (read(fd, header, sizeof(struct db_header_t)) !=
        sizeof(struct db_header_t)) {
        perror("read");
        free(header);
        return STATUS_ERROR;
    }

    header->version = ntohs(header->version);
    header->count = ntohs(header->count);
    header->magic = ntohl(header->magic);
    header->filesize = ntohl(header->filesize);

    if (header->version != 1) {
        printf("Improper Header Version\n");
        free(header);
        return STATUS_ERROR;
    }
    if (header->magic != HEADER_MAGIC) {
        printf("Improper Header Version\n");
        free(header);
        return STATUS_ERROR;
    }

    struct stat dbstat = {0};
    fstat(fd, &dbstat);
    if (header->filesize != dbstat.st_size) {
        printf("Incorrect file size given\n");
        return STATUS_ERROR;
    }

    *header_out = header;
    return STATUS_SUCCESS;
}
int output_file(int fd, struct db_header_t *dbhd) {

    if (fd < 0) {
        printf("Recieved bad fd from user\n");
        return STATUS_SUCCESS;
    }

    dbhd->magic = htonl(dbhd->magic);
    dbhd->filesize = htonl(dbhd->filesize);
    dbhd->count = htons(dbhd->filesize);
    dbhd->version = htons(dbhd->version);

    lseek(fd, 0, SEEK_SET);

    write(fd, dbhd, sizeof(struct db_header_t));

    return STATUS_SUCCESS;
}
