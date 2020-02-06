#ifndef _TEMPLATE_H
#define _TEMPLATE_H

typedef void (* Step) (void);

typedef struct _drive {
    Step one;
    Step two;
    Step three;
    Step four;
} Drive;

#endif
