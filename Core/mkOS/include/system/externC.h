#pragma once

#include "systemConfig.h"

enum{
     systemSyncXXX= 0,
};

#ifdef __cplusplus
extern "C"{
#endif
     void *systemSync(int32_t, void *, void *, void *, void *);
#ifdef __cplusplus
}
#endif