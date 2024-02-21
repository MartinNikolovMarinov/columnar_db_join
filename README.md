# Build instructions

```bash
mkdir build
cd build
cmake .. --preset release
make -j
```

# Run instructions

```bash
./dbms <path_to_db_directory>
```

# Run tests

```bash
./dbms_test
```

# Examples

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
