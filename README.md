# bg_theme

**bg_theme** is a very simple background music controller.

## But why?

To give your desktop a gimmicky background theme.

## Dependencies

- A C compiler
- Libc
- [mpv](https://mpv.io)
- Your own background theme audio file

## Build

Type `make`. You may want to copy `config.def.h` to `config.h` and edit `config.h` to suit your needs. All paths are relative from your home directory.

## Usage

Type `bg_theme` without arguments to toggle whether the background theme is played or not.

The program behaviour can be influenced by setting the following environment variables:

- `BG_THEME` can point to an alternative audio file to be played.
- `PIDFILE` can be used to to store files which manage the state of the program at a different location.
