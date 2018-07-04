## Building

### Installing deps in Ubuntu
$ sudo apt-get install build-essential cmake git xorg-dev libglu1-mesa-dev libusb-1.0-0-dev libsqlite3-dev

### Building
$ cd build && cmake .. && make

### Create cube records
$ ./pixo-creator db.sqlite LEDS_PER_SIDE hostname_of_fadecandy [hostname_of_fadecandy*]  # localhost is fine

### Add patterns
$ ./pixo-patterns db.sqlite ../patterns/*.glsl

### Run the thing
$ ./pixo [--fullscreen] db.sqlite
