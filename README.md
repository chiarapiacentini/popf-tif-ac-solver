# popf-tif-ac-solver
AC power flow solver for popf-tif

### Installation

Get the prerequisites:
```sh
sudo apt-get install cmake armadillo
```

Create a new directory:
```sh
mkdir ac-solver
cd ac-solver
```

Get the code:
```sh
git clone https://github.com/chiarapiacentini/popf-tif-ac-solver src
```

Configure the sources for building:
```sh
cd src
./build-instructions.txt 
```

Build the external solver:
```sh
cd ../build
make network 
```

This creates a dynamic library libnetwork.so

### Usage

To run POPF-TIF with the ac solver

```sh
./popf3-clp -x ac-domain problemfile libnetwork.so networkfile
```

The input file of the external solver contains the network configuration in the [Matpower](http://www.pserc.cornell.edu//matpower/) format.


For more detailed information, please visit our Wiki Page.