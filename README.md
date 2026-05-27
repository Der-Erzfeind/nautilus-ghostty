# nautilus-ghostty

A Nautilus extension that adds an **"Open in Ghostty"** context menu entry when right-clicking folders or the background in Nautilus.

## Requirements

- [Ghostty](https://ghostty.org/) installed and available in `$PATH`
- `nautilus-devel`
- `gcc`

```bash
sudo dnf install nautilus-devel gcc
```

## Build & Install

```bash
gcc -fPIC -shared nautilus-ghostty.c -o libnautilus-ghostty.so \
  $(pkg-config --cflags --libs gio-2.0 glib-2.0 gobject-2.0 libnautilus-extension-4)

sudo cp libnautilus-ghostty.so /usr/lib64/nautilus/extensions-4/
nautilus -q
```

## Uninstall

```bash
sudo rm /usr/lib64/nautilus/extensions-4/libnautilus-ghostty.so
nautilus -q
```

## Notes

- Tested on Fedora 44 with Nautilus 50 and Ghostty installed via `dnf`
- The extension uses `--working-directory=/path` syntax required by Ghostty
- Inspired by [nautilus-ptyxis](https://codeberg.org/jakedane/nautilus-ptyxis) by Jake Dane
