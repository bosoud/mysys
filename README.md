# MySys - System Information Utility

A lightweight command-line tool that displays detailed system information for macOS and Linux systems with a beautiful colored output.

## Features

- 🖥️ **System Information**
  - Operating System details
  - Kernel version
  - Hostname
  - Machine architecture
  - Current user

- 💾 **Memory Statistics**
  - Total RAM
  - Used RAM (with percentage)
  - Free RAM
  - System uptime
  - Number of processors (macOS)

- 🌐 **Network Information**
  - Network interfaces with IP addresses
  - Default gateway (macOS)

## Screenshots

![MySys Output](screenshots/mysys-output.png)

## Installation

### Prerequisites
- GCC compiler
- Make

### Building from Source

1. Clone the repository:
```sh
git clone https://github.com/yourusername/mysys.git
cd mysys
```

2. Build the project:
```sh
make
```

3. Install system-wide (optional):
```sh
sudo make install
```

## Usage

Simply run the command:
```sh
mysys
```

## Platform Support

- ✅ macOS (Full support)
- ✅ Linux (Basic support)
- ❌ Windows (Not supported)

## Building

The project uses a simple Makefile system:
- `make` - Builds the executable
- `make install` - Installs to /usr/local/bin
- `make clean` - Removes built files

## Contributing

1. Fork the repository
2. Create your feature branch (`git checkout -b feature/amazing-feature`)
3. Commit your changes (`git commit -m 'Add some amazing feature'`)
4. Push to the branch (`git push origin feature/amazing-feature`)
5. Open a Pull Request

## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

## Author

Your Name - [GitHub Profile](https://github.com/yourusername)