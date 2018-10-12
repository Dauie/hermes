
#include "../../munit/munit.h"
#include "../../incl/hermes.h"

static char *ip_class_a[] = {
		(char *)"10.21.7.80", (char *)"10.34.25.3", (char *)"10.42.42.1", NULL
};

//static char *ip_class_b[] = {
//		(char *)"172.16.0.0", (char *)"172.16.23.6", (char *)"172.16.42.1", NULL
//};

static char *ip_class_c[] = {
		(char *)"192.168.99.21", (char *)"192.168.23.5", (char *)"192.168.42.9", NULL
};

static MunitParameterEnum ip4_cmp_test_greaterless_params[] = {
		{ (char *)"class_a", ip_class_a },
		{ (char *)"class_c", ip_class_c },
		{ NULL, NULL }
};

static MunitParameterEnum ip4_cmp_test_equal_params[] = {
		{ (char *)"class_c", ip_class_c},
		{ NULL, NULL }
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

static MunitResult	test_ip4_increment(const MunitParameter params[], void *user_data)
{
	struct in_addr	ip1;
	struct in_addr	ip2;
	struct in_addr	ip3;
	struct in_addr	ip4;
	char			*ip1oi = {"255.255.255.255"};
	char			*ip2oi = {"192.255.255.255"};
	char			*ip3oi = {"192.168.255.255"};
	char			*ip4oi = {"192.168.0.255"};

	(void)params;
	(void)user_data;
	inet_pton(AF_INET, ip1oi, &ip1.s_addr);
	inet_pton(AF_INET, ip2oi, &ip2.s_addr);
	inet_pton(AF_INET, ip3oi, &ip3.s_addr);
	inet_pton(AF_INET, ip4oi, &ip4.s_addr);
	ip1.s_addr = ip4_increment(ip1.s_addr, 1);
	ip2.s_addr = ip4_increment(ip2.s_addr, 1);
	ip3.s_addr = ip4_increment(ip3.s_addr, 1);
	ip4.s_addr = ip4_increment(ip4.s_addr, 1);
	munit_assert_string_equal(inet_ntoa(ip1), "0.0.0.0");
	munit_assert_string_equal(inet_ntoa(ip2), "193.0.0.0");
	munit_assert_string_equal(inet_ntoa(ip3), "192.169.0.0");
	munit_assert_string_equal(inet_ntoa(ip4), "192.168.1.0");
	return (MUNIT_OK);
}

static MunitResult	test_ip4_decrement(const MunitParameter params[], void *user_data)
{
	struct in_addr	ip1;
	struct in_addr	ip2;
	struct in_addr	ip3;
	struct in_addr	ip4;
	char			*ip1oi = {"0.0.0.0"};
	char			*ip2oi = {"192.0.0.0"};
	char			*ip3oi = {"192.168.0.0"};
	char			*ip4oi = {"192.168.1.0"};

	(void)params;
	(void)user_data;
	inet_pton(AF_INET, ip1oi, &ip1.s_addr);
	inet_pton(AF_INET, ip2oi, &ip2.s_addr);
	inet_pton(AF_INET, ip3oi, &ip3.s_addr);
	inet_pton(AF_INET, ip4oi, &ip4.s_addr);
	ip1.s_addr = ip4_decrement(ip1.s_addr, 1);
	ip2.s_addr = ip4_decrement(ip2.s_addr, 1);
	ip3.s_addr = ip4_decrement(ip3.s_addr, 1);
	ip4.s_addr = ip4_decrement(ip4.s_addr, 1);
	munit_assert_string_equal(inet_ntoa(ip1), "255.255.255.255");
	munit_assert_string_equal(inet_ntoa(ip2), "191.255.255.255");
	munit_assert_string_equal(inet_ntoa(ip3), "192.167.255.255");
	munit_assert_string_equal(inet_ntoa(ip4), "192.168.0.255");
	return (MUNIT_OK);
}

static const MunitTest ip4_tests[] = {
		{ (char *)"ip4_cmp less-than", test_ip4_cmp_less, NULL, NULL, MUNIT_TEST_OPTION_NONE, ip4_cmp_test_greaterless_params},
		{ (char *)"ip4_cmp greater-than", test_ip4_cmp_great, NULL, NULL, MUNIT_TEST_OPTION_NONE, ip4_cmp_test_greaterless_params},
		{ (char *)"ip4_cmp equal", test_ip4_cmp_equal, NULL, NULL, MUNIT_TEST_OPTION_NONE, ip4_cmp_test_equal_params},
		{ (char *)"ip4_cmp increment", test_ip4_increment, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
		{ (char *)"ip4_cmp decrement", test_ip4_decrement, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
		{ NULL, NULL, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL },
};

static const MunitSuite hermes_test_suites = {
		(char *)"",				/*Add prefix-name to output of tests*/
		(MunitTest*)ip4_tests,	/*Start of suites to use during test runs*/
		NULL,					/*End of suites to use during test runs*/
		1,						/*Amount of times to running tests*/
		MUNIT_SUITE_OPTION_NONE
};

int		main(int ac, char *av[MUNIT_ARRAY_PARAM(ac + 1)])
{
	return (munit_suite_main(&hermes_test_suites, (void*)"unit", ac, av));
}