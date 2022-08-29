# select

## INTRO  

The select function has following parameters.  
```c
int select(int nfds,
	fd_set *restrict readfds, fd_set *restrict writefds, fd_set *restrict errorfds,
struct timeval *restrict timeout)
```
- `nfds`: The descriptors from zero through `nfds - 1` in the descriptor sets shall be examined.  
- `readfds`: The specified set of file descriptors to be checked for being ready to read.  

## Explanation  

- This program is an echo server implemented by `select`. It will listen on `localhost:8888` and wait for the client to connect.  
- Before pass the read set to `select`, it must be initialize by `FD_ZERO` first. And the `FD_SET` is used to add a file descriptor to the read set.  
- At the begin, the read set only contains the server's socket fd, buf when the client connection is established, it will also be added to the read set.  
- The `FD_ISSET` is used to check if the fd is in the set. It is useful after `select` returns.  
- When the server has send the response to the client, it will use `FD_CLR` to remove the fd from the set.  

## Result  

When the server executes, it will block in the `select` until a the client connects.  

``` shell
$ ./build/select
Accept (4): 127.0.0.1:34518
Receive: Hi
```

On the client side, we use the `nc` to establish the connection and send the message in TCP.  

``` shell
$ echo Hi | nc -v localhost 8888
Connection to localhost (127.0.0.1) 8888 port [tcp/*] succeeded!
Hello
```
