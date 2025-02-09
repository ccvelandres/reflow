#include <stdint.h>
#include <stddef.h>
#include <sys/lock.h>
#include <sys/reent.h>
#include <sys/stat.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#undef errno
extern int errno;

#include <libopencm3/cm3/cortex.h>
#include <uart.h>

// int _close(int file) { return -1; }
// int _isatty(int file) { return 1; }

// int _lseek(int file, int ptr, int dir) { return 0; }
int _open(const char *name, int flags, int mode) { return -1; }

int _fstat(int file, struct stat *st)
{
    errno = -ENOSYS;
    return -1;
    // st->st_mode = S_IFCHR;
    // return 0;
}
// void *_sbrk (int incr)
// {
//    extern char   end; /* Set by linker.  */
//    static char * heap_end;
//    char *        prev_heap_end;

//    if (heap_end == 0)
//      heap_end = & end;

//    prev_heap_end = heap_end;
//    heap_end += incr;

//    return (void *) prev_heap_end;
// }

// int _read(int file, char *ptr, int len) {
//     return 0;
// }

// _ssize_t write(int fd, const char* ptr, uint32_t len)
// {
//     return uart_write(ptr, len);
// }


int _write (int fd, char *buf,  int cnt)
{
    return uart_write(buf, cnt);
}

int _write_r (struct _reent *ptr, int fd, char *buf,  int cnt)
{
    return _write(fd, buf, cnt);
}

void __malloc_lock(struct _reent *r)
{
    (void)r;
    cm_disable_interrupts();
}

void __malloc_unlock(struct _reent *r)
{
    (void)r;
    cm_enable_interrupts();
}