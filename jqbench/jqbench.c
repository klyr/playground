#include <stdio.h>
#include <time.h>
#include <stdlib.h>

#include <hdr_histogram.h>

#include <jq.h>

const char *source = "{\"sensor_name\": \"fridge1\",\"temperatureC\": 3.1,\"timestamp\": 1499681285}";
const char *filter = "{sensor: .sensor_name,temperatureC,timestamp,temperatureF: (.temperatureC * 9/5 + 32),date: .timestamp | todateiso8601,transformedAt: (now)}";

static int64_t diff_in_ns(struct timespec t1, struct timespec t2) {
    struct timespec diff;

    if (t2.tv_nsec-t1.tv_nsec < 0) {
        diff.tv_sec  = t2.tv_sec - t1.tv_sec - 1;
        diff.tv_nsec = t2.tv_nsec - t1.tv_nsec + 1000000000;
    } else {
        diff.tv_sec  = t2.tv_sec - t1.tv_sec;
        diff.tv_nsec = t2.tv_nsec - t1.tv_nsec;
    }
    return (diff.tv_sec * 1000000000.0 + diff.tv_nsec);
}

static void transform(jq_state *s, const char *source) {
    jv jv_source, result;

    jv_source = jv_parse(source);
    jq_start(s, jv_source, 0);

    while (jv_is_valid(result = jq_next(s))) {
        jv_free(result);
    }
    jv_free(result);
}

static void print_resolution() {
    struct timespec tp;
    clockid_t clk_id;

    clk_id = CLOCK_MONOTONIC;

    clock_getres(clk_id, &tp);
    printf("[+] Resolution:\n");
    printf(" |   tp.tv_sec: %ld\n", tp.tv_sec);
    printf(" |   tp.tv_nsec: %ld\n", tp.tv_nsec);
    printf(" +------------\n");
}

static void bench(const char *source, const char *filter, int nb, struct hdr_histogram *h) {
    jq_state *s;
    struct timespec start, end;
    clockid_t clk_id = CLOCK_MONOTONIC;

    s = jq_init();

    jq_compile(s, filter);

    for (int i = 0; i < nb; ++i) {
        clock_gettime(clk_id, &start);
        transform(s, source);
        clock_gettime(clk_id, &end);

        hdr_record_value(h, diff_in_ns(start, end));
    }

    jq_teardown(&s);
}

void print_result(struct hdr_histogram *h) {
    hdr_percentiles_print(h, stdout, 5, 1.0, CLASSIC);
}

int main(int argc, const char *argv[]) {
    int rc;
    struct hdr_histogram *histogram;

    int64_t max_value = INT64_C(24) * 60 * 60 * 1000000;
    int64_t min_value = 1;

    rc = hdr_init(min_value, max_value, 1, &histogram);
    if (rc != 0) {
        printf("[E] Unable to allocate histogram\n");
        exit(1);
    }

    print_resolution();
    bench(source, filter, 1000000, histogram);

    printf("[i] Values are in nano seconds\n");
    print_result(histogram);

    free(histogram);

#if 0
    for (const char **source = sources; *source != NULL; ++source) {
        jv_source = jv_parse(*source);
        jq_start(s, jv_source, 0);

        while (jv_is_valid(result = jq_next(s))) {
            jv_show(result, 0);
        }

        jv_free(result);
    }
#endif

    return(0);
}
