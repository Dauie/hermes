#include "../../../incl/parser.h"

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
		return ;					/*TODO hermese_error() */
	if ((devrand = fopen("/dev/rand", "r")) == NULL)
		return ;					/* TODO hermese_error() */
	/* TODO check len for a sane amount (MAX_IP_DATA_LEN) */
	if (!(payload = (uint8_t *)memalloc(len)))
		return ;					/* TODO hermese_error() */
	if (fread(payload, sizeof(uint8_t), len, devrand) < len)
		return ;					/* TODO hermese_error() for not getting the correct payload size */
	job->custom_payload = payload;
}

void			h_custom_payload_ascii(t_job *job, char *input)
{
	job->options.evasops.ascii_payload = TRUE;
	if (!(job->custom_payload = (void *)strdup(input)))
		return ; /* TODO hermese_error() */
}
