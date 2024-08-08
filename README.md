
# Ling Language

Ling is a simple, interpreted programming language created for fun and learning. It provides basic programming principles and is designed to be easy to use. Ling is a WIP and everything is subject to change.

## Features

- **Variable Declaration**: Create and manipulate variables easily.
- **Print Statements**: Output text and variable contents to the console.
- **Basic Arithmetic**: Perform addition, multiplication and division operations on integers.
- **File-based Execution**: Run Ling scripts from text files.
- **Basic If Statements**: Compare values easily.

## Usage

### Running a Ling Script

To run a Ling script, use the following command from a folder containing Ling.exe:

```
Ling script.ling
```

### Syntax

#### Printing

To print a line of text:

```
PrintLine("yello warld")
```

To print the value of a variable:

```
PrintLine(VariableName)
```

You can also use `Print` to not make a new line, call them the same way.
These are the only existing functions.
Functions can't have arithmetic as a parameter just yet.

#### Variables

To declare and initialize a variable:

```
Variable MyVariable = 123

```

Variables can store strings or numbers, but are orientated towards numbers

#### If Statements

Determine whether or not to run code by following this basic process.

If 1 == 1 Run
{

}

Not following this syntax can cause errors.

As of now, they do not support variables (next update)

#### Arithmetic

Basic addition is supported for integers:

```
Variable a = 5 + 5
Variable b = a / 2
Variable result = a * b
Variable sum = result + 10
PrintLine(sum)
```
Result: `60`

There is no order of operations.

#### Lexer Defines
Lexer defines change the way the lexer behaves when interpreting your script.

Lexer defines can be used with:
`DEFINE TREAT_WARNINGS_AS_FATAL 1`
or
`DEFINE TREAT_WARNINGS_AS_FATAL false`

 - `TREAT_WARNINGS_AS_FATAL`
	 Changes the way warnings are treated, if the lexer discovers a warning and this is true or 1, your script will stop being interpreted and the lexer will stop.

## Examples

You can find an example script in `script.ling`