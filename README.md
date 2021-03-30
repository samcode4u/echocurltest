# echocurltest
Golang echo based server with gzip compression enabled and curl c program with gzip post data. 

Run golang echo server. 

```bash
$ go build

$ ./echocurltest
```

Compile c program and run. 

```bash
curltest$ gcc main.c -lz -lcurl

curltest$ ./a.out
```
