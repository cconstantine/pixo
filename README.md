## Building

### Installing deps in Ubuntu
$ sudo apt-get install build-essential cmake git xorg-dev libglu1-mesa-dev libpq-dev [ postgresql ]

### Configure database
$ sudo -u postgres createuser ${USER}
$ sudo -u postgres createdb -O ${USER} ${USER}

### Building
$ cd build && cmake .. && make

If you do not wish to compile the person tracker:
$ cd build && cmake .. && make

### Create cube records
$ ./pixo-creator SCULPTURE_NAME LEDS_PER_SIDE hostname_of_fadecandy [hostname_of_fadecandy*]  # localhost is fine

### Add patterns
$ ./pixo-patterns SCULPTURE_NAME ../patterns/*.glsl

### Run the thing
$ ./pixo [--fullscreen] SCULPTURE_NAME
