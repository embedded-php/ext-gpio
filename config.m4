dnl config.m4 for extension gpio

PHP_ARG_WITH(
  gpio,
  for gpio support,
  [
    AS_HELP_STRING(
      [ --with-gpio ],
      [ Include gpio support ]
    )
  ],
  no
)

if test "{PHP_GPIO}" != "no"; then
  KERNEL_VERSION=$(uname --kernel-release)
  HIGHER_VERSION=$(echo -e "$KERNEL_VERSION\n5.10" | sort --version-sort --field-separator='.' | tail --lines=1)
  AC_MSG_CHECKING([Kernel version]);
  if test "${HIGHER_VERSION}" != "${KERNEL_VERSION}"; then
    AC_MSG_ERROR([gpio requires Kernel 5.10+]);
  fi
  AC_MSG_RESULT([$KERNEL_VERSION])


  PHP_VERSION=$($PHP_CONFIG --vernum)
  AC_MSG_CHECKING([PHP version]);
  if test $PHP_VERSION -lt 80000; then
    AC_MSG_ERROR([gpio requires PHP 8.0+]);
  fi
  AC_MSG_RESULT([$PHP_VERSION])

  AC_DEFINE(HAVE_GPIO, 1, [ Have gpio support ])

  PHP_NEW_EXTENSION(gpio, gpio.c src/chip.c src/exception.c src/event.c src/pin.c src/pins.c, $ext_shared)
fi
