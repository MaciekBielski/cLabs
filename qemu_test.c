#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <glib.h>
#include <string.h>

/* testing elements of QEMU code */
typedef struct InetSocketAddress
{
    char *host;
    char *port;
    bool has_to;
    uint16_t to;
    bool has_ipv4;
    bool ipv4;
    bool has_ipv6;
    bool ipv6;
} InetSocketAddress;

InetSocketAddress *inet_parse(const char *str)
{
    InetSocketAddress *addr;
    const char *optstr, *h;
    char host[64];
    char port[33];
    int to;
    int pos;

    addr = g_new0(InetSocketAddress, 1);

    /* parse address */
    if (str[0] == ':') {
        /* no host given */
        host[0] = '\0';
        if (1 != sscanf(str, ":%32[^,]%n", port, &pos)) {
            fprintf(stderr, "error parsing port in address '%s'", str);
            goto fail;
        }
    } else if (str[0] == '[') {
        /* IPv6 addr */
        if (2 != sscanf(str, "[%64[^]]]:%32[^,]%n", host, port, &pos)) {
            fprintf(stderr, "error parsing IPv6 address '%s'", str);
            goto fail;
        }
        addr->ipv6 = addr->has_ipv6 = true;
    } else {
        /* hostname or IPv4 addr */
        if (2 != sscanf(str, "%64[^:]:%32[^,]%n", host, port, &pos)) {
            fprintf(stderr, "error parsing address '%s'", str);
            goto fail;
        }
        if (host[strspn(host, "0123456789.")] == '\0') {
            addr->ipv4 = addr->has_ipv4 = true;
        }
    }

    addr->host = g_strdup(host);
    addr->port = g_strdup(port);

    /* parse options */
    optstr = str + pos;
    h = strstr(optstr, ",to=");
    if (h) {
        h += 4;
        if (sscanf(h, "%d%n", &to, &pos) != 1 ||
            (h[pos] != '\0' && h[pos] != ',')) {
            fprintf(stderr, "error parsing to= argument");
            goto fail;
        }
        addr->has_to = true;
        addr->to = to;
    }
    if (strstr(optstr, ",ipv4")) {
        addr->ipv4 = addr->has_ipv4 = true;
    }
    if (strstr(optstr, ",ipv6")) {
        addr->ipv6 = addr->has_ipv6 = true;
    }
    return addr;

fail:
    return NULL;
}

#define PARSE_SZ 4
#define FMT_PATT(width) "%"#width"u%n"
#define FMT(arg) FMT_PATT(arg)

int main(int argc, char** argv)
{
    //InetSocketAddress *isa = NULL;
    //isa = inet_parse("127.0.0.1:4545");
    char fmt[20] = {0};
    char *data = "456";
    snprintf(fmt, 20,"%%%1cu%%n", *data);
    printf("fmt: %s\n", fmt);
    char *test="12345678901234";
    size_t ret, n;
    sscanf(test, FMT(PARSE_SZ), &ret, &n);
    printf("ret:%u, n: %u\n", ret, n);
    return 0;
}
