/**
 * kfifo.h
 * linux内核的kfifo数据结构, 拷贝过来整理一下即可
 */

#ifndef LIBSG_KFIFO_H
#define LIBSG_KFIFO_H

#include <sg/sg.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

typedef struct sg_kfifo_real sg_kfifo_t;

sg_kfifo_t *fifo_alloc(uint64_t size);

bool kfifo_put(sg_kfifo_t *self, const void *buf, uint64_t len);

bool kfifo_get(sg_kfifo_t *self, void *out_buf, uint64_t out_buf_len, uint64_t *real_out_len);/*这个接口的参数可能要根据实际情况调整,尽量不做memcpy*/

uint64_t sg_kfifo_len(sg_kfifo_t *self);

void sg_kfifo_free(sg_kfifo_t *self);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* LIBSG_KFIFO_H */
