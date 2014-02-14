## PMDA++ Changelog

### 0.3.1 (2014-02-14)
Bug fixes:
- `pcp::exception` copy constructor base construction ([52a392e](
  ../../commit/52a392e66922f53b4de4890256f4fb4f79438759))

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
