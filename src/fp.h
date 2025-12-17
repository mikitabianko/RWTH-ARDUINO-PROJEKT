#ifndef FP
#define FP

// Q8.8 fixed point (int16_t)
typedef int16_t fix16;

#define FIX_SHIFT           8
#define FIX_ONE             (1 << FIX_SHIFT)
#define FIX_HALF            (FIX_ONE >> 1)

#define FIX_FROM_INT(x)     ((fix16)(((int32_t)(x)) << FIX_SHIFT))
#define FIX_FROM_FLOAT(x)   ((fix16)((x) * (float)FIX_ONE))
#define FIX_TO_INT(x)       (((x) >> FIX_SHIFT))
#define FIX_TO_FLOAT(x)     ((float)(x) / FIX_ONE)

#define FIX_ADD(a,b)        ((a) + (b))
#define FIX_SUB(a,b)        ((a) - (b))
#define FIX_MUL(a,b)        ((fix16)(((int32_t)(a) * (b)) >> FIX_SHIFT))
#define FIX_DIV(a,b)        ((fix16)(((int32_t)(a) << FIX_SHIFT) / (b)))
#define FIX_ABS(x)          ((x) < 0 ? -(x) : (x))

#endif