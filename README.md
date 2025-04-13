# Minishell

A simple Unix-style command-line interpreter written in C. This project serves as an educational exercise in understanding system-level programming concepts such as process management, signal handling, and command parsing.

## Table of Contents

- [Features](#features)
- [Installation](#installation)
  - [Prerequisites](#prerequisites)
  - [Compilation](#compilation)
- [Usage](#usage)
- [Built-in Commands](#built-in-commands)
- [Code Structure](#code-structure)
- [Author](#author)
- [License](#license)
- [Acknowledgments](#acknowledgments)

## Features

- **Interactive Prompt**: Displays the current working directory in blue before the command prompt.
- **Signal Handling**: Implements a SIGINT handler to manage interruptions gracefully.
- **Built-in Commands**: Supports internal commands such as `cd`, `pwd`, `lf` (list files), `lp` (list processes), and `echo`.
- **External Commands**: If a command is not built in, the shell forks a new process and executes it using `execvp`.
- **Error Reporting**: Provides helpful error messages when operations fail (e.g., when changing directories or executing commands).

## Installation

### Prerequisites

- A Unix/Linux-based operating system.
- A C compiler (for example, GCC).

### Compilation

Compile the source code with the following command:

```bash
gcc -Wall -Wextra -Werror minishell.c -o minishell
