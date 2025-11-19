#include <getopt.h>
#include <stdbool.h>
#include <stdio.h>
#include <unistd.h>

#include "common.h"
#include "file.h"
#include "parse.h"

void print_usage(char *argv[]) {
    printf("Usage: %s -n -f <database file>\n", argv[0]);
    printf("\t-n - create new database file\n");
    printf("\t-f - (required) path to database file\n");
    printf("\t-a - add a new employee via csv (name,address,hours)\n");
    printf("\t-l - list all employees\n");
}

int main(int argc, char *argv[]) {

    int c;
    bool newfile = false;
    bool list_all = false;
    char *filepath = NULL;
    char *addString = NULL;
    int dbfd = -1;

    struct dbheader_t *dbhd;
    struct employee_t *employees;

    while ((c = getopt(argc, argv, "nlf:a:")) != -1) {
        switch (c) {
        case 'n':
            newfile = true;
            break;
        case 'f':
            filepath = optarg;
            break;
        case 'a':
            addString = optarg;
            break;
        case 'l':
            list_all = true;
            break;
        case '?':
            printf("Unkown argument -%c\n", c);
        default:
            return -1;
        }
    }

    if (filepath == NULL) {
        printf("filepath is a required arguemnt\n");
        print_usage(argv);
        return 0;
    }

    if (newfile) {
        dbfd = create_db_file(filepath);
        if (dbfd == STATUS_ERROR) {
            printf("Error: Unable to create database file \n");
            return -1;
        }
        if (create_db_header(&dbhd) == STATUS_ERROR) {
            printf("Error: Could not create new database header\n");
            return -1;
        }
    } else {
        dbfd = open_db_file(filepath);
        if (dbfd == STATUS_ERROR) {
            printf("Error: Unable to open database file\n");
            return -1;
        }
        if (validate_db_header(dbfd, &dbhd)) {
            printf("Error: Could not read new database header\n");
            return -1;
        }

        if (read_employees(dbfd, dbhd, &employees) == STATUS_ERROR) {
            printf("Error: Failed to Read Employees\n");
            return -1;
        }
    }

    if (addString) {
        if (add_employee(dbhd, &employees, addString) == STATUS_ERROR) {
            printf("Error: Failed to Add Employees\n");
            return -1;
        }
    }

    if (output_file(dbfd, dbhd, employees) == STATUS_ERROR) {
        printf("Error: Failed to write to db\n");
        return -1;
    }

    if (list_all) {
        if (list_employees(dbhd, employees) == STATUS_ERROR) {
            printf("Error: Failed to list employees\n");
            return -1;
        }
    }

    close(dbfd);

    return 0;
}
