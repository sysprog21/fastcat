# fastcat

An implementation of cat that uses `splice` and `sendfile` to write to
stdout. In situations where stdout is piped, `splice` is used; in all other
cases, `sendfile` is used. The combination of these calls avoids unnecessary
copying to userspace.

One unfortunate side effect is that
```shell
$ fastcat FILE >> OUT
```
is not supported as neither `splice` nor `sendfile` support appending to files.
