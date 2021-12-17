#pragma once

#define BIT_MASK(x)         (1 << ((x) % 8))
#define BIT_SLOT(x)         ((x) / 8)
#define SET_BIT(arr, x)     ((arr)[BIT_SLOT(x)] |= BIT_MASK(x))
#define CLEAR_BIT(arr, x)   ((arr)[BIT_SLOT(x)] &= ~BIT_MASK(x))
#define TEST_BIT(arr, x)    ((arr)[BIT_SLOT(x)] & BIT_MASK(x))
#define TOGGLE_BIT(arr, x)  ((arr)[BIT_SLOT(x)] ^ BIT_MASK(x))
#define BIT_SLOTS(n)        ((n + 7) / 8)