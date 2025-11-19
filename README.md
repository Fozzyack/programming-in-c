# programming-in-c

A simple C learning project implementing a basic employee database to practice fundamental C programming concepts.

## Overview

This project is a command-line employee database that demonstrates core C concepts including:
- File I/O operations
- Binary data serialization
- Memory management (malloc/realloc/free)
- Command-line argument parsing
- Network byte order conversion (endianness handling)
- Data structures

## Building

```bash
make default
```

## Usage

### Create a new database
```bash
./bin/a.out -n -f mydatabase.db
```

### Add an employee
```bash
./bin/a.out -f mydatabase.db -a "John Doe,123 Main St,40"
```

Format: `"Name,Address,Hours"`

### Open/validate an existing database
```bash
./bin/a.out -f mydatabase.db
```

## Command-line Options

- `-n` - Create a new database file
- `-f <path>` - (required) Path to the database file
- `-a <data>` - Add an employee with comma-separated data (Name,Address,Hours)

## Database Structure

The database uses a binary format with:
- **Header**: Magic number, version, employee count, and file size
- **Employee records**: Fixed-size records with name (256 bytes), address (256 bytes), and hours (4 bytes)

All multi-byte values are stored in network byte order (big-endian) for portability.

## Project Structure

```
src/
  main.c   - Entry point and command-line handling
  file.c   - File operations (create/open)
  parse.c  - Database I/O and employee management
include/
  common.h - Common definitions
  file.h   - File operation headers
  parse.h  - Database structures and function headers
```

## Clean Up

```bash
make clean      # Remove object files and binaries
make clean-all  # Also remove test database files
```
