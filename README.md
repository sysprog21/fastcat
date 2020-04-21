# fastcat

`fastcat` is an implementation of [cat(1)](http://man7.org/linux/man-pages/man1/cat.1.html)
that uses Linux-specific system calls [splice](http://man7.org/linux/man-pages/man2/splice.2.html)
and [sendfile](http://man7.org/linux/man-pages/man2/sendfile.2.html) to write to stdout.
In situations where stdout is piped, `splice` is used; in all other cases,
`sendfile` is used. The combination of these calls avoids unnecessary copying to userspace.

One unfortunate side effect is that
```shell
$ fastcat FILE >> OUT
```
is not supported as neither `splice` nor `sendfile` support appending to files.

## License
`fastcat` is released under the MIT License. Use of this source code is governed
by a MIT License that can be found in the LICENSE file.
