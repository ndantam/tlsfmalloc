#include <stdio.h>
#include <tlsf/tlsf.h>


int main(void) {
    void *x = tlsf_malloc(2048);
    x = tlsf_realloc(x, 1024);
    tlsf_free(x);
    tlsf_free(x);

    return 0;
}
