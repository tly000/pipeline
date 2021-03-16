//
// Created by tly on 04.08.2018.
//

#pragma once

#include "StandardTypes.h"

typedef float4 qf128;

qf128 qfAssign(float value);
qf128 qfNegate(qf128 value);
void qfAdd(qf128* sum, const qf128 a, const qf128 b);
void qfMul(qf128* prod, const qf128 a, const qf128 b);
void qfMulFloat(qf128* prod, const qf128 a, const float b);
bool qfLessThan(const qf128* a, float b);
qf128 qfDiv(const qf128 a, const qf128 b);
qf128 qfSqr(const qf128 a);

inline qf128 tadd(const qf128 a,const qf128 b){
    qf128 res;
    qfAdd(&res,a,b);
    return res;
}

inline qf128 tsub(const qf128 a,const qf128 b){
    qf128 res;
    qfAdd(&res,a,qfNegate(b));
    return res;
}

inline qf128 tmul(const qf128 a,const qf128 b){
    qf128 res;
    qfMul(&res,a,b);
    return res;
}

inline qf128 tdiv(const qf128 a,const qf128 b){
    return qfDiv(a,b);
}

inline qf128 tabs(const qf128 a){
    return a[0] < 0.0 ? (qfNegate(a)) : (a);
}

inline float tofloat(const qf128 a){
    return a[0];
}

inline qf128 tneg(const qf128 a){
    return qfNegate(a);
}

inline qf128 tsqrt(const qf128 a){
    if (a == qf128{0,0,0,0})
        return qfAssign(0);

    //!!!!!!!!!!
    if (qfLessThan(&a,0)) {
        //TO DO: should return an error
        //return _nan;
        return qfAssign(0);
    }

    qf128 r = qfAssign(1.0f / sqrtf(a[0]));
    qf128 h = a * 0.5f;

    const int PREC = 3;
    for(int i = 0; i < PREC; i++){
        r = tadd(r,tmul((tsub(qfAssign(0.5),tmul(h,qfSqr(r)))),r));
    }
    return tmul(r,a);
}

inline qf128 floatToqf128(const float a){
    return qfAssign(a);
}

/*
 * JOCL - Java bindings for OpenCL
 *
 * Copyright 2009 Marco Hutter - http://www.jocl.org/
 */

// Quad-Float functions for OpenCL float4 type.
// Ported from quad-double (QD) package:
// http://crd.lbl.gov/~dhbailey/mpdist/index.html

inline float4 qfAssign(float value)
{
    return {value, 0.0f, 0.0f, 0.0f};
}

inline float4 qfAssign2(float2 value)
{
    return {value.x, value.y, 0.0f, 0.0f};
}

inline float4 qfNegate(float4 value)
{
    return {-value[0], -value[1], -value[2], -value[3]};
}

inline float two_sum(float a, float b, float *err)
{
    float s = a + b;
    float bb = s - a;
    *err = (a - (s - bb)) + (b - bb);
    return s;
}

inline void three_sum(float *a, float *b, float *c)
{
    float t1, t2, t3;
    t1 = two_sum(*a, *b, &t2);
    *a  = two_sum(*c, t1, &t3);
    *b  = two_sum(t2, t3, c);
}

inline void three_sum2(float *a, float *b, float *c)
{
    float t1, t2, t3;
    t1 = two_sum(*a, *b, &t2);
    *a  = two_sum(*c, t1, &t3);
    *b = t2 + t3;
}


inline float quick_two_sum(float a, float b, float *err)
{
    float s = a + b;
    *err = b - (s - a);
    return s;
}

inline void renorm(float *c0, float *c1,
                   float *c2, float *c3, float *c4)
{
    float s0, s1, s2 = 0.0f, s3 = 0.0f;

    s0 = quick_two_sum(*c3, *c4, c4);
    s0 = quick_two_sum(*c2, s0, c3);
    s0 = quick_two_sum(*c1, s0, c2);
    *c0 = quick_two_sum(*c0, s0, c1);

    s0 = *c0;
    s1 = *c1;

    s0 = quick_two_sum(*c0, *c1, &s1);
    if (s1 != 0.0f)
    {
        s1 = quick_two_sum(s1, *c2, &s2);
        if (s2 != 0.0f)
        {
            s2 = quick_two_sum(s2, *c3, &s3);
            if (s3 != 0.0f)
            {
                s3 += *c4;
            }
            else
            {
                s2 += *c4;
            }
        }
        else
        {
            s1 = quick_two_sum(s1, *c3, &s2);
            if (s2 != 0.0f)
            {
                s2 = quick_two_sum(s2, *c4, &s3);
            }
            else
            {
                s1 = quick_two_sum(s1, *c4, &s2);
            }
        }
    }
    else
    {
        s0 = quick_two_sum(s0, *c2, &s1);
        if (s1 != 0.0f)
        {
            s1 = quick_two_sum(s1, *c3, &s2);
            if (s2 != 0.0f)
            {
                s2 = quick_two_sum(s2, *c4, &s3);
            }
            else
            {
                s1 = quick_two_sum(s1, *c4, &s2);
            }
        }
        else
        {
            s0 = quick_two_sum(s0, *c3, &s1);
            if (s1 != 0.0f)
            {
                s1 = quick_two_sum(s1, *c4, &s2);
            }
            else
            {
                s0 = quick_two_sum(s0, *c4, &s1);
            }
        }
    }

    *c0 = s0;
    *c1 = s1;
    *c2 = s2;
    *c3 = s3;
}



inline void qfAdd(float4 *sum, const float4 a, const float4 b)
{
    float s0, s1, s2, s3;
    float t0, t1, t2, t3;

    s0 = two_sum(a[0], b[0], &t0);
    s1 = two_sum(a[1], b[1], &t1);
    s2 = two_sum(a[2], b[2], &t2);
    s3 = two_sum(a[3], b[3], &t3);

    s1 = two_sum(s1, t0, &t0);
    three_sum(&s2, &t0, &t1);
    three_sum2(&s3, &t0, &t2);
    t0 = t0 + t1 + t3;

    renorm(&s0, &s1, &s2, &s3, &t0);
    (*sum)[0] = s0;
    (*sum)[1] = s1;
    (*sum)[2] = s2;
    (*sum)[3] = s3;
}

inline void split(float a, float *hi, float *lo)
{
    float temp = ((1<<12)+1) * a;
    *hi = temp - (temp - a);
    *lo = a - *hi;
}


inline float two_prod(float a, float b, float *err)
{
    float a_hi, a_lo, b_hi, b_lo;
    float p = a * b;
    split(a, &a_hi, &a_lo);
    split(b, &b_hi, &b_lo);
    *err = ((a_hi * b_hi - p) + a_hi * b_lo + a_lo * b_hi) + a_lo * b_lo;
    return p;
}


inline void qfMul(float4 *prod, const float4 a, const float4 b)
{
    float p0, p1, p2, p3, p4, p5;
    float q0, q1, q2, q3, q4, q5;
    float t0, t1;
    float s0, s1, s2;

    p0 = two_prod(a[0], b[0], &q0);

    p1 = two_prod(a[0], b[1], &q1);
    p2 = two_prod(a[1], b[0], &q2);

    p3 = two_prod(a[0], b[2], &q3);
    p4 = two_prod(a[1], b[1], &q4);
    p5 = two_prod(a[2], b[0], &q5);

    three_sum(&p1, &p2, &q0);

    three_sum(&p2, &q1, &q2);
    three_sum(&p3, &p4, &p5);

    s0 = two_sum(p2, p3, &t0);
    s1 = two_sum(q1, p4, &t1);
    s2 = q2 + p5;
    s1 = two_sum(s1, t0, &t0);
    s2 += (t0 + t1);

    s1 += a[0]*b[3] + a[1]*b[2] + a[2]*b[1] + a[3]*b[0] + q0 + q3 + q4 + q5;
    renorm(&p0, &p1, &s0, &s1, &s2);
    (*prod)[0] = p0;
    (*prod)[1] = p1;
    (*prod)[2] = p2;
    (*prod)[3] = p3;
}


inline void qfMulFloat(float4 *prod, const float4 a, const float b)
{
    float p0, p1, p2, p3;
    float q0, q1, q2;
    float s0, s1, s2, s3, s4;

    p0 = two_prod(a[0], b, &q0);
    p1 = two_prod(a[1], b, &q1);
    p2 = two_prod(a[2], b, &q2);
    p3 = a[3] * b;

    s0 = p0;

    s1 = two_sum(q0, p1, &s2);

    three_sum(&s2, &q1, &p2);

    three_sum2(&q1, &q2, &p3);
    s3 = q1;

    s4 = q2 + p2;

    renorm(&s0, &s1, &s2, &s3, &s4);
    (*prod)[0] = s0;
    (*prod)[1] = s1;
    (*prod)[2] = s2;
    (*prod)[3] = s3;
}


inline bool qfLessThan(const float4 *a, float b)
{
    return ((*a)[0] < b || ((*a)[0] == b && (*a)[1] < 0.0f));
}

inline void renorm4(float *c0, float *c1,
                    float *c2, float *c3)
{
    float s0, s1, s2 = 0.0f, s3 = 0.0f;

    s0 = quick_two_sum(*c2, *c3, c3);
    s0 = quick_two_sum(*c1, s0, c2);
    *c0 = quick_two_sum(*c0, s0, c1);

    s0 = *c0;
    s1 = *c1;
    if (s1 != 0.0f)
    {
        s1 = quick_two_sum(s1, *c2, &s2);
        if (s2 != 0.0f)
        {
            s2 = quick_two_sum(s2, *c3, &s3);
        }
        else
        {
            s1 = quick_two_sum(s1, *c3, &s2);
        }
    }
    else
    {
        s0 = quick_two_sum(s0, *c2, &s1);
        if (s1 != 0.0f)
        {
            s1 = quick_two_sum(s1, *c3, &s2);
        }
        else
        {
            s0 = quick_two_sum(s0, *c3, &s1);
        }
    }
    *c0 = s0;
    *c1 = s1;
    *c2 = s2;
    *c3 = s3;
}

inline float4 qfDiv(const float4 a, const float4 b)
{
    float q0, q1, q2, q3;

    float4 r;
    float4 p;

    q0 = a[0] / b[0];

    // r = a - (b * q0);
    qfMulFloat(&p, b, q0);
    p = qfNegate(p);
    qfAdd(&r, a, p);

    q1 = r[0] / b[0];
    // r -= (b * q1);
    qfMulFloat(&p, b, q1);
    p = qfNegate(p);
    qfAdd(&r, r, p);

    q2 = r[0] / b[0];
    //r -= (b * q2);
    qfMulFloat(&p, b, q2);
    p = qfNegate(p);
    qfAdd(&r, r, p);

    q3 = r[0] / b[0];

    renorm4(&q0, &q1, &q2, &q3);

    return {q0, q1, q2, q3};
}

/* Computes fl(a*a) and err(a*a).  Faster than the above method. */
inline float two_sqr(float a, float* err)
{
    float hi, lo;
    float q = a * a;
    split(a, &hi, &lo);
    *err = ((hi * hi - q) + 2.0 * hi * lo) + lo * lo;
    return q;
}

inline float4 qfSqr(const float4 a)
{
    float p0, p1, p2, p3, p4, p5;
    float q0, q1, q2, q3;
    float s0, s1;
    float t0, t1;

    p0 = two_sqr(a[0], &q0);
    p1 = two_prod(2.0f * a[0], a[1], &q1);
    p2 = two_prod(2.0f * a[0], a[2], &q2);
    p3 = two_sqr(a[1], &q3);

    p1 = two_sum(q0, p1, &q0);

    q0 = two_sum(q0, q1, &q1);
    p2 = two_sum(p2, p3, &p3);

    s0 = two_sum(q0, p2, &t0);
    s1 = two_sum(q1, p3, &t1);

    s1 = two_sum(s1, t0, &t0);
    t0 += t1;

    s1 = quick_two_sum(s1, t0, &t0);
    p2 = quick_two_sum(s0, s1, &t1);
    p3 = quick_two_sum(t1, t0, &q0);

    p4 = 2.0f * a[0] * a[3];
    p5 = 2.0f * a[1] * a[2];

    p4 = two_sum(p4, p5, &p5);
    q2 = two_sum(q2, q3, &q3);

    t0 = two_sum(p4, q2, &t1);
    t1 = t1 + p5 + q3;

    p3 = two_sum(p3, t0, &p4);
    p4 = p4 + q0 + t1;

    renorm(&p0, &p1, &p2, &p3, &p4);
    return {p0, p1, p2, p3};
}