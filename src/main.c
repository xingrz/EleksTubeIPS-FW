#include <logging/log.h>
#include <zephyr.h>

LOG_MODULE_REGISTER(main);

void
main(void)
{
	LOG_INF("SYSTEM READY");
}
