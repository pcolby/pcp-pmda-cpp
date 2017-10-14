To build the `pcp-pmda-cpp` RPMS:

1. Configure your local build environment, if not already:
  ```bash
  mkdir -p ~/rpmbuild/{BUILD,RPMS,SOURCES,SPECS,SRPMS}
  [ -e ~/.rpmmacros ] || echo '%_topdir %(echo $HOME)/rpmbuild' > ~/.rpmmacros
  ```

2. Install the pre-requisites, if not already:
  ```bash
  sudo yum install boost cmake pcp-libs pcp-libs-devel
  ```

3. Download the source archive:
  ```bash
  wget --content-disposition --directory-prefix=$HOME/rpmbuild/SOURCES \
      https://github.com/pcolby/pcp-pmda-cpp/archive/v<VERSION>.tar.gz
      https://github.com/pcolby/pcp-pmda-cpp/releases/tag/v<VERSION>
  ```

4. Download the spec file:
  ```bash
  wget --directory-prefix=$HOME/rpmbuild/SPECS \
      https://raw.githubusercontent.com/pcolby/pcp-pmda-cpp/master/package/rpm/pcp-pmda-cpp.spec
  ```

5. Build the RPMs:
  ```bash
  rpmbuild -ba ~/rpmbuild/SPECS/pcp-pmda-cpp.spec
  ```

This will produce the following RPMs:
* `pcp-pmda-cpp-debuginfo` - debug info for the `pcp-pmda-cpp-*` packages.
* `pcp-pmda-cpp-devel` - development headers for the PMDA++ library.
* `pcp-pmda-cpp-examples` - example PMDAs (binaries, and source).
