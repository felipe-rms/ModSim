#line 1 "numpy/core/src/umath/loops_arithmetic.dispatch.c.src"

/*
 *****************************************************************************
 **       This file was autogenerated from a template  DO NOT EDIT!!!!      **
 **       Changes should be made to the original source (.src) file         **
 *****************************************************************************
 */

#line 1
/*@targets
 ** $maxopt baseline
 ** sse2 sse41 avx2 avx512f avx512_skx
 ** vsx2
 ** neon
 **/
#define _UMATHMODULE
#define _MULTIARRAYMODULE
#define NPY_NO_DEPRECATED_API NPY_API_VERSION

#include "simd/simd.h"
#include "loops_utils.h"
#include "loops.h"
#include "lowlevel_strided_loops.h"
// Provides the various *_LOOP macros
#include "fast_loop_macros.h"

//###############################################################################
//## Division
//###############################################################################
/********************************************************************************
 ** Defining the SIMD kernels
 *
 * Floor division of signed is based on T. Granlund and P. L. Montgomery
 * "Division by invariant integers using multiplication(see [Figure 6.1]
 * http://citeseerx.ist.psu.edu/viewdoc/summary?doi=10.1.1.1.2556)"
 * For details on TRUNC division see simd/intdiv.h for more clarification
 ***********************************************************************************
 ** Figure 6.1: Signed division by run-time invariant divisor, rounded towards -INF
 ***********************************************************************************
 * For q = FLOOR(a/d), all sword:
 *     sword -dsign = SRL(d, N - 1);
 *     uword -nsign = (n < -dsign);
 *     uword -qsign = EOR(-nsign, -dsign);
 *     q = TRUNC((n - (-dsign ) + (-nsign))/d) - (-qsign);
 ********************************************************************************/

#if NPY_SIMD
#line 44
static NPY_INLINE void
simd_divide_by_scalar_contig_s8(char **args, npy_intp len)
{
    npyv_lanetype_s8 *src   = (npyv_lanetype_s8 *) args[0];
    npyv_lanetype_s8 scalar = *(npyv_lanetype_s8 *) args[1];
    npyv_lanetype_s8 *dst   = (npyv_lanetype_s8 *) args[2];
    const int vstep            = npyv_nlanes_s8;
    const npyv_s8x3 divisor = npyv_divisor_s8(scalar);

    if (scalar == -1) {
        npyv_b8 noverflow = npyv_cvt_b8_s8(npyv_setall_s8(-1));
        npyv_s8 vzero      = npyv_zero_s8();
        for (; len >= vstep; len -= vstep, src += vstep, dst += vstep) {
            npyv_s8 a       = npyv_load_s8(src);
            npyv_b8 gt_min = npyv_cmpgt_s8(a, npyv_setall_s8(NPY_MIN_INT8));
            noverflow          = npyv_and_b8(noverflow, gt_min);
            npyv_s8 neg     = npyv_ifsub_s8(gt_min, vzero, a, vzero);
            npyv_store_s8(dst, neg);
        }

        int raise_err = npyv_tobits_b8(npyv_not_b8(noverflow)) != 0;
        for (; len > 0; --len, ++src, ++dst) {
            npyv_lanetype_s8 a = *src;
            if (a == NPY_MIN_INT8) {
                raise_err = 1;
                *dst  = 0;
            } else {
                *dst = -a;
            }
        }
        if (raise_err) {
            npy_set_floatstatus_divbyzero();
        }
    } else {
        for (; len >= vstep; len -= vstep, src += vstep, dst += vstep) {
            npyv_s8  nsign_d   = npyv_setall_s8(scalar < 0);
            npyv_s8  a         = npyv_load_s8(src);
            npyv_s8  nsign_a   = npyv_cvt_s8_b8(npyv_cmplt_s8(a, nsign_d));
            nsign_a               = npyv_and_s8(nsign_a, npyv_setall_s8(1));
            npyv_s8  diff_sign = npyv_sub_s8(nsign_a, nsign_d);
            npyv_s8  to_ninf   = npyv_xor_s8(nsign_a, nsign_d);
            npyv_s8  trunc     = npyv_divc_s8(npyv_add_s8(a, diff_sign), divisor);
            npyv_s8  floor     = npyv_sub_s8(trunc, to_ninf);
            npyv_store_s8(dst, floor);
        }

        for (; len > 0; --len, ++src, ++dst) {
            const npyv_lanetype_s8 a = *src;
            npyv_lanetype_s8 r = a / scalar;
            // Negative quotients needs to be rounded down
            if (((a > 0) != (scalar > 0)) && ((r * scalar) != a)) {
                r--;
            }
            *dst = r;
        }
    }
    npyv_cleanup();
}

#line 44
static NPY_INLINE void
simd_divide_by_scalar_contig_s16(char **args, npy_intp len)
{
    npyv_lanetype_s16 *src   = (npyv_lanetype_s16 *) args[0];
    npyv_lanetype_s16 scalar = *(npyv_lanetype_s16 *) args[1];
    npyv_lanetype_s16 *dst   = (npyv_lanetype_s16 *) args[2];
    const int vstep            = npyv_nlanes_s16;
    const npyv_s16x3 divisor = npyv_divisor_s16(scalar);

    if (scalar == -1) {
        npyv_b16 noverflow = npyv_cvt_b16_s16(npyv_setall_s16(-1));
        npyv_s16 vzero      = npyv_zero_s16();
        for (; len >= vstep; len -= vstep, src += vstep, dst += vstep) {
            npyv_s16 a       = npyv_load_s16(src);
            npyv_b16 gt_min = npyv_cmpgt_s16(a, npyv_setall_s16(NPY_MIN_INT16));
            noverflow          = npyv_and_b16(noverflow, gt_min);
            npyv_s16 neg     = npyv_ifsub_s16(gt_min, vzero, a, vzero);
            npyv_store_s16(dst, neg);
        }

        int raise_err = npyv_tobits_b16(npyv_not_b16(noverflow)) != 0;
        for (; len > 0; --len, ++src, ++dst) {
            npyv_lanetype_s16 a = *src;
            if (a == NPY_MIN_INT16) {
                raise_err = 1;
                *dst  = 0;
            } else {
                *dst = -a;
            }
        }
        if (raise_err) {
            npy_set_floatstatus_divbyzero();
        }
    } else {
        for (; len >= vstep; len -= vstep, src += vstep, dst += vstep) {
            npyv_s16  nsign_d   = npyv_setall_s16(scalar < 0);
            npyv_s16  a         = npyv_load_s16(src);
            npyv_s16  nsign_a   = npyv_cvt_s16_b16(npyv_cmplt_s16(a, nsign_d));
            nsign_a               = npyv_and_s16(nsign_a, npyv_setall_s16(1));
            npyv_s16  diff_sign = npyv_sub_s16(nsign_a, nsign_d);
            npyv_s16  to_ninf   = npyv_xor_s16(nsign_a, nsign_d);
            npyv_s16  trunc     = npyv_divc_s16(npyv_add_s16(a, diff_sign), divisor);
            npyv_s16  floor     = npyv_sub_s16(trunc, to_ninf);
            npyv_store_s16(dst, floor);
        }

        for (; len > 0; --len, ++src, ++dst) {
            const npyv_lanetype_s16 a = *src;
            npyv_lanetype_s16 r = a / scalar;
            // Negative quotients needs to be rounded down
            if (((a > 0) != (scalar > 0)) && ((r * scalar) != a)) {
                r--;
            }
            *dst = r;
        }
    }
    npyv_cleanup();
}

#line 44
static NPY_INLINE void
simd_divide_by_scalar_contig_s32(char **args, npy_intp len)
{
    npyv_lanetype_s32 *src   = (npyv_lanetype_s32 *) args[0];
    npyv_lanetype_s32 scalar = *(npyv_lanetype_s32 *) args[1];
    npyv_lanetype_s32 *dst   = (npyv_lanetype_s32 *) args[2];
    const int vstep            = npyv_nlanes_s32;
    const npyv_s32x3 divisor = npyv_divisor_s32(scalar);

    if (scalar == -1) {
        npyv_b32 noverflow = npyv_cvt_b32_s32(npyv_setall_s32(-1));
        npyv_s32 vzero      = npyv_zero_s32();
        for (; len >= vstep; len -= vstep, src += vstep, dst += vstep) {
            npyv_s32 a       = npyv_load_s32(src);
            npyv_b32 gt_min = npyv_cmpgt_s32(a, npyv_setall_s32(NPY_MIN_INT32));
            noverflow          = npyv_and_b32(noverflow, gt_min);
            npyv_s32 neg     = npyv_ifsub_s32(gt_min, vzero, a, vzero);
            npyv_store_s32(dst, neg);
        }

        int raise_err = npyv_tobits_b32(npyv_not_b32(noverflow)) != 0;
        for (; len > 0; --len, ++src, ++dst) {
            npyv_lanetype_s32 a = *src;
            if (a == NPY_MIN_INT32) {
                raise_err = 1;
                *dst  = 0;
            } else {
                *dst = -a;
            }
        }
        if (raise_err) {
            npy_set_floatstatus_divbyzero();
        }
    } else {
        for (; len >= vstep; len -= vstep, src += vstep, dst += vstep) {
            npyv_s32  nsign_d   = npyv_setall_s32(scalar < 0);
            npyv_s32  a         = npyv_load_s32(src);
            npyv_s32  nsign_a   = npyv_cvt_s32_b32(npyv_cmplt_s32(a, nsign_d));
            nsign_a               = npyv_and_s32(nsign_a, npyv_setall_s32(1));
            npyv_s32  diff_sign = npyv_sub_s32(nsign_a, nsign_d);
            npyv_s32  to_ninf   = npyv_xor_s32(nsign_a, nsign_d);
            npyv_s32  trunc     = npyv_divc_s32(npyv_add_s32(a, diff_sign), divisor);
            npyv_s32  floor     = npyv_sub_s32(trunc, to_ninf);
            npyv_store_s32(dst, floor);
        }

        for (; len > 0; --len, ++src, ++dst) {
            const npyv_lanetype_s32 a = *src;
            npyv_lanetype_s32 r = a / scalar;
            // Negative quotients needs to be rounded down
            if (((a > 0) != (scalar > 0)) && ((r * scalar) != a)) {
                r--;
            }
            *dst = r;
        }
    }
    npyv_cleanup();
}

#line 44
static NPY_INLINE void
simd_divide_by_scalar_contig_s64(char **args, npy_intp len)
{
    npyv_lanetype_s64 *src   = (npyv_lanetype_s64 *) args[0];
    npyv_lanetype_s64 scalar = *(npyv_lanetype_s64 *) args[1];
    npyv_lanetype_s64 *dst   = (npyv_lanetype_s64 *) args[2];
    const int vstep            = npyv_nlanes_s64;
    const npyv_s64x3 divisor = npyv_divisor_s64(scalar);

    if (scalar == -1) {
        npyv_b64 noverflow = npyv_cvt_b64_s64(npyv_setall_s64(-1));
        npyv_s64 vzero      = npyv_zero_s64();
        for (; len >= vstep; len -= vstep, src += vstep, dst += vstep) {
            npyv_s64 a       = npyv_load_s64(src);
            npyv_b64 gt_min = npyv_cmpgt_s64(a, npyv_setall_s64(NPY_MIN_INT64));
            noverflow          = npyv_and_b64(noverflow, gt_min);
            npyv_s64 neg     = npyv_ifsub_s64(gt_min, vzero, a, vzero);
            npyv_store_s64(dst, neg);
        }

        int raise_err = npyv_tobits_b64(npyv_not_b64(noverflow)) != 0;
        for (; len > 0; --len, ++src, ++dst) {
            npyv_lanetype_s64 a = *src;
            if (a == NPY_MIN_INT64) {
                raise_err = 1;
                *dst  = 0;
            } else {
                *dst = -a;
            }
        }
        if (raise_err) {
            npy_set_floatstatus_divbyzero();
        }
    } else {
        for (; len >= vstep; len -= vstep, src += vstep, dst += vstep) {
            npyv_s64  nsign_d   = npyv_setall_s64(scalar < 0);
            npyv_s64  a         = npyv_load_s64(src);
            npyv_s64  nsign_a   = npyv_cvt_s64_b64(npyv_cmplt_s64(a, nsign_d));
            nsign_a               = npyv_and_s64(nsign_a, npyv_setall_s64(1));
            npyv_s64  diff_sign = npyv_sub_s64(nsign_a, nsign_d);
            npyv_s64  to_ninf   = npyv_xor_s64(nsign_a, nsign_d);
            npyv_s64  trunc     = npyv_divc_s64(npyv_add_s64(a, diff_sign), divisor);
            npyv_s64  floor     = npyv_sub_s64(trunc, to_ninf);
            npyv_store_s64(dst, floor);
        }

        for (; len > 0; --len, ++src, ++dst) {
            const npyv_lanetype_s64 a = *src;
            npyv_lanetype_s64 r = a / scalar;
            // Negative quotients needs to be rounded down
            if (((a > 0) != (scalar > 0)) && ((r * scalar) != a)) {
                r--;
            }
            *dst = r;
        }
    }
    npyv_cleanup();
}


#line 109
static NPY_INLINE void
simd_divide_by_scalar_contig_u8(char **args, npy_intp len)
{
    npyv_lanetype_u8 *src   = (npyv_lanetype_u8 *) args[0];
    npyv_lanetype_u8 scalar = *(npyv_lanetype_u8 *) args[1];
    npyv_lanetype_u8 *dst   = (npyv_lanetype_u8 *) args[2];
    const int vstep            = npyv_nlanes_u8;
    const npyv_u8x3 divisor = npyv_divisor_u8(scalar);

    for (; len >= vstep; len -= vstep, src += vstep, dst += vstep) {
        npyv_u8 a = npyv_load_u8(src);
        npyv_u8 c = npyv_divc_u8(a, divisor);
        npyv_store_u8(dst, c);
    }

    for (; len > 0; --len, ++src, ++dst) {
        const npyv_lanetype_u8 a = *src;
        *dst = a / scalar;
    }
    npyv_cleanup();
}

#line 109
static NPY_INLINE void
simd_divide_by_scalar_contig_u16(char **args, npy_intp len)
{
    npyv_lanetype_u16 *src   = (npyv_lanetype_u16 *) args[0];
    npyv_lanetype_u16 scalar = *(npyv_lanetype_u16 *) args[1];
    npyv_lanetype_u16 *dst   = (npyv_lanetype_u16 *) args[2];
    const int vstep            = npyv_nlanes_u16;
    const npyv_u16x3 divisor = npyv_divisor_u16(scalar);

    for (; len >= vstep; len -= vstep, src += vstep, dst += vstep) {
        npyv_u16 a = npyv_load_u16(src);
        npyv_u16 c = npyv_divc_u16(a, divisor);
        npyv_store_u16(dst, c);
    }

    for (; len > 0; --len, ++src, ++dst) {
        const npyv_lanetype_u16 a = *src;
        *dst = a / scalar;
    }
    npyv_cleanup();
}

#line 109
static NPY_INLINE void
simd_divide_by_scalar_contig_u32(char **args, npy_intp len)
{
    npyv_lanetype_u32 *src   = (npyv_lanetype_u32 *) args[0];
    npyv_lanetype_u32 scalar = *(npyv_lanetype_u32 *) args[1];
    npyv_lanetype_u32 *dst   = (npyv_lanetype_u32 *) args[2];
    const int vstep            = npyv_nlanes_u32;
    const npyv_u32x3 divisor = npyv_divisor_u32(scalar);

    for (; len >= vstep; len -= vstep, src += vstep, dst += vstep) {
        npyv_u32 a = npyv_load_u32(src);
        npyv_u32 c = npyv_divc_u32(a, divisor);
        npyv_store_u32(dst, c);
    }

    for (; len > 0; --len, ++src, ++dst) {
        const npyv_lanetype_u32 a = *src;
        *dst = a / scalar;
    }
    npyv_cleanup();
}

#line 109
static NPY_INLINE void
simd_divide_by_scalar_contig_u64(char **args, npy_intp len)
{
    npyv_lanetype_u64 *src   = (npyv_lanetype_u64 *) args[0];
    npyv_lanetype_u64 scalar = *(npyv_lanetype_u64 *) args[1];
    npyv_lanetype_u64 *dst   = (npyv_lanetype_u64 *) args[2];
    const int vstep            = npyv_nlanes_u64;
    const npyv_u64x3 divisor = npyv_divisor_u64(scalar);

    for (; len >= vstep; len -= vstep, src += vstep, dst += vstep) {
        npyv_u64 a = npyv_load_u64(src);
        npyv_u64 c = npyv_divc_u64(a, divisor);
        npyv_store_u64(dst, c);
    }

    for (; len > 0; --len, ++src, ++dst) {
        const npyv_lanetype_u64 a = *src;
        *dst = a / scalar;
    }
    npyv_cleanup();
}

#endif

/********************************************************************************
 ** Defining ufunc inner functions
 ********************************************************************************/

#line 142
#undef TO_SIMD_SFX
#if 0
#line 147
#elif NPY_BITSOF_BYTE == 8
    #define TO_SIMD_SFX(X) X##_s8

#line 147
#elif NPY_BITSOF_BYTE == 16
    #define TO_SIMD_SFX(X) X##_s16

#line 147
#elif NPY_BITSOF_BYTE == 32
    #define TO_SIMD_SFX(X) X##_s32

#line 147
#elif NPY_BITSOF_BYTE == 64
    #define TO_SIMD_SFX(X) X##_s64

#endif

#if NPY_BITSOF_BYTE == 64 && !defined(NPY_HAVE_VSX4) && (defined(NPY_HAVE_VSX) || defined(NPY_HAVE_NEON))
    #undef TO_SIMD_SFX
#endif

NPY_FINLINE npy_byte floor_div_BYTE(const npy_byte n, const npy_byte d)
{
    /*
     * FIXME: On x86 at least, dividing the smallest representable integer
     * by -1 causes a SIFGPE (division overflow). We treat this case here
     * (to avoid a SIGFPE crash at python level), but a good solution would
     * be to treat integer division problems separately from FPU exceptions
     * (i.e. a different approach than npy_set_floatstatus_divbyzero()).
     */
    if (NPY_UNLIKELY(d == 0 || (n == NPY_MIN_BYTE && d == -1))) {
        npy_set_floatstatus_divbyzero();
        return 0;
    }
    npy_byte r = n / d;
    // Negative quotients needs to be rounded down
    if (((n > 0) != (d > 0)) && ((r * d) != n)) {
        r--;
    }
    return r;
}

NPY_NO_EXPORT void NPY_CPU_DISPATCH_CURFX(BYTE_divide)
(char **args, npy_intp const *dimensions, npy_intp const *steps, void *NPY_UNUSED(func))
{
    if (IS_BINARY_REDUCE) {
        BINARY_REDUCE_LOOP(npy_byte) {
            io1 = floor_div_BYTE(io1, *(npy_byte*)ip2);
        }
        *((npy_byte *)iop1) = io1;
    }
#if NPY_SIMD && defined(TO_SIMD_SFX)
    // for contiguous block of memory, divisor is a scalar and not 0
    else if (IS_BLOCKABLE_BINARY_SCALAR2(sizeof(npy_byte), NPY_SIMD_WIDTH) &&
             (*(npy_byte *)args[1]) != 0) {
        TO_SIMD_SFX(simd_divide_by_scalar_contig)(args, dimensions[0]);
    }
#endif
    else {
        BINARY_LOOP {
            *((npy_byte *)op1) = floor_div_BYTE(*(npy_byte*)ip1, *(npy_byte*)ip2);
        }
    }
}

#line 142
#undef TO_SIMD_SFX
#if 0
#line 147
#elif NPY_BITSOF_SHORT == 8
    #define TO_SIMD_SFX(X) X##_s8

#line 147
#elif NPY_BITSOF_SHORT == 16
    #define TO_SIMD_SFX(X) X##_s16

#line 147
#elif NPY_BITSOF_SHORT == 32
    #define TO_SIMD_SFX(X) X##_s32

#line 147
#elif NPY_BITSOF_SHORT == 64
    #define TO_SIMD_SFX(X) X##_s64

#endif

#if NPY_BITSOF_SHORT == 64 && !defined(NPY_HAVE_VSX4) && (defined(NPY_HAVE_VSX) || defined(NPY_HAVE_NEON))
    #undef TO_SIMD_SFX
#endif

NPY_FINLINE npy_short floor_div_SHORT(const npy_short n, const npy_short d)
{
    /*
     * FIXME: On x86 at least, dividing the smallest representable integer
     * by -1 causes a SIFGPE (division overflow). We treat this case here
     * (to avoid a SIGFPE crash at python level), but a good solution would
     * be to treat integer division problems separately from FPU exceptions
     * (i.e. a different approach than npy_set_floatstatus_divbyzero()).
     */
    if (NPY_UNLIKELY(d == 0 || (n == NPY_MIN_SHORT && d == -1))) {
        npy_set_floatstatus_divbyzero();
        return 0;
    }
    npy_short r = n / d;
    // Negative quotients needs to be rounded down
    if (((n > 0) != (d > 0)) && ((r * d) != n)) {
        r--;
    }
    return r;
}

NPY_NO_EXPORT void NPY_CPU_DISPATCH_CURFX(SHORT_divide)
(char **args, npy_intp const *dimensions, npy_intp const *steps, void *NPY_UNUSED(func))
{
    if (IS_BINARY_REDUCE) {
        BINARY_REDUCE_LOOP(npy_short) {
            io1 = floor_div_SHORT(io1, *(npy_short*)ip2);
        }
        *((npy_short *)iop1) = io1;
    }
#if NPY_SIMD && defined(TO_SIMD_SFX)
    // for contiguous block of memory, divisor is a scalar and not 0
    else if (IS_BLOCKABLE_BINARY_SCALAR2(sizeof(npy_short), NPY_SIMD_WIDTH) &&
             (*(npy_short *)args[1]) != 0) {
        TO_SIMD_SFX(simd_divide_by_scalar_contig)(args, dimensions[0]);
    }
#endif
    else {
        BINARY_LOOP {
            *((npy_short *)op1) = floor_div_SHORT(*(npy_short*)ip1, *(npy_short*)ip2);
        }
    }
}

#line 142
#undef TO_SIMD_SFX
#if 0
#line 147
#elif NPY_BITSOF_INT == 8
    #define TO_SIMD_SFX(X) X##_s8

#line 147
#elif NPY_BITSOF_INT == 16
    #define TO_SIMD_SFX(X) X##_s16

#line 147
#elif NPY_BITSOF_INT == 32
    #define TO_SIMD_SFX(X) X##_s32

#line 147
#elif NPY_BITSOF_INT == 64
    #define TO_SIMD_SFX(X) X##_s64

#endif

#if NPY_BITSOF_INT == 64 && !defined(NPY_HAVE_VSX4) && (defined(NPY_HAVE_VSX) || defined(NPY_HAVE_NEON))
    #undef TO_SIMD_SFX
#endif

NPY_FINLINE npy_int floor_div_INT(const npy_int n, const npy_int d)
{
    /*
     * FIXME: On x86 at least, dividing the smallest representable integer
     * by -1 causes a SIFGPE (division overflow). We treat this case here
     * (to avoid a SIGFPE crash at python level), but a good solution would
     * be to treat integer division problems separately from FPU exceptions
     * (i.e. a different approach than npy_set_floatstatus_divbyzero()).
     */
    if (NPY_UNLIKELY(d == 0 || (n == NPY_MIN_INT && d == -1))) {
        npy_set_floatstatus_divbyzero();
        return 0;
    }
    npy_int r = n / d;
    // Negative quotients needs to be rounded down
    if (((n > 0) != (d > 0)) && ((r * d) != n)) {
        r--;
    }
    return r;
}

NPY_NO_EXPORT void NPY_CPU_DISPATCH_CURFX(INT_divide)
(char **args, npy_intp const *dimensions, npy_intp const *steps, void *NPY_UNUSED(func))
{
    if (IS_BINARY_REDUCE) {
        BINARY_REDUCE_LOOP(npy_int) {
            io1 = floor_div_INT(io1, *(npy_int*)ip2);
        }
        *((npy_int *)iop1) = io1;
    }
#if NPY_SIMD && defined(TO_SIMD_SFX)
    // for contiguous block of memory, divisor is a scalar and not 0
    else if (IS_BLOCKABLE_BINARY_SCALAR2(sizeof(npy_int), NPY_SIMD_WIDTH) &&
             (*(npy_int *)args[1]) != 0) {
        TO_SIMD_SFX(simd_divide_by_scalar_contig)(args, dimensions[0]);
    }
#endif
    else {
        BINARY_LOOP {
            *((npy_int *)op1) = floor_div_INT(*(npy_int*)ip1, *(npy_int*)ip2);
        }
    }
}

#line 142
#undef TO_SIMD_SFX
#if 0
#line 147
#elif NPY_BITSOF_LONG == 8
    #define TO_SIMD_SFX(X) X##_s8

#line 147
#elif NPY_BITSOF_LONG == 16
    #define TO_SIMD_SFX(X) X##_s16

#line 147
#elif NPY_BITSOF_LONG == 32
    #define TO_SIMD_SFX(X) X##_s32

#line 147
#elif NPY_BITSOF_LONG == 64
    #define TO_SIMD_SFX(X) X##_s64

#endif

#if NPY_BITSOF_LONG == 64 && !defined(NPY_HAVE_VSX4) && (defined(NPY_HAVE_VSX) || defined(NPY_HAVE_NEON))
    #undef TO_SIMD_SFX
#endif

NPY_FINLINE npy_long floor_div_LONG(const npy_long n, const npy_long d)
{
    /*
     * FIXME: On x86 at least, dividing the smallest representable integer
     * by -1 causes a SIFGPE (division overflow). We treat this case here
     * (to avoid a SIGFPE crash at python level), but a good solution would
     * be to treat integer division problems separately from FPU exceptions
     * (i.e. a different approach than npy_set_floatstatus_divbyzero()).
     */
    if (NPY_UNLIKELY(d == 0 || (n == NPY_MIN_LONG && d == -1))) {
        npy_set_floatstatus_divbyzero();
        return 0;
    }
    npy_long r = n / d;
    // Negative quotients needs to be rounded down
    if (((n > 0) != (d > 0)) && ((r * d) != n)) {
        r--;
    }
    return r;
}

NPY_NO_EXPORT void NPY_CPU_DISPATCH_CURFX(LONG_divide)
(char **args, npy_intp const *dimensions, npy_intp const *steps, void *NPY_UNUSED(func))
{
    if (IS_BINARY_REDUCE) {
        BINARY_REDUCE_LOOP(npy_long) {
            io1 = floor_div_LONG(io1, *(npy_long*)ip2);
        }
        *((npy_long *)iop1) = io1;
    }
#if NPY_SIMD && defined(TO_SIMD_SFX)
    // for contiguous block of memory, divisor is a scalar and not 0
    else if (IS_BLOCKABLE_BINARY_SCALAR2(sizeof(npy_long), NPY_SIMD_WIDTH) &&
             (*(npy_long *)args[1]) != 0) {
        TO_SIMD_SFX(simd_divide_by_scalar_contig)(args, dimensions[0]);
    }
#endif
    else {
        BINARY_LOOP {
            *((npy_long *)op1) = floor_div_LONG(*(npy_long*)ip1, *(npy_long*)ip2);
        }
    }
}

#line 142
#undef TO_SIMD_SFX
#if 0
#line 147
#elif NPY_BITSOF_LONGLONG == 8
    #define TO_SIMD_SFX(X) X##_s8

#line 147
#elif NPY_BITSOF_LONGLONG == 16
    #define TO_SIMD_SFX(X) X##_s16

#line 147
#elif NPY_BITSOF_LONGLONG == 32
    #define TO_SIMD_SFX(X) X##_s32

#line 147
#elif NPY_BITSOF_LONGLONG == 64
    #define TO_SIMD_SFX(X) X##_s64

#endif

#if NPY_BITSOF_LONGLONG == 64 && !defined(NPY_HAVE_VSX4) && (defined(NPY_HAVE_VSX) || defined(NPY_HAVE_NEON))
    #undef TO_SIMD_SFX
#endif

NPY_FINLINE npy_longlong floor_div_LONGLONG(const npy_longlong n, const npy_longlong d)
{
    /*
     * FIXME: On x86 at least, dividing the smallest representable integer
     * by -1 causes a SIFGPE (division overflow). We treat this case here
     * (to avoid a SIGFPE crash at python level), but a good solution would
     * be to treat integer division problems separately from FPU exceptions
     * (i.e. a different approach than npy_set_floatstatus_divbyzero()).
     */
    if (NPY_UNLIKELY(d == 0 || (n == NPY_MIN_LONGLONG && d == -1))) {
        npy_set_floatstatus_divbyzero();
        return 0;
    }
    npy_longlong r = n / d;
    // Negative quotients needs to be rounded down
    if (((n > 0) != (d > 0)) && ((r * d) != n)) {
        r--;
    }
    return r;
}

NPY_NO_EXPORT void NPY_CPU_DISPATCH_CURFX(LONGLONG_divide)
(char **args, npy_intp const *dimensions, npy_intp const *steps, void *NPY_UNUSED(func))
{
    if (IS_BINARY_REDUCE) {
        BINARY_REDUCE_LOOP(npy_longlong) {
            io1 = floor_div_LONGLONG(io1, *(npy_longlong*)ip2);
        }
        *((npy_longlong *)iop1) = io1;
    }
#if NPY_SIMD && defined(TO_SIMD_SFX)
    // for contiguous block of memory, divisor is a scalar and not 0
    else if (IS_BLOCKABLE_BINARY_SCALAR2(sizeof(npy_longlong), NPY_SIMD_WIDTH) &&
             (*(npy_longlong *)args[1]) != 0) {
        TO_SIMD_SFX(simd_divide_by_scalar_contig)(args, dimensions[0]);
    }
#endif
    else {
        BINARY_LOOP {
            *((npy_longlong *)op1) = floor_div_LONGLONG(*(npy_longlong*)ip1, *(npy_longlong*)ip2);
        }
    }
}


#line 207
#undef TO_SIMD_SFX
#if 0
#line 212
#elif NPY_BITSOF_BYTE == 8
    #define TO_SIMD_SFX(X) X##_u8

#line 212
#elif NPY_BITSOF_BYTE == 16
    #define TO_SIMD_SFX(X) X##_u16

#line 212
#elif NPY_BITSOF_BYTE == 32
    #define TO_SIMD_SFX(X) X##_u32

#line 212
#elif NPY_BITSOF_BYTE == 64
    #define TO_SIMD_SFX(X) X##_u64

#endif
/*
 * For 64-bit division on Armv7, Aarch64, and IBM/Power, NPYV fall-backs to the scalar division
 * because emulating multiply-high on these architectures is going to be expensive comparing
 * to the native scalar dividers.
 * Therefore it's better to disable NPYV in this special case to avoid any unnecessary shuffles.
 * Power10(VSX4) is an exception here since it has native support for integer vector division,
 * note neither infrastructure nor NPYV has supported VSX4 yet.
 */
#if NPY_BITSOF_BYTE == 64 && !defined(NPY_HAVE_VSX4) && (defined(NPY_HAVE_VSX) || defined(NPY_HAVE_NEON))
    #undef TO_SIMD_SFX
#endif
NPY_NO_EXPORT void NPY_CPU_DISPATCH_CURFX(UBYTE_divide)
(char **args, npy_intp const *dimensions, npy_intp const *steps, void *NPY_UNUSED(func))
{
    if (IS_BINARY_REDUCE) {
        BINARY_REDUCE_LOOP(npy_ubyte) {
            const npy_ubyte d = *(npy_ubyte *)ip2;
            if (NPY_UNLIKELY(d == 0)) {
                npy_set_floatstatus_divbyzero();
                io1 = 0;
            } else {
                io1 /= d;
            }
        }
        *((npy_ubyte *)iop1) = io1;
    }
#if NPY_SIMD && defined(TO_SIMD_SFX)
    // for contiguous block of memory, divisor is a scalar and not 0
    else if (IS_BLOCKABLE_BINARY_SCALAR2(sizeof(npy_ubyte), NPY_SIMD_WIDTH) &&
             (*(npy_ubyte *)args[1]) != 0) {
        TO_SIMD_SFX(simd_divide_by_scalar_contig)(args, dimensions[0]);
    }
#endif
    else {
        BINARY_LOOP {
            const npy_ubyte in1 = *(npy_ubyte *)ip1;
            const npy_ubyte in2 = *(npy_ubyte *)ip2;
            if (NPY_UNLIKELY(in2 == 0)) {
                npy_set_floatstatus_divbyzero();
                *((npy_ubyte *)op1) = 0;
            } else{
                *((npy_ubyte *)op1) = in1 / in2;
            }
        }
    }
}

#line 207
#undef TO_SIMD_SFX
#if 0
#line 212
#elif NPY_BITSOF_SHORT == 8
    #define TO_SIMD_SFX(X) X##_u8

#line 212
#elif NPY_BITSOF_SHORT == 16
    #define TO_SIMD_SFX(X) X##_u16

#line 212
#elif NPY_BITSOF_SHORT == 32
    #define TO_SIMD_SFX(X) X##_u32

#line 212
#elif NPY_BITSOF_SHORT == 64
    #define TO_SIMD_SFX(X) X##_u64

#endif
/*
 * For 64-bit division on Armv7, Aarch64, and IBM/Power, NPYV fall-backs to the scalar division
 * because emulating multiply-high on these architectures is going to be expensive comparing
 * to the native scalar dividers.
 * Therefore it's better to disable NPYV in this special case to avoid any unnecessary shuffles.
 * Power10(VSX4) is an exception here since it has native support for integer vector division,
 * note neither infrastructure nor NPYV has supported VSX4 yet.
 */
#if NPY_BITSOF_SHORT == 64 && !defined(NPY_HAVE_VSX4) && (defined(NPY_HAVE_VSX) || defined(NPY_HAVE_NEON))
    #undef TO_SIMD_SFX
#endif
NPY_NO_EXPORT void NPY_CPU_DISPATCH_CURFX(USHORT_divide)
(char **args, npy_intp const *dimensions, npy_intp const *steps, void *NPY_UNUSED(func))
{
    if (IS_BINARY_REDUCE) {
        BINARY_REDUCE_LOOP(npy_ushort) {
            const npy_ushort d = *(npy_ushort *)ip2;
            if (NPY_UNLIKELY(d == 0)) {
                npy_set_floatstatus_divbyzero();
                io1 = 0;
            } else {
                io1 /= d;
            }
        }
        *((npy_ushort *)iop1) = io1;
    }
#if NPY_SIMD && defined(TO_SIMD_SFX)
    // for contiguous block of memory, divisor is a scalar and not 0
    else if (IS_BLOCKABLE_BINARY_SCALAR2(sizeof(npy_ushort), NPY_SIMD_WIDTH) &&
             (*(npy_ushort *)args[1]) != 0) {
        TO_SIMD_SFX(simd_divide_by_scalar_contig)(args, dimensions[0]);
    }
#endif
    else {
        BINARY_LOOP {
            const npy_ushort in1 = *(npy_ushort *)ip1;
            const npy_ushort in2 = *(npy_ushort *)ip2;
            if (NPY_UNLIKELY(in2 == 0)) {
                npy_set_floatstatus_divbyzero();
                *((npy_ushort *)op1) = 0;
            } else{
                *((npy_ushort *)op1) = in1 / in2;
            }
        }
    }
}

#line 207
#undef TO_SIMD_SFX
#if 0
#line 212
#elif NPY_BITSOF_INT == 8
    #define TO_SIMD_SFX(X) X##_u8

#line 212
#elif NPY_BITSOF_INT == 16
    #define TO_SIMD_SFX(X) X##_u16

#line 212
#elif NPY_BITSOF_INT == 32
    #define TO_SIMD_SFX(X) X##_u32

#line 212
#elif NPY_BITSOF_INT == 64
    #define TO_SIMD_SFX(X) X##_u64

#endif
/*
 * For 64-bit division on Armv7, Aarch64, and IBM/Power, NPYV fall-backs to the scalar division
 * because emulating multiply-high on these architectures is going to be expensive comparing
 * to the native scalar dividers.
 * Therefore it's better to disable NPYV in this special case to avoid any unnecessary shuffles.
 * Power10(VSX4) is an exception here since it has native support for integer vector division,
 * note neither infrastructure nor NPYV has supported VSX4 yet.
 */
#if NPY_BITSOF_INT == 64 && !defined(NPY_HAVE_VSX4) && (defined(NPY_HAVE_VSX) || defined(NPY_HAVE_NEON))
    #undef TO_SIMD_SFX
#endif
NPY_NO_EXPORT void NPY_CPU_DISPATCH_CURFX(UINT_divide)
(char **args, npy_intp const *dimensions, npy_intp const *steps, void *NPY_UNUSED(func))
{
    if (IS_BINARY_REDUCE) {
        BINARY_REDUCE_LOOP(npy_uint) {
            const npy_uint d = *(npy_uint *)ip2;
            if (NPY_UNLIKELY(d == 0)) {
                npy_set_floatstatus_divbyzero();
                io1 = 0;
            } else {
                io1 /= d;
            }
        }
        *((npy_uint *)iop1) = io1;
    }
#if NPY_SIMD && defined(TO_SIMD_SFX)
    // for contiguous block of memory, divisor is a scalar and not 0
    else if (IS_BLOCKABLE_BINARY_SCALAR2(sizeof(npy_uint), NPY_SIMD_WIDTH) &&
             (*(npy_uint *)args[1]) != 0) {
        TO_SIMD_SFX(simd_divide_by_scalar_contig)(args, dimensions[0]);
    }
#endif
    else {
        BINARY_LOOP {
            const npy_uint in1 = *(npy_uint *)ip1;
            const npy_uint in2 = *(npy_uint *)ip2;
            if (NPY_UNLIKELY(in2 == 0)) {
                npy_set_floatstatus_divbyzero();
                *((npy_uint *)op1) = 0;
            } else{
                *((npy_uint *)op1) = in1 / in2;
            }
        }
    }
}

#line 207
#undef TO_SIMD_SFX
#if 0
#line 212
#elif NPY_BITSOF_LONG == 8
    #define TO_SIMD_SFX(X) X##_u8

#line 212
#elif NPY_BITSOF_LONG == 16
    #define TO_SIMD_SFX(X) X##_u16

#line 212
#elif NPY_BITSOF_LONG == 32
    #define TO_SIMD_SFX(X) X##_u32

#line 212
#elif NPY_BITSOF_LONG == 64
    #define TO_SIMD_SFX(X) X##_u64

#endif
/*
 * For 64-bit division on Armv7, Aarch64, and IBM/Power, NPYV fall-backs to the scalar division
 * because emulating multiply-high on these architectures is going to be expensive comparing
 * to the native scalar dividers.
 * Therefore it's better to disable NPYV in this special case to avoid any unnecessary shuffles.
 * Power10(VSX4) is an exception here since it has native support for integer vector division,
 * note neither infrastructure nor NPYV has supported VSX4 yet.
 */
#if NPY_BITSOF_LONG == 64 && !defined(NPY_HAVE_VSX4) && (defined(NPY_HAVE_VSX) || defined(NPY_HAVE_NEON))
    #undef TO_SIMD_SFX
#endif
NPY_NO_EXPORT void NPY_CPU_DISPATCH_CURFX(ULONG_divide)
(char **args, npy_intp const *dimensions, npy_intp const *steps, void *NPY_UNUSED(func))
{
    if (IS_BINARY_REDUCE) {
        BINARY_REDUCE_LOOP(npy_ulong) {
            const npy_ulong d = *(npy_ulong *)ip2;
            if (NPY_UNLIKELY(d == 0)) {
                npy_set_floatstatus_divbyzero();
                io1 = 0;
            } else {
                io1 /= d;
            }
        }
        *((npy_ulong *)iop1) = io1;
    }
#if NPY_SIMD && defined(TO_SIMD_SFX)
    // for contiguous block of memory, divisor is a scalar and not 0
    else if (IS_BLOCKABLE_BINARY_SCALAR2(sizeof(npy_ulong), NPY_SIMD_WIDTH) &&
             (*(npy_ulong *)args[1]) != 0) {
        TO_SIMD_SFX(simd_divide_by_scalar_contig)(args, dimensions[0]);
    }
#endif
    else {
        BINARY_LOOP {
            const npy_ulong in1 = *(npy_ulong *)ip1;
            const npy_ulong in2 = *(npy_ulong *)ip2;
            if (NPY_UNLIKELY(in2 == 0)) {
                npy_set_floatstatus_divbyzero();
                *((npy_ulong *)op1) = 0;
            } else{
                *((npy_ulong *)op1) = in1 / in2;
            }
        }
    }
}

#line 207
#undef TO_SIMD_SFX
#if 0
#line 212
#elif NPY_BITSOF_LONGLONG == 8
    #define TO_SIMD_SFX(X) X##_u8

#line 212
#elif NPY_BITSOF_LONGLONG == 16
    #define TO_SIMD_SFX(X) X##_u16

#line 212
#elif NPY_BITSOF_LONGLONG == 32
    #define TO_SIMD_SFX(X) X##_u32

#line 212
#elif NPY_BITSOF_LONGLONG == 64
    #define TO_SIMD_SFX(X) X##_u64

#endif
/*
 * For 64-bit division on Armv7, Aarch64, and IBM/Power, NPYV fall-backs to the scalar division
 * because emulating multiply-high on these architectures is going to be expensive comparing
 * to the native scalar dividers.
 * Therefore it's better to disable NPYV in this special case to avoid any unnecessary shuffles.
 * Power10(VSX4) is an exception here since it has native support for integer vector division,
 * note neither infrastructure nor NPYV has supported VSX4 yet.
 */
#if NPY_BITSOF_LONGLONG == 64 && !defined(NPY_HAVE_VSX4) && (defined(NPY_HAVE_VSX) || defined(NPY_HAVE_NEON))
    #undef TO_SIMD_SFX
#endif
NPY_NO_EXPORT void NPY_CPU_DISPATCH_CURFX(ULONGLONG_divide)
(char **args, npy_intp const *dimensions, npy_intp const *steps, void *NPY_UNUSED(func))
{
    if (IS_BINARY_REDUCE) {
        BINARY_REDUCE_LOOP(npy_ulonglong) {
            const npy_ulonglong d = *(npy_ulonglong *)ip2;
            if (NPY_UNLIKELY(d == 0)) {
                npy_set_floatstatus_divbyzero();
                io1 = 0;
            } else {
                io1 /= d;
            }
        }
        *((npy_ulonglong *)iop1) = io1;
    }
#if NPY_SIMD && defined(TO_SIMD_SFX)
    // for contiguous block of memory, divisor is a scalar and not 0
    else if (IS_BLOCKABLE_BINARY_SCALAR2(sizeof(npy_ulonglong), NPY_SIMD_WIDTH) &&
             (*(npy_ulonglong *)args[1]) != 0) {
        TO_SIMD_SFX(simd_divide_by_scalar_contig)(args, dimensions[0]);
    }
#endif
    else {
        BINARY_LOOP {
            const npy_ulonglong in1 = *(npy_ulonglong *)ip1;
            const npy_ulonglong in2 = *(npy_ulonglong *)ip2;
            if (NPY_UNLIKELY(in2 == 0)) {
                npy_set_floatstatus_divbyzero();
                *((npy_ulonglong *)op1) = 0;
            } else{
                *((npy_ulonglong *)op1) = in1 / in2;
            }
        }
    }
}

