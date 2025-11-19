#ifndef PARSE_H
#define PARSE_H

#define HEADER_MAGIC 0x4c4c4144
#define HEADER_MAGIC 0x4c4c4144

#define NAME_LEN 256

#define ADDRESS_LEN 256

struct dbheader_t {
    unsigned int magic;
    unsigned short version;
    unsigned short count;
    unsigned int filesize;
};

struct employee_t {
    char name[NAME_LEN];
    char address[ADDRESS_LEN];
    unsigned int hours;
};

// int create_db_header(int fd, struct db_header_t **header_out);
// int validate_db_header(int fd, struct db_header_t **header_out);
// int read_employees(int fd, struct db_header_t *,
//                    struct employee_t **employees_out);
// int output_file(int fd, struct db_header_t *);

int create_db_header(struct dbheader_t **);
int validate_db_header(int fd, struct dbheader_t **);
int read_employees(int fd, struct dbheader_t *, struct employee_t **);
int output_file(int fd, struct dbheader_t *, struct employee_t *);

#endif // !PARSE_H
