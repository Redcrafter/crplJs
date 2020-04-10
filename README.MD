# crplJs
crplJs is a Transpiler created to convert code with Javascript like syntax to crpl.

## Usage
To compile files specify them via the command line.\
Outputs will be created with the same file name and a .crpl extension

### Windows
Download the program from the release section. Or compile it yourself using CMake\
```
crpl.exe foo.js bar.js
```

### Linux & OSX
Compile the program using CMake
```bash
mkdir bin
cd bin
cmake ../
make

./crpl someFile.js
```

## Usage
The Syntax is basically just plain Javascript.\
There is an example file inside the sample folder.\
And a definitions file for ease of use.

All code on the base level of the file will be put into a `once` block \
All code inside the main function will be run every frame.

Input variables are declared by using const. \
`const foo = 10` -> `$foo:10`

Normal variables can be declared by using let. \
`let bar = 3.14` -> `3.14 ->bar` \
These variables will only be accessible in their current and lower scopes.

Global variables can be declared by using var. These will be accessible from everywhere \
`var global = 3.14` -> `3.14 ->global`

String concatenation is done using `+`.
There could be errors in the output code if the compiler is unsure wether to use concatenation or addition. In these cases the `concat` function can be used.\
`"Hello " + "world!"`-> `"Hello " "world!" concat`\
or\
`concat("Hello ", "world!")`-> `"Hello " "world!" concat`

Another thing to watch out for is the amount of parameters a function requires and returns.\
For example `CurrentCoords` returns 2 parameters.

# TODO
Add all native functions\
Add more examples\
Add all functions to to definition file
