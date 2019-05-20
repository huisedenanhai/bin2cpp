# bin2cpp

Dump bytes from a given binary file and write them as a C++ unsigned char array.

This is a byproduct of my another working-on project.

## Compile

```bash
g++ -std=c++11 -o bin2cpp bin2cpp.cpp
```

## Usage

Given a valid binary file, bin2cpp will produce C++ code with the following format

```c++
namespace GivenNameSpace {
unsigned char array_name[] = {
    /* dumped bytes from the given binary file */
};
unsigned long long array_name_length = /* element count of the array */;
}
```

Run `bin2cpp` without argument, you will see the detailed usage info

```
bin2cpp usage:
  bin2cpp [-c] [-ce] [-s] [-z] [-ns namespace] [-n var_name] [-i indent] [-m line_max] [-h] filename
  -c const
  -ce constexpr
  -s static
  -z add trailing zero (helpful for text input file)
  -ns namespace
  -n name of the array variable, use filename by defualt
  -i indent, 2 by default
  -m max char cnt in one line, 80 by defualt
  -h show this msg
  filename the input filename
```

`-z` option will add a trailing `0x00` in the resulting array, which comes handy when the input file is a text file, a feature I didn't found in other implementations, the reason for rewriting the whole stuff myself.

It should be noticed that `array_name_length` represents the total element number in the array, including the possibly added trailing zero.

