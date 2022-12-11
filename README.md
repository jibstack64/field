# field

A subcommand-based argument parsing library for C++.

### Information
`field` is *not* a replacement for [argh!](https://github.com/adishavit/argh) and similar libraries, it is for use in specific types of programs, as described below.

**It is not for multi-argument parsing**. `field` is for programs where, instead of having traditional GNU-style `--arg` arguments, you have so-called 'command arguments', that act as individual parts of the program.

> If you insist on using `field` for multi-argument parsing, you may set `parser.disableLock` to true. This will allow multiple subcommands to be used, instead of locking to one.

For example, say you are developing a package manager that is solely controlled using command-line arguments. When installing a package, you want the command-line syntax looks something like this:

`package-manager install <package>`

Well? Perfect! This is exactly what `field` is for, segmenting your program into individual chunks that play their own part in the application.

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

### Sub-commands

Sub-commands can be added with the `parser.add(...)` function. You pass a name, and optionally a function and `takes` (the number of positional arguments the sub-command receives).

For instance, if I wanted a basic mirroring program:
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

    // we now add the sub-command with its matching function
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

Flags can be named in any way you would prefer, however I recommend using the GNU-style `-` and `--` prefixes to distinguish them from sub-commands.

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