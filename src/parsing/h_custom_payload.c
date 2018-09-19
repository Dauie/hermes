#include "../../incl/parser.h"

void			h_custom_payload_hex(t_job *job, char *input)
{
	/* TODO implement it.*/
	(void)job;
	(void)input;
	return ;
}

void			h_custom_random_payload(t_job *job, char *input)
{
	uint8_t		*payload;
	long		len;
	FILE		*devrand;

	if (!input)
		hermes_error(INPUT_ERROR, TRUE, 1, "--data-length not specified");
	if ((len = atoi(input)) <= 0 || len > MAX_PKT_DATA)
		hermes_error(INPUT_ERROR, TRUE, 1, "bad data-length for random payload");
	if ((devrand = fopen("/dev/random", "r")) == NULL)
		hermes_error(errno, TRUE, 2, "fopen()", strerror(errno));
	/* TODO check len for a sane amount (MAX_IP_DATA_LEN) */
	if (!(payload = (uint8_t *)memalloc((size_t)len)))
		hermes_error(errno, TRUE, 2, "malloc()", strerror(errno));
	if (fread(payload, sizeof(uint8_t), (size_t)len, devrand) < len)
		hermes_error(errno, TRUE, 2, "fread()", strerror(errno));
	job->options.bitops.custom_rand_payload = TRUE;
	job->custom_payload = payload;
}

void			h_custom_payload_ascii(t_job *job, char *input)
{
	if (!input)
		hermes_error(INPUT_ERROR, TRUE, 1, "--data not specified");
	if (!(job->custom_payload = (void *)strdup(input)))
		hermes_error(errno, TRUE, 2, "malloc()", strerror(errno));
	job->options.bitops.custom_ascii_payload = TRUE;
}
