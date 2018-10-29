# include "../../incl/hermes.h"

int				h_custom_payload_hex(t_mgr *mgr, char *input)
{
	/* TODO implement it.*/
	(void)mgr;
	(void)input;
	return (SUCCESS);
}

int				h_custom_random_payload(t_mgr *mgr, char *input)
{
	uint8_t		*payload;
	long		len;
	FILE		*devrand;

	if (!input)
		return (hermes_error(EXIT_FAILURE, "--data-length not specified"));
	if ((len = atoi(input)) <= 0 || len > MAX_PKT_DATA)
		return (hermes_error(EXIT_FAILURE, "bad data-length for random payload"));
	if ((devrand = fopen("/dev/random", "r")) == NULL)
		return (hermes_error(EXIT_FAILURE, "fopen() %s", strerror(errno)));
	/* TODO check len for a sane amount (MAX_IP_DATA_LEN) */
	if (!(payload = (uint8_t *)memalloc((size_t)len)))
		return (hermes_error(EXIT_FAILURE, "malloc() %s", strerror(errno)));
	fread(payload, sizeof(uint8_t), (size_t)len, devrand);
	mgr->env.custom_payload = payload;
	return (SUCCESS);
}

int				h_custom_payload_ascii(t_mgr *mgr, char *input)
{
	if (!input)
		return (hermes_error(EXIT_FAILURE, "--data not specified"));
	if (!(mgr->env.custom_payload = (void *)strdup(input)))
		return (hermes_error(EXIT_FAILURE, "malloc() %s", strerror(errno)));
	return (SUCCESS);
}
