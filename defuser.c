#include "defuser.h"

#include <inttypes.h>
#include <stdio.h>

#define MAX_FIELDS 8

/*
 * 把 values 中的 count 个整数写成一行“空格分隔的无符号十进制整数”，存入 out。
 * 成功返回 0；count 为 0（还没有实现）、snprintf 出错或结果被截断时返回非零。
 */
static int write_answer(char *out, size_t cap, const uint32_t *values, size_t count) {
    size_t used = 0;
    for (size_t i = 0; i < count; ++i) {
        int size = snprintf(out + used, cap - used, "%s%" PRIu32, i ? " " : "", values[i]);
        if (size < 0 || (size_t)size >= cap - used) return 1;
        used += (size_t)size;
    }
    return count == 0;
}

/* phase_01: p1_step1 -> p1_step0 -> p1_step2，全部是 32 位运算。 */
static uint32_t phase1_answer(uint64_t challenge) {
    uint32_t hi = (uint32_t)(challenge >> 32);
    uint32_t mid = (uint32_t)(challenge >> 16);
    uint32_t lo = (uint32_t)challenge;
    uint32_t step1 = (hi * 3u + mid) ^ 0x1140u;
    uint32_t step0 = (lo + hi * 4u) ^ 0xe668u;
    return (step0 - step1) * 5u + 0x8016u;
}

int solve_phase_1(uint64_t challenge, char *out, size_t cap) {
    uint32_t values[MAX_FIELDS] = {phase1_answer(challenge)};
    return write_answer(out, cap, values, 1);
}

/* phase_02: a[0] 取 challenge 的一段位域，后面 5 项由前一项递推。 */
int solve_phase_2(uint64_t challenge, char *out, size_t cap) {
    uint32_t values[MAX_FIELDS];
    values[0] = (uint32_t)((challenge >> 36) & 0x3fu);
    for (uint32_t i = 1; i < 6; ++i) {
        uint32_t piece = (uint32_t)((challenge >> (i + 7)) & 0xfu) + i;
        values[i] = piece ^ (values[i - 1] * 3u);
    }
    return write_answer(out, cap, values, 6);
}

/* 跳转表 0x4057e0 的 8 个槽位，下标是 challenge 的低 3 位。 */
static uint32_t phase3_case(uint32_t index, uint32_t value) {
    switch (index) {
    case 0: return value * 5u + 0x6a11u;
    case 1: return value * 5u + 0x28280u;
    case 2: return (value * 2u) ^ 0x15c5u;
    case 3: return value * 3u + 0x6d47u;
    case 4: return (value ^ 0x26a6u) + (value >> 1);
    case 5: return (value * 2u) ^ 0x17bbu;
    case 6: return value * 5u + 0x49edau;
    default: return (value ^ 0xe9e5u) + (value >> 2);
    }
}

int solve_phase_3(uint64_t challenge, char *out, size_t cap) {
    uint32_t index = (uint32_t)(challenge & 7u);
    uint32_t value = (uint32_t)(challenge >> 32) ^ (uint32_t)(challenge >> 3);
    uint32_t values[MAX_FIELDS] = {index, phase3_case(index, value)};
    return write_answer(out, cap, values, 2);
}

/* phase_04: 在 [0, 127] 上二分，命中后沿返回路径折叠。 */
static uint32_t phase4_walk(uint32_t target, uint32_t lo, uint32_t hi, uint32_t key) {
    uint32_t mid = lo + ((hi - lo) >> 1);
    if (mid == target) return (mid + 0xf5u) ^ key;
    uint32_t folded = mid + key;
    if (target < mid) {
        uint32_t child = phase4_walk(target, lo, mid - 1u, key);
        return (child * 3u) ^ folded ^ 0x19u;
    }
    uint32_t child = phase4_walk(target, mid + 1u, hi, key);
    return (child * 3u) ^ folded ^ 0x0bu;
}

int solve_phase_4(uint64_t challenge, char *out, size_t cap) {
    uint32_t target = (uint32_t)(challenge & 0x7fu);
    uint32_t key = (uint32_t)(challenge >> 7) ^ (uint32_t)(challenge >> 32);
    uint32_t values[MAX_FIELDS] = {phase4_walk(target, 0, 0x7fu, key)};
    return write_answer(out, cap, values, 1);
}

/* p5_nodes：12 个结点按 next 串成一条链，p5_heads 指向其中前 8 个。 */
static const uint32_t p5_value[] = {
    39u, 58934u, 29285u, 11053u, 21877u, 62709u,
    51901u, 6574u, 39139u, 18887u, 61063u, 41750u
};
static const uint32_t p5_tag[] = {
    2905u, 1387u, 1046u, 600u, 1607u, 22u,
    297u, 2390u, 2149u, 780u, 1893u, 2617u
};

int solve_phase_5(uint64_t challenge, char *out, size_t cap) {
    uint32_t index = (uint32_t)(challenge & 7u);
    uint32_t steps = (uint32_t)((challenge >> 3) & 3u);
    uint32_t acc = (uint32_t)(challenge >> 6) ^ (uint32_t)(challenge >> 32);
    acc = (acc ^ 0x564eu) + p5_value[index];
    acc = acc * 5u;
    uint32_t tag = p5_tag[index];
    acc ^= tag;
    for (uint32_t n = 0; n < steps; ++n) {
        index += 1u;
        acc += p5_value[index];
        acc *= 5u;
        tag = p5_tag[index];
        acc ^= tag;
    }
    uint32_t values[MAX_FIELDS] = {acc, tag};
    return write_answer(out, cap, values, 2);
}

static uint32_t rotl32(uint32_t value, unsigned shift) {
    return (value << shift) | (value >> (32u - shift));
}

static uint32_t rotr32(uint32_t value, unsigned shift) {
    return (value >> shift) | (value << (32u - shift));
}

/* 函数指针表 0x405dc0。两次间接调用的下标分别来自 challenge 的不同位域。 */
static uint32_t phase6_op(uint32_t index, uint32_t value) {
    switch (index) {
    case 0: return (value * 4u + 0x4bd1u) ^ (value * 3u);
    case 1: return rotl32(value, 5) ^ 0x6201u;
    case 2: return rotl32(value, 5) ^ 0x6201u;
    case 3: return rotr32(value, 3);
    case 4: return (value * 4u + 0x4bd1u) ^ (value * 3u);
    case 5: return (value ^ 0x3055u) + (value << 7);
    case 6: return (value + 0xa8fu) ^ (value >> 2);
    default: return (value ^ (value >> 1)) + 0x2d18u;
    }
}

int solve_phase_6(uint64_t challenge, char *out, size_t cap) {
    uint32_t state = (uint32_t)(challenge >> 6) ^ (uint32_t)(challenge >> 32);
    state = phase6_op((uint32_t)(challenge & 7u), state) ^ 0xf11bu;
    state = phase6_op((uint32_t)((challenge >> 3) & 7u), state) + 0x2616u;
    uint32_t values[MAX_FIELDS] = {state};
    return write_answer(out, cap, values, 1);
}

static uint32_t secret_mix(uint32_t value) {
    return ((value >> 5) ^ 0xae2au) + (value ^ 0x7754u);
}

int solve_secret(uint64_t challenge, uint64_t context, char *out, size_t cap) {
    uint32_t n0 = (uint32_t)(context & 3u);
    uint32_t challenge_mix = (uint32_t)(challenge >> 32) ^ (uint32_t)challenge;
    uint32_t context_mix = (uint32_t)(context >> 32) ^ (uint32_t)context;
    uint32_t mixed = secret_mix(challenge_mix ^ context_mix);
    uint32_t salt = (n0 * 0x7754u) ^ (uint32_t)(context >> 16);
    uint32_t n1 = mixed ^ salt;
    uint32_t n2 = n1 ^ (context_mix + n0 + 0xae2au + (challenge_mix >> 5));
    /* secret_gate 要求第 32 字节处是 "OPEN"，前面 32 字节内容不参与校验。 */
    int size = snprintf(out, cap, "................................OPEN %" PRIu32 " %" PRIu32 " %" PRIu32,
                        n0, n1, n2);
    if (size < 0 || (size_t)size >= cap) return 1;
    return 0;
}
