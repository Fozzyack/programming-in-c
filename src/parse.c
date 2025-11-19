#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

#include "common.h"
#include "parse.h"

int create_db_header(struct dbheader_t **headerOut) {

    struct dbheader_t *header = calloc(1, sizeof(struct dbheader_t));
    if (header == NULL) {
        printf("Malloc Failed to create DB Header\n");
        return STATUS_ERROR;
    }
    header->version = 0x1;
    header->count = 0;
    header->magic = HEADER_MAGIC;
    header->filesize = sizeof(struct dbheader_t);

    *headerOut = header;
    return STATUS_SUCCESS;
}

int validate_db_header(int fd, struct dbheader_t **header_out) {
    if (fd < 0) {
        printf("Recieved bad fd from user\n");
        return STATUS_ERROR;
    }

    struct dbheader_t *header = calloc(1, sizeof(struct dbheader_t));
    if (header == NULL) {
        printf("Malloc Failed to create DB Header\n");
        return STATUS_ERROR;
    }

    if (read(fd, header, sizeof(struct dbheader_t)) !=
        sizeof(struct dbheader_t)) {
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

int output_file(int fd, struct dbheader_t *dbhd, struct employee_t *employees) {

    if (fd < 0) {
        printf("Received bad fd from user\n");
        return STATUS_SUCCESS;
    }

    // Save count in host byte order for later use
    int count = dbhd->count;

    // Convert employee hours to network byte order first, using host byte order
    // count
    for (int i = 0; i < count; i++) {
        employees[i].hours = htonl(employees[i].hours);
    }

    // Now convert header fields to network byte order
    dbhd->magic = htonl(dbhd->magic);
    dbhd->filesize = htonl(dbhd->filesize);
    dbhd->count = htons(dbhd->count);
    dbhd->version = htons(dbhd->version);

    lseek(fd, 0, SEEK_SET);

    write(fd, dbhd, sizeof(struct dbheader_t));
    write(fd, employees, sizeof(struct employee_t) * count);

    return STATUS_SUCCESS;
}

int read_employees(int fd, struct dbheader_t *dbhd,
                   struct employee_t **employeesOut) {

    if (fd < 0) {
        printf("Receieved bad fd from user\n");
        return STATUS_ERROR;
    }

    struct employee_t *employees =
        calloc(dbhd->count, sizeof(struct employee_t));
    if (employees == NULL) {
        printf("Malloc failed to create employees list\n");
        return STATUS_ERROR;
    }

    lseek(fd, sizeof(struct dbheader_t), SEEK_SET);

    read(fd, employees, sizeof(struct employee_t) * dbhd->count);
    for (int i = 0; i < dbhd->count; i++) {
        employees[i].hours = ntohl(employees[i].hours);
    }

    *employeesOut = employees;

    return STATUS_SUCCESS;
}

int list_employees(struct dbheader_t *dbhd, struct employee_t *employees) {

    for (int i = 0; i < ntohs(dbhd->count); i++) {
        struct employee_t emp = employees[i];
        printf("%s %s %d\n", emp.name, emp.address, ntohl(emp.hours));
    }

    return STATUS_SUCCESS;
}

int add_employee(struct dbheader_t *dbhd, struct employee_t **employees, char *add_string) {

    if (dbhd == NULL) return STATUS_ERROR;
    if (employees == NULL) return STATUS_ERROR;
    if (*employees == NULL) return STATUS_ERROR;
    if (add_string == NULL) return STATUS_ERROR;

    struct employee_t *e = realloc(*employees, sizeof(struct employee_t) * dbhd->count + 1);
    if (e == NULL) {
        printf("Error: Couldn't resize employees array\n");
        return STATUS_ERROR;
    }
    *employees = e;

    dbhd->count++;

    printf("Adding new employee\n");

    char *name = strtok(add_string, ",");
    if (name == NULL) return STATUS_ERROR;
    char *addr = strtok(NULL, ",");
    if (addr == NULL) return STATUS_ERROR;
    char *hours = strtok(NULL, ",");
    if (hours == NULL) return STATUS_ERROR;

    strncpy((*employees)[dbhd->count - 1].name, name, sizeof((*employees)[dbhd->count - 1].name) - 1);
    strncpy((*employees)[dbhd->count - 1].address, addr, sizeof((*employees)[dbhd->count - 1].address) - 1);
    (*employees)[dbhd->count - 1].hours = atoi(hours);

    dbhd->filesize += sizeof(struct employee_t);

    return STATUS_SUCCESS;
}
