# mwc - Multithreaded Word Count

## Overview

`mwc` is a C utility program designed to efficiently count the number of lines, words, and characters contained in a file, or from standard input to standard output. Inspired by the traditional `wc` utility, `mwc` offers the additional capability of multithreading and concurrency for enhanced performance.

## Features

- **Counting Functionality:** `mwc` counts the number of lines, words, and characters in a file or set of files.
- **Multithreading:** Utilizes multithreading to enhance performance, particularly useful for large files or multiple files.
- **Customizable Output:** Users can specify whether they want to count lines, words, characters, or all three.
- **Command-Line Interface:** Simple and intuitive command-line interface for ease of use.

## Installation

To build and install `mwc`, follow these steps:

1. Clone the repository:

   ```bash
   git clone https://github.com/Ferny730JR/mwc.git
   ```

2. Navigate to the cloned directory:

   ```bash
   cd mwc
   ```

3. Build the project using CMake:

   ```bash
   cmake .
   make
   ```

4. Install the executable (optional):

   ```bash
   sudo make install
   ```

## Usage

```
Usage: mwc [OPTIONS] [<file1>] [<file2>]...

Count the number of lines, words, and characters in a file.

Options:
  -h, --help         Print help and exit
  -V, --version      Print version and exit
  -c, --characters   Count the number of bytes in each input file, and write it to standard output. (default=off)
  -l, --lines        Count the number of lines in each input file, and write it to standard output. (default=off)
  -w, --words        Count the number of words in each input file, and write it to standard output. (default=off)
  -t, --threads=INT  Specify the number of threads to use. (default=`0')
```

## Examples

Count lines, words, and characters in a single file:

```bash
mwc file.txt
```

Count lines and characters in multiple files with 4 threads:

```bash
mwc -l -c -t 4 file1.txt file2.txt file3.txt
```

## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

## Acknowledgments

- This program is heavily inspired by the traditional `wc` utility.
