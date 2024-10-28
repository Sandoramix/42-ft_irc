# FT_IRC

42 School - ft_irc project

## Description

This is a simple IRC server written in C++.

Made by:

- [@odudniak](https://profile.intra.42.fr/users/odudniak)
- [@afantini](https://profile.intra.42.fr/users/afantini)
- [@gbarone](https://profile.intra.42.fr/users/gbarone)

## Requirements

- **C++98 compiler** _(clang++ is used in this project)_
- **make**
- **valgrind** _(optional)_
- **curl** _(optional)_

## Getting started

### Compiling

```bash
make
```

### Running

```bash
./ircserv <port> <password>
```

#### Example

```bash
./ircserv 6667 password
```

### Testing

#### Valgrind

```bash
make valgrind
```
