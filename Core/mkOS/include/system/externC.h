#pragma once

#include "systemConfig.h"

enum{
     sysSyncUsbCdcRxCallback= 0,
};

#ifdef __cplusplus
extern "C"{
#endif
     void *systemSync(int32_t, void *, void *, void *, void *);
#ifdef __cplusplus
}
#endif