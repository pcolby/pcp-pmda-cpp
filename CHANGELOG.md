## PMDA++ Changelog

### 0.4.5 (_unreleased_)
Features:
- support for PCP 4.0.0+

Special thanks to @lberk for contributing to this release.

### 0.4.4 (2017-10-14)
Features:
- support for PCP 3.11.5+ ([41dd3e2](
  ../../commit/41dd3e29c4cab6bfa3b2e3d7a0a681f6f2f27899))
- minor performance tweaks ([898ee68](
  ../../commit/898ee6816fc3e77a319ed0f61973a482e222eacc))

Bug fixes:
- Explicit types for `pcp::unit` ([f495c3a](
  ../../commit/f495c3a5febe9b569f42f5a9b74cd073e0c38c7a))

### 0.4.3 (2016-07-13)
Features:
- C++11 support ([817e66f](
  ../../commit/817e66f0ba5bf94cb2234216b33ebf86fba86bc0))
- Coverity Scan automation with Travis CI ([2973](
  https://scan.coverity.com/projects/2973))
- Clang `scan-build` automation with Travis CI ([83032bc](
  ../../commit/83032bcb11fa571de145054c4a97f3df3e342e42))
- numerous build and CI improvements.
- additional unit tests.

Bug fixes:
- broken `--unix` command line option ([f7c20c5](
  ../../commit/f7c20c5984a7a78c0255984357f55fcce9ae980e))
- incorrect instance ID reported in two exception messages ([1d5e544](
  ../../commit/1d5e544be3a8525dc2df333813304269135a69f5))
- `cout` format flags left modified ([af388d9](
  ../../commit/af388d9c8d878e2322d4fd0ce378e3d34fa84379))
- `pcp::on_store` may return prematurely ([a3334c4](
  ../../commit/a3334c42b27f88d7ef4410003b297e95d8da5675))
- template functions not fully specialized ([1ad276f](
  ../../commit/1ad276f889c5dd08f268c632c5d64b6dbfc9ffae))

Special thanks to @badone and @bchoi for contributing to this release.

### 0.4.2 (2015-03-24)
Features:
- numerous build and CI improvements.
- PMDA/PMNS [de]registration scripts ([9120efc](
  ../../commit/9120efcf0989b2001be3b4e08629ade47815565b))
- Fedora packaging conformance ([1199693](
  https://bugzilla.redhat.com/show_bug.cgi?id=1199693))

Bug fixes:
- potential segfault in `pcp::pmda::cache::lookup` ([33e02b5](
  ../../commit/33e02b53cc7ddd55dcfedb334bc650ad9c7b3c9a))
- unnecessary `stdpmid` includes ([fe22161](
  ../../commit/fe221616fcc9269fc39859177b36339157f4ec07))
- Doxygen documentation corrections.

### 0.4.1 (2014-09-06)
Features:
- high resolution event timestamp support (requires PCP 3.9.10+)

Bug fixes:
- correct functional test dependencies ([#26](../../issues/26))

### 0.4.0 (2014-05-15)
Features:
- `pcp::cache::*` wrappers for `pmdaCache*` routines.
- DSO build examples, with functional tests ([#24](../../issues/24))

Bug fixes:
- DSO entry points work now ([54fbd03](
  ../../commit/54fbd032a5d60fbe8445b3ed55a680034fc5d496))

### 0.3.4 (2014-05-10)
Features:
- [API documentation](http://pcolby.github.io/pcp-pmda-cpp/api/annotated.html)
- default PMID in PMNS exports ([48a26c7](
  ../../commit/c4aba3608f0f699b2df6c3d61e9f7df3aca859a0))

Bug fixes:
- metrics description fallback ([#22](../../issues/22))
- C-style comments for PMNS ([#23](../../issues/23))

### 0.3.3 (2014-02-17)
Features:
- support for skipping `pcp::pmda::instance` definition ([7ffbd69](
  ../../commit/7ffbd69ad7ea8aa182f2e4f1431f92267af83f1b))

### 0.3.2 (2014-02-16)
Features:
- config file support ([#21](../../issues/21))

### 0.3.1 (2014-02-14)
Bug fixes:
- `pcp::exception` copy constructor base construction ([52a392e](
  ../../commit/52a392e66922f53b4de4890256f4fb4f79438759))
- install examples into examples directory ([93ce3ce](
  ../../commit/93ce3cea24704bd74851ff3e92c78540dc12f4ee))

### 0.3.0 (2014-02-09)
Features:
- hidden CLI option support ([950bebe](
  ../../commit/950bebe5dca940fc8ae37bec8147425d6a099dd5))
- more consistent API ([bdb5972](
  ../../commit/bdb5972183cf7b0396ccbe938cb3c68ba3006bd8))
- CMake build system ([#2](../../issues/2))
- CLI option intercept support ([#14](../../issues/14))
- install targets ([#15](../../issues/15))
- duplicate cluster name support ([#19](../../issues/19))
- `pmda::supported_metrics` exposed as `protected` ([a0dc1f1](
  ../../commit/a0dc1f1b39cd6772afedd92242359ed443eed952))

### 0.2.0 (2013-12-22)
Features:
- `PM_TEXT_INDOM` support ([#3](../../issues/3))
- unit tests ([#12](../../issues/12))

Bug fixes:
- `pcp::instance_id_type` is now unsigned ([#11](../../issues/11))
- `pcp::metrics_description` functors may segfault if not called in the correct
  order ([f7da268](../../commit/f7da2685b426410904fae2e9a8f24619685eb0b4))

### 0.1.0 (2013-12-08)
- initial release.
