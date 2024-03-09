#pragma once

#define max(a,b) (((a) < (b)) ? (b) : (a))
#define min(a,b) (((a) < (b)) ? (a) : (b))
#define clamp(a,b,c) max(min(a,c),b)
#define sign(a) ((a < 0) ? -1 : ((a > 0) ? 1 : 0))
#define abs(a) (a * sign(a))

#define DIV_ROUNDUP(a, b) (((a) + ((b) - 1)) / (b))

#define ALIGN_UP(x, a) ({ \
    typeof(x) value = x; \
    typeof(a) align = a; \
    value = DIV_ROUNDUP(value, align) * align; \
    value; \
})

#define ALIGN_DOWN(x, a) ({ \
    typeof(x) value = x; \
    typeof(a) align = a; \
    value = (value / align) * align; \
    value; \
})

#define SIZEOF_ARRAY(array) (sizeof(array) / sizeof(array[0]))
