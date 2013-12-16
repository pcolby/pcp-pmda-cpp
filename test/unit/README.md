
### Building / Running

To run these unit tests:

```sh
mkdir -p build
cd build
cmake .. && make check
cd ..
```

Or if you prefer `pushd` and `popd`:
```bash
mkdir -p path/to/some/build/dir
pushd path/to/some/build/dir
cmake `dirs +1` && make check
popd
```

### Requirements

* cmake 2.8+
* Boost 1.32+
* PCP development headers
* Network access to checkout (Google C++ Test Framework)[http://googletest.googlecode.com/svn/trunk/]
