# Rozbiory

A simple program for checking one specific operation on sets.

## Compilation

To compile run the following commands in your favourite shell:

```sh
$ git clone https://github.com/timorl/rozbiory.git
$ cd rozbiory
$ make all
```

If you also want to run the tests type:

```sh
$ make test
```

## Usage

The binary should be in `bin/`, so to run the program type:

```sh
$ bin/rho
```

The command waits for input in the form of a json file containing two records -- `"mainSet"` and `"family"`.
The first one should contain a list of integers that will be treated as a set, and the second one a list of lists that should be subsets of `mainSet`.
Example files with such contents can be found in the `test/` directory.

Usually one wants to run the command with a file as the input and output:

```sh
$ bin/rho < input.json > output.txt
```

The program also takes one optional argument specifying the maximum number of steps to perform. If the argument is not present the default number of steps is 100000.
To run the program with the maximum number of steps set to, for example, 100 type:

```sh
$ bin/rho 100 < input.json > output.txt
```
