# Rozbiory

A simple program for checking one specificoperation on sets.

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
