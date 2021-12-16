# Aermoss AeNCPP
A programming language made with C++.

## Getting Started
1) To execute a AeNCPP code open command prompt and type:

```
main <filename>
```

## Examples
# Hello World
``` go
include "ioaencpp"

func main() {
    ioaencpp.print("Hello, World!")
}
```

# Command Prompt
``` go
include "ioaencpp"

func main() {
    var command = ""
    var destroyed = false

    while (not destroyed) {
        command = ioaencpp.input(">")

        if (command == "exit") {
            ioaencpp.print("Exiting...")
            destroyed = true
        }

        if (command != "exit") {
            system(command)
        }
    }
}
```
