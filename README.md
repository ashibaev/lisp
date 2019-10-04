# Prepare

`git clone ...`

`docker build -t ubuntu_gcc .`

`docker run -it -v PATH_TO_LISP:/app ubuntu_gcc bash`

`cd app`

`make LD=g++ CC=gcc CXX=g++ WARN_OPTS="-pedantic -Wall" FLAGS="-O2"`

# Usage-

Run tests: `./check`

Run lisp: `./lisp_machine FILE`

...

