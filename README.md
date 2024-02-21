## Project brief

The program takes a database directory as input and joins the columns to produce a result to the given test task.

The program is written in C++ 20 and uses the following libraries:

- [mio](https://github.com/vimpunk/mio/blob/master/single_include/mio/mio.hpp): for memory-mapped file I/O. This is a very small single header library that provides a very basic corss-platform interface for memory mapping files. I used it for `mmap` and `munmap`. When the files are open I treat them as raw byte pointers noting more.

The program takes the following steps:

1. Read the database directory, create a list of the columns and memory map the underlining files.

2. Performs a basic planning step to determine a reasonably efficient execution order for the sequence of join operations.

    * Column groups that can be joined on both clustered keys are joined first.
    * Column groups that can be joined on at least one clustered key are joined second.
    * Column groups that can be joined on at least one non-clustered key are joined third.
    * Column groups that have no common keys with any other column group need to be cross joined. This is the slowest join to perfrom, so it is done last in the hopes that the resulting datasets would be smaller by then.
    * Some joins require a sort of the intermediate result before passing it down the chain of execution.

 3. There are 4 algorithms for joining column groups:
    * **Search join** - can join if there is at least one clustered key in common.
    * **Merge join** - can join only if both keys are clustered keys.
    * **Hash join** - can join any column group that has at least one key in common.
    * **Cross join** - can join when there are no keys in common.

## Build instructions

```bash
mkdir build
cd build
cmake .. --preset release
make -j
```

## Run instructions

```bash
./dbms <path_to_db_directory>
```

## Run tests

```bash
./dbms_test
```

## Examples

```bash
./dbms ../db/small-example
...
Total sum of squared values: 2545
Total running time: 273097ns
```

```bash
./dbms ../db/corner-case-example
...
Total sum of squared values: 93553260
Total running time: 598635ns
```

```bash
./dbms ../db/t1-example
...
Total sum of squared values: 266472168
Total running time: 02s 305ms
```
