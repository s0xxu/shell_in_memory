This is a shell I built that lives on the stack entirely, the reason I do this is because we do not have "fixed address" like a normal program, so if the CPU was to try and execute from a fixed address, it wouldn't be found resulting in a segfault.
I use pipes to handle the data from the shell and xor it before I send it to the socket.
I've used epoll for handling events from file descripts, and fork because when we use execve it will replace our process, I use loops to allocate memory on the stack and free as it goes, to ensure the stack doesn't get overflown and the program crashes.
I've used pipe2 with the flag close on exec (0_CLOEXEC) to help with resource management on the file descriptors, as we fork our processes.
In the actual "loader" program, I've overwritten the shell code with a xor key, so on the disk you'll have to xor with the key if you want to see the original shellcode content.
I allocate memory with mmap with write permissions and MAP_ANONYMOUS to allow us to write into a page, that is not backed by a file and then inside the actual page we apply the function that does XOR on the shellcode, giving us the original contents.
The permissions of the page are then changed from write to executable, because linux has strict WRITE/EXECUTE implementations, after we have changed the permissions we will now treat the memory as a executable function.
On the receiving connection side I use pthread to handle input/output, so that you can write and receive at the same time easily, this just seemed very simple to me and an efficient way to use it with EPOLL event handling on our file descriptors.
When the connection is received, we move into the function that allows us to talk to the connected host, and as we receive and send, our data will have a XOR key applied.
The primary thing I learned from this was EFLAGS behavior while debugging my assembly code in GDB.
example:
cmp rax, rax
sub rsp, 4096
je function
This would fail due to je relying on the EFLAGS value, which just holds mathematical operations.
This is also why you see the assembly code change from xor rax, rax to mov rax, value, because originally I was trying to nullify bytes for practice, but it was getting in the way of my loops and creating bugs that were unnecessary for something that isn't even null-byte terminated.
I believe I improved my GDB debugging from this, and got to understand CPU registers a little deeper.
