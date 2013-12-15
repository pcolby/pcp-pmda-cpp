To run these unit tests:

```sh
mkdir -p build
cd build
cmake .. && make && ./test
cd ..
```

Or if you prefer `pushd` and `popd`:
```bash
mkdir -p path/to/some/build/dir
pushd path/to/some/build/dir
cmake `dirs +1` && make && ./test
popd
```
