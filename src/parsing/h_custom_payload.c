#include "../../incl/parser.h"

int h_custom_payload_hex(t_mgr *mgr, char *input)
{
	/* TODO implement it.*/
	(void)mgr;
	(void)input;
	return (SUCCESS);
}

int h_custom_random_payload(t_mgr *mgr, char *input)
{
	uint8_t		*payload;
	long		len;
	FILE		*devrand;

	if (!input)
		return (hermes_error(FAILURE, 1, "--data-length not specified"));
	if ((len = atoi(input)) <= 0 || len > MAX_PKT_DATA)
		return (hermes_error(FAILURE, 1, "bad data-length for random payload"));
	if ((devrand = fopen("/dev/random", "r")) == NULL)
		return (hermes_error(FAILURE, 2, "fopen()", strerror(errno)));
	/* TODO check len for a sane amount (MAX_IP_DATA_LEN) */
	if (!(payload = (uint8_t *)memalloc((size_t)len)))
		return (hermes_error(FAILURE, 2, "malloc()", strerror(errno)));
	fread(payload, sizeof(uint8_t), (size_t)len, devrand);
	mgr->job.opts.bitops.custom_rand_payload = true;
	mgr->job.custom_payload = payload;
	return (SUCCESS);
}

int h_custom_payload_ascii(t_mgr *mgr, char *input)
{
	if (!input)
		return (hermes_error(FAILURE, 1, "--data not specified"));
	if (!(mgr->job.custom_payload = (void *)strdup(input)))
		return (hermes_error(FAILURE, 2, "malloc()", strerror(errno)));
	mgr->job.opts.bitops.custom_ascii_payload = true;
	return (SUCCESS);
}
