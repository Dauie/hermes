#ifndef set_ack_scan
#define set_ack_scan options->scanops->ack
#endif

void h_ack_scan(t_job *job) {
    if (job)
        job->options->scanops->ack = 1;
    // FAILURE
}