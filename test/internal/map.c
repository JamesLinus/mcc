#include "internal.h"

static void test_map()
{
	struct map *map = map_new();

	expectp(map_get(map, "name"), NULL);

	map_put(map, "name", "Bill");
	expects(map_get(map, "name"), "Bill");

	map_put(map, "name", NULL);
	expectp(map_get(map, "name"), NULL);

	map_put(map, "os", "mac os x");
	map_put(map, "os", "ubuntu");
	expects(map_get(map, "os"), "ubuntu");

	map_put(map, "os", NULL);
	expectp(map_get(map, "os"), NULL);

	map_put(map, "op", "delete");
	expects(map_get(map, "op"), "delete");

	const char *key1 = "key1";
	const char *key11 = strs("key1");
	map_put(map, key1, "value1");
	expects(map_get(map, key1), "value1");
	expects(map_get(map, key11), "value1");

	map_free(map);
}

void testmain()
{
	START("map ...");
	test_map();
}
