
#include "../../munit/munit.h"
#include "../../incl/hermes.h"

static char *ip_class_a[] = {
		(char *)"10.0.0.0", (char *)"10.1.0.0", (char *)"10.42.42.1", NULL
};

//static char *ip_class_b[] = {
//		(char *)"172.16.0.0", (char *)"172.16.23.6", (char *)"172.16.42.1", NULL
//};

static char *ip_class_c[] = {
		(char *)"192.168.0.0", (char *)"192.168.23.5", (char *)"192.168.42.9", NULL
};

static MunitParameterEnum ip4_cmp_test_greaterless_params[] = {
		{ (char *)"class_a", ip_class_a },
		{ (char *)"class_c", ip_class_c}
};

static MunitParameterEnum ip4_cmp_test_equal[] = {
		{ (char *)"class_c", ip_class_c}
};

static MunitResult	test_ip4_cmp_less(const MunitParameter params[], void *user_data)
{
	const char		*class_a;
	const char		*class_c;
	uint32_t		ip1;
	uint32_t		ip2;

	class_a = munit_parameters_get(params, "class_a");
	class_c = munit_parameters_get(params, "class_c");
	(void)user_data;
	inet_pton(AF_INET, class_a, &ip1);
	inet_pton(AF_INET, class_c, &ip2);
	munit_assert_int(ip4_cmp(&ip1, &ip2), <, 0);
	return (MUNIT_OK);
}

static MunitResult	test_ip4_cmp_great(const MunitParameter params[], void *user_data)
{
	const char		*class_a;
	const char		*class_c;
	uint32_t		ip1;
	uint32_t		ip2;

	class_a = munit_parameters_get(params, "class_a");
	class_c = munit_parameters_get(params, "class_c");
	(void)user_data;
	inet_pton(AF_INET, class_a, &ip1);
	inet_pton(AF_INET, class_c, &ip2);
	munit_assert_int(ip4_cmp(&ip2, &ip1), >, 0);
	return (MUNIT_OK);
}

static MunitResult	test_ip4_cmp_equal(const MunitParameter params[], void *user_data)
{
	const char		*class_c;
	uint32_t		ip1;
	uint32_t		ip2;

	class_c = munit_parameters_get(params, "class_c");
	(void)user_data;
	inet_pton(AF_INET, class_c, &ip1);
	inet_pton(AF_INET, class_c, &ip2);
	munit_assert_int(ip4_cmp(&ip2, &ip1), ==, 0);
	return (MUNIT_OK);
}

static const MunitTest ip4_tests[] = {
		{ (char *)"ip4_cmp less-than", test_ip4_cmp_less, NULL, NULL, MUNIT_TEST_OPTION_NONE, ip4_cmp_test_greaterless_params},
		{ (char *)"ip4_cmp greater-than", test_ip4_cmp_great, NULL, NULL, MUNIT_TEST_OPTION_NONE, ip4_cmp_test_greaterless_params},
		{ (char *)"ip4_cmp greater-equal", test_ip4_cmp_equal, NULL, NULL, MUNIT_TEST_OPTION_NONE, ip4_cmp_test_equal},
		{ NULL, NULL, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL },
};

static const MunitSuite hermes_test_suites = {
		(char *)"",				/*Add prefix-name to output of tests*/
		(MunitTest*)ip4_tests,	/*Start of suites to use during test runs*/
		NULL,					/*End of suites to use during test runs*/
		1,						/*Amount of times to run tests*/
		MUNIT_SUITE_OPTION_NONE
};

int		main(int ac, char *av[MUNIT_ARRAY_PARAM(ac + 1)])
{
	return (munit_suite_main(&hermes_test_suites, (void*)"unit", ac, av));
}