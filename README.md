# C-Shell

A custom Unix-like shell implementation in C that provides a comprehensive command-line interface with support for process management, I/O redirection, piping, job control, and custom built-in commands.

## 📋 Table of Contents

- [Overview](#overview)
- [Features](#features)
- [Prerequisites](#prerequisites)
- [Installation](#installation)
- [Running the Shell](#running-the-shell)
- [Built-in Commands](#built-in-commands)
- [Advanced Features](#advanced-features)
- [Project Structure](#project-structure)
- [Testing](#testing)
- [Implementation Details](#implementation-details)
- [Signal Handling](#signal-handling)
- [Author](#author)

## 🔍 Overview

C-Shell is a Unix-like shell implemented in C that mimics the functionality of popular shells like bash and zsh. It features a custom prompt, command parsing, process execution, job control, and signal handling. The shell is built with modularity in mind, separating concerns across multiple source files for maintainability and scalability.

## ✨ Features

### Core Features
- **Custom Shell Prompt**: Displays username, hostname, and current working directory
- **Command Execution**: Execute system commands and custom built-in commands
- **Process Management**: Support for both foreground and background process execution
- **I/O Redirection**: Input (`<`), output (`>`), and append (`>>`) redirection
- **Piping**: Chain multiple commands using pipes (`|`)
- **Command Chaining**: Support for sequential (`;`) and conditional (`&&`) command execution
- **Command History**: Persistent command logging with execution replay
- **Job Control**: Background job management with process tracking
- **Signal Handling**: Graceful handling of Ctrl+C, Ctrl+Z, and child process termination

### Built-in Commands
1. **`hop`** - Enhanced directory navigation
2. **`reveal`** - Advanced directory listing with flags
3. **`log`** - Command history management
4. **`activities`** - Display all background processes
5. **`ping`** - Send signals to processes
6. **`fg`** - Bring background jobs to foreground
7. **`bg`** - Resume stopped jobs in background

## 🔧 Prerequisites

Before building and running C-Shell, ensure you have the following installed:

- **GCC Compiler** (version supporting C99 standard)
- **Make** utility
- **POSIX-compliant system** (Linux, macOS, or Unix-like OS)
- **Python 3** (for running tests)
- **pip** (for installing test dependencies)

### System Requirements
- Operating System: Linux/Unix/macOS
- Memory: Minimum 256 MB RAM
- Disk Space: ~10 MB for compilation and execution

## 🚀 Installation

1. **Clone the repository:**
   ```bash
   git clone https://github.com/TECHIE-TITAN/C-Shell.git
   cd C-Shell
   ```

2. **Compile the shell:**
   ```bash
   make all
   ```
   This will:
   - Create a `build/` directory for object files
   - Compile all source files from `src/` directory
   - Generate the executable `shell.out`

3. **Verify compilation:**
   ```bash
   ls -l shell.out
   ```
   You should see the `shell.out` executable file.

## 🎯 Running the Shell

### Basic Usage

```bash
./shell.out
```

Or use the make target:

```bash
make run
```

### Shell Prompt Format

```
<username@hostname:current_directory>
```

Example:
```
<vansh@ubuntu:~/C-Shell>
```

The tilde (`~`) represents the home directory of the shell session.

### Exiting the Shell

- Type `exit` or press `Ctrl+D` (EOF)
- The shell will terminate all child processes before exiting

## 📚 Built-in Commands

### 1. `hop` - Directory Navigation

Navigate between directories with enhanced features.

**Syntax:**
```bash
hop [path]
```

**Examples:**
```bash
hop                 # Go to home directory
hop ~               # Go to home directory
hop .               # Stay in current directory
hop ..              # Go to parent directory
hop -               # Go to previous directory
hop /path/to/dir    # Go to specific directory
```

**Features:**
- Maintains previous directory for quick switching
- Supports absolute and relative paths
- Handles special symbols (`~`, `.`, `..`, `-`)

### 2. `reveal` - Enhanced Directory Listing

Display directory contents with optional flags.

**Syntax:**
```bash
reveal [flags] [path]
```

**Flags:**
- `-a` : Show hidden files (files starting with `.`)
- `-l` : Display in long format (one file per line)

**Examples:**
```bash
reveal              # List current directory
reveal -a           # Show hidden files
reveal -l           # Long format listing
reveal -al ~        # Show all files in home directory (long format)
reveal /etc         # List specific directory
```

**Features:**
- Lexicographically sorted output
- Support for multiple flags
- Works with special directory symbols

### 3. `log` - Command History Management

Manage and replay command history.

**Syntax:**
```bash
log                 # Display command history
log purge           # Clear all command history
log execute <index> # Execute command at index
```

**Examples:**
```bash
log                 # Show last 15 commands
log execute 3       # Execute the 3rd command from history
log purge           # Clear all history
```

**Features:**
- Stores up to 15 most recent commands
- Persistent storage across shell sessions
- Command deduplication (consecutive duplicates ignored)
- Indexed execution for quick replay

### 4. `activities` - Background Job Listing

Display all running and stopped background processes.

**Syntax:**
```bash
activities
```

**Output Format:**
```
[job_number] : command - Status
```

**Example Output:**
```
[1] : sleep 100 - Running
[2] : vim file.txt - Stopped
```

**Features:**
- Lexicographically sorted by command name
- Shows job number, command, and status
- Status: Running or Stopped

### 5. `ping` - Process Signal Management

Send signals to processes by PID.

**Syntax:**
```bash
ping <pid> <signal_number>
```

**Examples:**
```bash
ping 1234 9         # Send SIGKILL (signal 9) to process 1234
ping 5678 15        # Send SIGTERM (signal 15) to process 5678
```

**Features:**
- Signal number modulo 32 for safety
- Process existence validation
- Error handling for invalid PIDs

### 6. `fg` - Foreground Job Control

Bring a background or stopped job to the foreground.

**Syntax:**
```bash
fg <job_number>
```

**Examples:**
```bash
fg 1                # Bring job [1] to foreground
```

**Features:**
- Resumes stopped jobs
- Waits for job completion
- Updates job status

### 7. `bg` - Background Job Control

Resume a stopped job in the background.

**Syntax:**
```bash
bg <job_number>
```

**Examples:**
```bash
bg 2                # Resume job [2] in background
```

**Features:**
- Sends SIGCONT to stopped jobs
- Updates job status to running
- Non-blocking operation

## 🔥 Advanced Features

### Background Process Execution

Append `&` to run commands in the background:

```bash
sleep 100 &
firefox &
./long_running_script.sh &
```

**Features:**
- Automatic job number assignment
- Non-blocking execution
- Process completion notifications

### I/O Redirection

**Input Redirection:**
```bash
wc < input.txt
./program < data.txt
```

**Output Redirection:**
```bash
ls > output.txt          # Overwrite
echo "text" >> file.txt  # Append
```

**Combined:**
```bash
sort < input.txt > sorted.txt
```

### Piping

Chain multiple commands:

```bash
ls -l | grep ".c" | wc -l
cat file.txt | sort | uniq | grep "pattern"
ps aux | grep "process" | awk '{print $2}'
```

**Features:**
- Unlimited pipe chaining
- Efficient inter-process communication
- Proper error handling

### Command Chaining

**Sequential Execution (`;`):**
```bash
cd src ; ls ; pwd
```
Executes all commands sequentially, regardless of success/failure.

**Conditional Execution (`&&`):**
```bash
make clean && make all && ./shell.out
```
Executes next command only if previous succeeded.

**Background Execution (`&`):**
```bash
command1 & command2 & command3
```
All commands run in background simultaneously.

### Signal Handling

- **Ctrl+C (SIGINT)**: Terminates foreground process, shell continues
- **Ctrl+Z (SIGTSTP)**: Stops foreground process, moves to background
- **Ctrl+D (EOF)**: Graceful shell exit with cleanup
- **SIGCHLD**: Automatic background process reaping

## 📁 Project Structure

```
C-Shell/
├── Makefile                 # Build configuration
├── README.md                # This file
├── requirements.txt         # Python test dependencies
├── shell.tcl                # TCL test script
├── test.py                  # Python test suite
├── shell.out                # Compiled executable (after build)
│
├── data/                    # Runtime data files
│   ├── logs.txt            # Command history storage
│   └── logs_stat.txt       # Log metadata
│
├── include/                 # Header files
│   └── header.h            # Main header with declarations
│
├── src/                     # Source files
│   ├── main.c              # Entry point and main loop
│   ├── prompter.c          # Shell prompt display
│   ├── tokeniser.c         # Input tokenization
│   ├── parser.c            # Command parsing
│   ├── executor.c          # Command execution engine
│   ├── function.c          # Built-in command implementations
│   ├── log_ops.c           # Command history operations
│   ├── job_manager.c       # Background job management
│   ├── signal_handler.c    # Signal handling logic
│   └── utility.c           # Utility functions
│
├── build/                   # Object files (generated)
│   └── *.o                 # Compiled object files
│
└── __pycache__/            # Python cache (generated)
```

## 🧪 Testing

### Install Test Dependencies

```bash
pip install -r requirements.txt
```

This installs:
- `pytest` - Testing framework
- `pexpect` - Process control and automation
- `psutil` - Process and system monitoring
- Additional dependencies for test execution

### Run Tests

```bash
pytest test.py -v
```

**Test Coverage:**
- Shell prompt format validation
- Built-in command functionality
- I/O redirection operations
- Piping mechanisms
- Background process handling
- Signal handling behavior

### Manual Testing

```bash
# Start the shell
./shell.out

# Test basic commands
<user@host:~> ls -la
<user@host:~> pwd

# Test built-in commands
<user@host:~> hop ..
<user@host:/home> reveal -al
<user@host:/home> log

# Test I/O redirection
<user@host:~> echo "Hello" > test.txt
<user@host:~> cat < test.txt

# Test piping
<user@host:~> ls | grep ".c"

# Test background jobs
<user@host:~> sleep 50 &
<user@host:~> activities
```

## 🔨 Implementation Details

### Tokenization

The tokenizer (`tokeniser.c`) converts raw input into structured tokens:

**Token Types:**
- `NAME` - Command names and arguments
- `PIPE` - Pipe operator (`|`)
- `INPUT/OUTPUT` - I/O redirection (`<`, `>`, `>>`)
- `AND` - Conditional execution (`&&`)
- `SEMICOLON` - Sequential execution (`;`)
- `BACKGROUND` - Background execution (`&`)
- `END` - End of input

### Parsing

The recursive descent parser (`parser.c`) validates command syntax:

```
parse → cmd_group [(&& | ;) cmd_group]* [&]
cmd_group → atomic [| atomic]*
atomic → NAME [INPUT | OUTPUT]*
```

### Execution

The executor (`executor.c`) handles:
1. **Command type identification** (built-in vs system)
2. **Process creation** (`fork()` for system commands)
3. **I/O redirection** setup
4. **Pipe creation** and management
5. **Background job tracking**
6. **Error handling** and recovery

### Job Management

Background jobs are tracked using a job list:

```c
typedef struct {
    pid_t pid;              // Process ID
    char command[MAX_LENGTH]; // Command string
    int job_number;          // Job identifier
    int status;             // 0=running, 1=stopped
} Job;
```

**Operations:**
- `add_job()` - Register new background job
- `remove_job()` - Remove completed job
- `get_job_by_number()` - Lookup by job number
- `update_job_status()` - Update job state

### Command History

Persistent logging system (`log_ops.c`):
- Stores commands in `data/logs.txt`
- Maintains metadata in `data/logs_stat.txt`
- Circular buffer for 15 most recent commands
- Automatic deduplication

## 🛡️ Signal Handling

### Implemented Signals

1. **SIGINT (Ctrl+C)**
   - Terminates foreground process only
   - Shell remains active
   - No effect if no foreground process

2. **SIGTSTP (Ctrl+Z)**
   - Stops foreground process
   - Converts to background job
   - Job can be resumed with `fg` or `bg`

3. **SIGCHLD**
   - Automatic zombie process reaping
   - Background job completion notification
   - Non-blocking status checks

### Signal Handler Setup

```c
void setup_signal_handlers() {
    signal(SIGINT, handle_sigint);
    signal(SIGTSTP, handle_sigtstp);
    signal(SIGCHLD, handle_sigchld);
}
```

## 🧹 Cleaning Up

Remove compiled files and reset the build:

```bash
make clean
```

This removes:
- `build/` directory and all object files
- `shell.out` executable

## 🐛 Troubleshooting

### Compilation Errors

**Error: `gcc: command not found`**
```bash
# Install GCC on Ubuntu/Debian
sudo apt-get update
sudo apt-get install build-essential

# Install on macOS
xcode-select --install
```

**Error: Missing headers**
- Ensure you're on a POSIX-compliant system
- Check that all source files are present in `src/`

### Runtime Issues

**Issue: Command not found**
- Ensure the command exists in your system PATH
- Use absolute paths for custom executables

**Issue: Background jobs not showing**
- Use `activities` to list all jobs
- Check if processes terminated immediately

**Issue: Signal handling not working**
- Verify you're running on a Unix-like system
- Check terminal emulator compatibility

## 📝 Notes

- The shell maintains its own home directory context from startup
- Previous directory (`hop -`) is tracked per shell session
- Command history persists across shell sessions
- Maximum 15 commands stored in history (configurable in `header.h`)
- Maximum 256 background jobs supported (configurable in `header.h`)

## 🤝 Contributing

Contributions are welcome! To contribute:

1. Fork the repository
2. Create a feature branch (`git checkout -b feature/amazing-feature`)
3. Commit your changes (`git commit -m 'Add amazing feature'`)
4. Push to the branch (`git push origin feature/amazing-feature`)
5. Open a Pull Request

## 📄 License

This project is part of an academic assignment. Please refer to your institution's policies on code sharing and collaboration.

## 👨‍💻 Author

**TECHIE-TITAN**
- GitHub: [@TECHIE-TITAN](https://github.com/TECHIE-TITAN)
- Repository: [C-Shell](https://github.com/TECHIE-TITAN/C-Shell)

## 🙏 Acknowledgments

- Unix/Linux shell documentation and man pages
- POSIX standards for system calls
- Course instructors and teaching assistants
- Open-source community for inspiration

---

**Last Updated:** October 2025

For issues, questions, or suggestions, please open an issue on the GitHub repository.
