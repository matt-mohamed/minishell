Minishell
A simple Unix-style command-line interpreter written in C. This project serves as an educational exercise in understanding system-level programming concepts such as process management, signal handling, and command parsing.

Table of Contents
Features

Installation

Usage

Built-in Commands

Code Structure

Author

License

Acknowledgments

Features
Interactive Prompt: Displays the current working directory in blue before the command prompt.

Signal Handling: Implements a SIGINT handler to manage interruptions gracefully.

Built-in Commands: Supports internal commands such as cd, pwd, lf (list files), lp (list processes), and echo.

External Commands: If a command is not built in, the shell forks a new process and executes it using execvp.

Error Reporting: Provides helpful error messages when operations fail (e.g., when changing directories or executing commands).

Installation
Prerequisites
A Unix/Linux-based operating system.

A C compiler (for example, GCC).

Compilation
Compile the source code with the following command:

bash
Copy
gcc -Wall -Wextra -Werror minishell.c -o minishell
Usage
Run the shell after compiling:

bash
Copy
./minishell
Once running, you can use the shell to execute both built-in and external commands. For example:

bash
Copy
[minishell PATH] > cd /path/to/directory
[minishell PATH] > pwd
[minishell PATH] > lf
[minishell PATH] > lp
[minishell PATH] > echo Hello, World!
[minishell PATH] > exit
Built-in Commands
cd: Change the current working directory. When no argument is provided (or using ~), it defaults to the user's home directory.

pwd: Print the current working directory.

lf: List files in the current directory, skipping the . and .. entries.

lp: List processes from the /proc directory along with the associated username and command details.

echo: Print text to the console.

exit: Terminate the minishell session.

Code Structure
Signal Handling: A custom SIGINT handler is installed to manage interruptions and clean up properly.

Command Parsing: The main loop reads user input, extracts the command and arguments, and routes to either built-in functionality or external command execution.

Process Management: Uses fork() to create child processes and execvp() to execute external commands. The parent process waits for the child process to complete.

Directory Operations: Implements commands for changing directories (cd) and displaying the current working directory (pwd).

Custom Listings: Provides functions to list files (lf) and processes (lp).

Author
Matthew Mohamed

Pledge: I pledge my honor that I have abided by the Stevens Honor System.

License
This project is provided "as is" without any warranty. Feel free to use, modify, and distribute it according to your needs.
