#include "../../incl/parser.h"

void			h_custom_payload_hex(t_job *job, char *input)
{
	/* TODO implement it.*/
	return ;
}

void			h_custom_random_payload(t_job *job, char *input)
{
	uint8_t		*payload;
	size_t		len;
	FILE		*devrand;

	if ((len = (size_t)atoi(input)) <= 0)
		hermes_error(-1, TRUE, 1, "");
	if ((devrand = fopen("/dev/rand", "r")) == NULL)
		hermes_error(errno, TRUE, 2, "fopen()", strerror(errno));
	/* TODO check len for a sane amount (MAX_IP_DATA_LEN) */
	if (!(payload = (uint8_t *)memalloc(len)))
		hermes_error(errno, TRUE, 2, "malloc()", strerror(errno));
	if (fread(payload, sizeof(uint8_t), len, devrand) < len)
		hermes_error(errno, TRUE, 2, "fopen()", strerror(errno));
	job->options.bitops.custom_rand_payload = TRUE;
	job->custom_payload = payload;
}

void			h_custom_payload_ascii(t_job *job, char *input)
{
	if (!(job->custom_payload = (void *)strdup(input)))
		hermes_error(errno, TRUE, 2, "malloc()", strerror(errno));
	job->options.bitops.custom_ascii_payload = TRUE;
}
