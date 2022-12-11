# field

![GitHub tag (latest by date)](https://img.shields.io/github/v/tag/jibstack64/field)
![GitHub all releases](https://img.shields.io/github/downloads/jibstack64/field/total)
![GitHub](https://img.shields.io/github/license/jibstack64/field)

*A subcommand-based argument parsing library for C++.*

### Information
`field` is *not* a replacement for [argh!](https://github.com/adishavit/argh) and similar libraries, it is for use in specific types of programs, as described below.

**It is not for multi-argument parsing**. `field` is for programs where, instead of having traditional GNU-style `--arg` arguments, you have so-called 'command arguments', that act as individual parts of the program.

> If you insist on using `field` for multi-argument parsing, you may set `parser.disableLock` to true. This will allow multiple subcommands to be used, instead of locking to one.

For example, say you are developing a package manager that is solely controlled using command-line arguments. When installing a package, you want the command-line syntax looks something like this:

`package-manager install <package>`

Well? Perfect! This is exactly what `field` is for, segmenting your program into individual chunks that play their own part in the application.

*How does it work?*
The basic idea is that when an argument is passed, it has a matching function that is called alongside it - a `field::context` object and an `std::vector<const char *>` are passed to the function, allowing the function to access the flags passed, and the values passed to it through the arguments.

*What about flags?*
They are completely supported. As long as they are passed before the subcommand, or after all of the subcommand's positional arguments.

### Basic syntax
The syntax in `field` is relatively basic, and is designed to be as sleek as possible.

A basic application would look something like this:
```cpp
#include <iostream>
#include <vector>

#include ".../field.hpp"

void say(field::context& ctx, std::vector<const char *> values) {
    // as we did not specify a limit to the number of positional arguments,
    // values will include all positional argument values after 'say'
    for (const auto& c : values) {
        std::cout << c << std::endl;
    }
}

int main(int argc, char ** argv) {
    // create the argument parser
    field::parser parser;

    // add our argument
    // we pass our 'say' function
    // this will be ran when the argument is passed
    field::arg sayArg = parser.add("say", say);

    // now we parse
    parser.parse(argc, argv);
    // or alternatively drop the argc
    // parser.parse(argv);

    return 0;
}
```
Lets run it:

`./out.o say "hello world!" "and FOSS developers!!!"`

Output:
```
hello world!
and FOSS developers!!!
```

---

### Context
Context (`field::context`) is passed to every "runner" function (a function that is called when an argument is passed). It contains a list of flags passed, and any values passed outside of the boundaries of the argument.

---

### Subcommands

Subcommands can be added with the `parser.add(...)` function. You pass a name, and optionally a function and `takes` (the number of positional arguments the subcommand receives).

For instance, if I wanted a basic text-mirroring program:
```cpp
#include <iostream>
#include <vector>

#include ".../field.hpp"

// this is ran when "hello-world" is passed
void mirror(field::context& _, std::vector<const char *> text) {
    for (const auto& c : text) {
        std::cout << c << std::endl;
    }
}

int main(int argc, char ** argv) {
    field::parser parser;

    // we now add the subcommand with its matching function
    //                      number of pos. arguments
    parser.add("mirror", mirror, 3);

    parser.parse();

    return 0;
}
```

Now, when we run the program, it takes a maximum of 3 positional arguments:

`./out.o mirror hey! im very cool`

Output:
```
hey!
im
very
```

As you can see, `cool` was cut off due to the `takes` (number of taken values) being `3`.

### Flags

Flags can be named in any way you would prefer, however I recommend using the GNU-style `-` and `--` prefixes to distinguish them from subcommands.

Flags are very similar to arguments, lets try making a simple hello world program:
```cpp
#include <iostream>

#include ".../field.hpp"

void helloWorld(field::context& _) {
    std::cout << "hello world!" << std::endl;
}

int main(int argc, char ** argv) {
    field::parser parser;

    parser.addFlag("--hello-world", helloWorld);

    parser.parse();

    return 0;
}
```

And when we run it:

`./a.out --hello-world`

Output:
```
hello world!
```

---

### Hate using functions?
If you would rather not pass functions around, and instead have a series of if statements, you can do that too.

Instead of passing a function, only pass a name, and store the pointer to the argument provided:
```cpp
#include ".../field.hpp"

int main(int argc, char ** argv) {
    field::parser parser;
    
    // no function passed
    field::arg* ar = parser.add("no-func");

    parser.parse();

    if (ar->passed) {
        std::cout << "passed values:" << std::endl;
        for (const auto& v : ar->values()) {
        std::cout << v << std::endl;
        }
        std::cout << "passed flags:" << std::endl;
        for (auto& fl : parser.ctx.passedFlags) {
            std::cout << fl->name << std::endl;
        }
        std::cout << "overflowing values:" << std::endl;
        for (auto& of : parser.ctx.overflowValues) {
            std::cout << of << std::endl;
        }
    }
}

```

As you can see, this allows you to access the `passed` boolean of the argument, and run code based on if statements, rather than providing a function to be ran.

*Not only this*, you can access the context outside of a function by accessing the `ctx` variables of your `parser`.
