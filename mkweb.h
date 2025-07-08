#ifndef _MKWEB_H_
#define _MKWEB_H_

#include <stdbool.h>
#include <stddef.h>

#define MKWEB_IMPLEMENTATION

struct attribute {
    const char *key;
    const char *value;
};

enum mode {
    /* <tag>...</tag> */
    MKWEB_BODY,
    /* <tag></tag> */
    MKWEB_AUTO_CLOSE,
    /* <tag /> */
    MKWEB_INLINE_CLOSE
};

void mkweb_init(void);

bool begin_page(const char *path);
void end_page(void);

void tag(const char *name, struct attribute *attrs, size_t nattrs,
    enum mode mode);
void close_tag(void);

#ifdef MKWEB_IMPLEMENTATION
#include <stdio.h>
#include <string.h>
#include <unistd.h>

struct element_stack {
    char *name;
    enum mode mode;
    struct element_stack *next;
};

struct mkweb {
    struct element_stack *stack;
    size_t stack_size;
    const char *current_page;
    FILE *file;
};

struct mkweb web = { 0 };
static char mkweb_cwd[BUFSIZ] = { 0 };

static void print_stack(struct element_stack *current, size_t remain);

void mkweb_init(void)
{
    getcwd(mkweb_cwd, BUFSIZ);
}

bool begin_page(const char *path)
{
    size_t cwd_len;
    cwd_len = strlen(mkweb_cwd) + 1; /* include nul */
    strncat(mkweb_cwd, "/", 2);
    strncat(mkweb_cwd + 1, path, BUFSIZ - cwd_len - 1); /* include path sep */
    puts(mkweb_cwd);
    web.file = fopen(mkweb_cwd, "w");
    web.current_page = path;

    memset(mkweb_cwd + cwd_len - 1, 0, BUFSIZ - cwd_len + 1);
    if (web.file == NULL)
        return false;

    return true;
}

void end_page(void)
{
    if (web.file != NULL) {
        if (web.stack != NULL) {
            fprintf(stderr, "Page \"%s\" still has open tags!\n==========",
                web.current_page);
            print_stack(web.stack, web.stack_size);
            fprintf(stderr, "==========\n");
        }
        fclose(web.file);
    }
}

void tag(const char *name, struct attribute *attrs, size_t nattrs, enum mode
    mode)
{
}

void close_tag(void)
{
}

static void print_stack(struct element_stack *current, size_t remain)
{
    if (remain == 0)
        return;
    fprintf(stderr, "%4ld | %s\n", remain, current->name);
    print_stack(current->next, remain - 1);
}
#endif
#endif
