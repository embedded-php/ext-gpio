dnl config.m4 for extension gpio

PHP_ARG_WITH(
  gpio,
  for gpio support,
  [
    AS_HELP_STRING(
      [ --with-gpio ],
      [ Include gpio support ]
    )
  ]
)

if test "{PHP_GPIO}" != "no"; then
  AC_DEFINE(HAVE_GPIO, 1, [ Have gpio support ])

  PHP_NEW_EXTENSION(gpio, gpio.c src/chip.c src/exception.c src/event.c src/pin.c src/pins.c, $ext_shared)
fi
