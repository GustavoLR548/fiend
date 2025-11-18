# miniaudio

Single-header audio playback library for Fiend.

- **Version**: 0.11.21 (November 2024)
- **Author**: David Reid
- **License**: MIT-0 (Public Domain equivalent)
- **Website**: https://miniaud.io
- **Repository**: https://github.com/mackron/miniaudio

## Why miniaudio?

Fiend uses miniaudio as a lightweight, cross-platform replacement for FMOD.
It provides simple audio playback with zero dependencies and is completely
free and open source (public domain).

## Features Used in Fiend

- Simple sound loading from files (.wav, .mp3, .ogg, etc.)
- Sound playback with looping support
- Volume control
- Multiple simultaneous sounds

## Technical Details

miniaudio is embedded directly into the Fiend executable during compilation.
Users don't need to install any additional audio libraries - everything needed
is compiled in.

On Linux, miniaudio will automatically use the best available backend:
- PulseAudio (preferred)
- ALSA (fallback)
- JACK (if available)

No configuration needed - it just works!

## Integration

The miniaudio library is wrapped by `src/audio.h` and `src/audio.c` which
provide a simple API for the game to use. This abstraction makes it easy
to maintain and potentially swap audio backends in the future if needed.

## Attribution

This project uses miniaudio by David Reid. We are grateful for this excellent
library being available in the public domain.
