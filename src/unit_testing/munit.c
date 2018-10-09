#include "../../munit/munit.h"
#include "../../incl/hermes.h"

static const MunitTest ip4_tests[] = {
		{},
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