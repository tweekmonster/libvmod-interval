#include <stdlib.h>
#include <time.h>
#include <config.h>

#include "vrt.h"
#include "bin/varnishd/cache.h"

#include "vcc_if.h"


time_t
future_time(const time_t *t, int hour, int minute, int second)
{
    struct tm *future = localtime(t);

    if ((future->tm_hour == hour && future->tm_min == minute
            && future->tm_sec >= second)
        || (future->tm_hour == hour && future->tm_min >= minute)
        || future->tm_hour > hour)
    {
        future->tm_mday++;
    }

    future->tm_hour = hour;
    future->tm_min = minute;
    future->tm_sec = second;

    return mktime(future);
}


time_t
adjust_time(const time_t *t, int hour, int minute, int second)
{
    struct tm *adjust = localtime(t);

    adjust->tm_hour = hour;
    adjust->tm_min = minute;
    adjust->tm_sec = second;

    return mktime(adjust);
}


int
init_function(struct vmod_priv *priv, const struct VCL_conf *conf)
{
#ifdef TIMEZONE
    setenv("TZ", TIMEZONE, 1);
#endif
    return (0);
}


double
vmod_every(struct sess *sp, int hour, int minute, int second)
{
    int m = 3600 * hour + 60 * minute + second;

    if (m == 0)
    {
        return 0.0;
    }

    int t = (int)time(NULL);
    int r = t % m;
    if (r == 0)
    {
        return 0.0;
    }

    int t2 = t + m - r;

    return (double)(t2 - t);
}


double
vmod_at(struct sess *sp, int hour, int minute, int second)
{
    time_t t1 = time(0);
    time_t t2 = future_time(&t1, hour, minute, second);
    return (double)(t2 - t1);
}


unsigned
vmod_is_before(struct sess *sp, int hour, int minute, int second)
{
    time_t t1 = time(NULL);
    time_t t2 = adjust_time(&t1, hour, minute, second);
    return t1 < t2;
}


unsigned
vmod_is_after(struct sess *sp, int hour, int minute, int second)
{
    time_t t1 = time(NULL);
    time_t t2 = adjust_time(&t1, hour, minute, second);
    return t1 > t2;
}
