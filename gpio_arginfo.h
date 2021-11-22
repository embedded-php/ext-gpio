/* This is a generated file, edit the .stub.php file instead.
 * Stub hash: c047d6153147ddf44f84970564cb788c751e0867 */

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_GPIO_Chip___construct, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, path, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, abiVersion, IS_LONG, 0, "self::ABI_ANY")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_GPIO_Chip_isAbiV2, 0, 0, _IS_BOOL, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_class_GPIO_Chip_getAllPins, 0, 0, GPIO\\Pins, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_class_GPIO_Chip_getPin, 0, 1, GPIO\\Pin, 0)
	ZEND_ARG_TYPE_INFO(0, offset, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_class_GPIO_Chip_getPins, 0, 1, GPIO\\Pins, 0)
	ZEND_ARG_TYPE_INFO(0, offsets, IS_ARRAY, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_GPIO_Chip_getPinCount, 0, 0, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_GPIO_Chip_getPath, 0, 0, IS_STRING, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_GPIO_Chip_getLabel arginfo_class_GPIO_Chip_getPath

#define arginfo_class_GPIO_Chip_getName arginfo_class_GPIO_Chip_getPath

#define arginfo_class_GPIO_Event_getType arginfo_class_GPIO_Chip_getPinCount

#define arginfo_class_GPIO_Event_getTimestamp arginfo_class_GPIO_Chip_getPinCount

#define arginfo_class_GPIO_Pin_getBias arginfo_class_GPIO_Chip_getPinCount

#define arginfo_class_GPIO_Pin_getConsumer arginfo_class_GPIO_Chip_getPath

#define arginfo_class_GPIO_Pin_getDrive arginfo_class_GPIO_Chip_getPinCount

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_class_GPIO_Pin_getChip, 0, 0, GPIO\\Chip, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_GPIO_Pin_isActiveLow arginfo_class_GPIO_Chip_isAbiV2

#define arginfo_class_GPIO_Pin_isUsed arginfo_class_GPIO_Chip_isAbiV2

#define arginfo_class_GPIO_Pin_getName arginfo_class_GPIO_Chip_getPath

#define arginfo_class_GPIO_Pin_getOffset arginfo_class_GPIO_Chip_getPinCount

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_class_GPIO_Pin_asInput, 0, 1, GPIO\\Pin, 0)
	ZEND_ARG_TYPE_INFO(0, consumer, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, flags, IS_LONG, 0, "self::FLAG_NONE")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, debouncePeriod, IS_LONG, 0, "0")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_class_GPIO_Pin_asOutput, 0, 1, GPIO\\Pin, 0)
	ZEND_ARG_TYPE_INFO(0, consumer, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, flags, IS_LONG, 0, "self::FLAG_NONE")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, setHigh, _IS_BOOL, 0, "false")
ZEND_END_ARG_INFO()

#define arginfo_class_GPIO_Pin_isInput arginfo_class_GPIO_Chip_isAbiV2

#define arginfo_class_GPIO_Pin_isOutput arginfo_class_GPIO_Chip_isAbiV2

#define arginfo_class_GPIO_Pin_isHigh arginfo_class_GPIO_Chip_isAbiV2

#define arginfo_class_GPIO_Pin_isLow arginfo_class_GPIO_Chip_isAbiV2

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_class_GPIO_Pin_setHigh, 0, 0, GPIO\\Pin, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_GPIO_Pin_setLow arginfo_class_GPIO_Pin_setHigh

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_class_GPIO_Pin_waitForEdge, 0, 1, GPIO\\Event, 1)
	ZEND_ARG_TYPE_INFO(0, seconds, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, nanoseconds, IS_LONG, 0, "0")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, edgeType, IS_LONG, 0, "self::BOTH_EDGES")
ZEND_END_ARG_INFO()

#define arginfo_class_GPIO_Pins_getChip arginfo_class_GPIO_Pin_getChip

#define arginfo_class_GPIO_Pins_count arginfo_class_GPIO_Chip_getPinCount

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_GPIO_Pins_offsetExists, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_INFO(0, offset)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_GPIO_Pins_offsetGet, 0, 0, 1)
	ZEND_ARG_INFO(0, offset)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_GPIO_Pins_offsetSet, 0, 0, 2)
	ZEND_ARG_INFO(0, offset)
	ZEND_ARG_INFO(0, value)
ZEND_END_ARG_INFO()

#define arginfo_class_GPIO_Pins_offsetUnset arginfo_class_GPIO_Pins_offsetGet

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_GPIO_Pins_current, 0, 0, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_GPIO_Pins_key arginfo_class_GPIO_Pins_current

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_GPIO_Pins_next, 0, 0, IS_VOID, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_GPIO_Pins_rewind arginfo_class_GPIO_Pins_next

#define arginfo_class_GPIO_Pins_valid arginfo_class_GPIO_Chip_isAbiV2


ZEND_METHOD(GPIO_Chip, __construct);
ZEND_METHOD(GPIO_Chip, isAbiV2);
ZEND_METHOD(GPIO_Chip, getAllPins);
ZEND_METHOD(GPIO_Chip, getPin);
ZEND_METHOD(GPIO_Chip, getPins);
ZEND_METHOD(GPIO_Chip, getPinCount);
ZEND_METHOD(GPIO_Chip, getPath);
ZEND_METHOD(GPIO_Chip, getLabel);
ZEND_METHOD(GPIO_Chip, getName);
ZEND_METHOD(GPIO_Event, getType);
ZEND_METHOD(GPIO_Event, getTimestamp);
ZEND_METHOD(GPIO_Pin, getBias);
ZEND_METHOD(GPIO_Pin, getConsumer);
ZEND_METHOD(GPIO_Pin, getDrive);
ZEND_METHOD(GPIO_Pin, getChip);
ZEND_METHOD(GPIO_Pin, isActiveLow);
ZEND_METHOD(GPIO_Pin, isUsed);
ZEND_METHOD(GPIO_Pin, getName);
ZEND_METHOD(GPIO_Pin, getOffset);
ZEND_METHOD(GPIO_Pin, asInput);
ZEND_METHOD(GPIO_Pin, asOutput);
ZEND_METHOD(GPIO_Pin, isInput);
ZEND_METHOD(GPIO_Pin, isOutput);
ZEND_METHOD(GPIO_Pin, isHigh);
ZEND_METHOD(GPIO_Pin, isLow);
ZEND_METHOD(GPIO_Pin, setHigh);
ZEND_METHOD(GPIO_Pin, setLow);
ZEND_METHOD(GPIO_Pin, waitForEdge);
ZEND_METHOD(GPIO_Pins, getChip);
ZEND_METHOD(GPIO_Pins, count);
ZEND_METHOD(GPIO_Pins, offsetExists);
ZEND_METHOD(GPIO_Pins, offsetGet);
ZEND_METHOD(GPIO_Pins, offsetSet);
ZEND_METHOD(GPIO_Pins, offsetUnset);
ZEND_METHOD(GPIO_Pins, current);
ZEND_METHOD(GPIO_Pins, key);
ZEND_METHOD(GPIO_Pins, next);
ZEND_METHOD(GPIO_Pins, rewind);
ZEND_METHOD(GPIO_Pins, valid);


static const zend_function_entry class_GPIO_Chip_methods[] = {
	ZEND_ME(GPIO_Chip, __construct, arginfo_class_GPIO_Chip___construct, ZEND_ACC_PUBLIC)
	ZEND_ME(GPIO_Chip, isAbiV2, arginfo_class_GPIO_Chip_isAbiV2, ZEND_ACC_PUBLIC)
	ZEND_ME(GPIO_Chip, getAllPins, arginfo_class_GPIO_Chip_getAllPins, ZEND_ACC_PUBLIC)
	ZEND_ME(GPIO_Chip, getPin, arginfo_class_GPIO_Chip_getPin, ZEND_ACC_PUBLIC)
	ZEND_ME(GPIO_Chip, getPins, arginfo_class_GPIO_Chip_getPins, ZEND_ACC_PUBLIC)
	ZEND_ME(GPIO_Chip, getPinCount, arginfo_class_GPIO_Chip_getPinCount, ZEND_ACC_PUBLIC)
	ZEND_ME(GPIO_Chip, getPath, arginfo_class_GPIO_Chip_getPath, ZEND_ACC_PUBLIC)
	ZEND_ME(GPIO_Chip, getLabel, arginfo_class_GPIO_Chip_getLabel, ZEND_ACC_PUBLIC)
	ZEND_ME(GPIO_Chip, getName, arginfo_class_GPIO_Chip_getName, ZEND_ACC_PUBLIC)
	ZEND_FE_END
};


static const zend_function_entry class_GPIO_Event_methods[] = {
	ZEND_ME(GPIO_Event, getType, arginfo_class_GPIO_Event_getType, ZEND_ACC_PUBLIC)
	ZEND_ME(GPIO_Event, getTimestamp, arginfo_class_GPIO_Event_getTimestamp, ZEND_ACC_PUBLIC)
	ZEND_FE_END
};


static const zend_function_entry class_GPIO_Exception_methods[] = {
	ZEND_FE_END
};


static const zend_function_entry class_GPIO_Pin_methods[] = {
	ZEND_ME(GPIO_Pin, getBias, arginfo_class_GPIO_Pin_getBias, ZEND_ACC_PUBLIC)
	ZEND_ME(GPIO_Pin, getConsumer, arginfo_class_GPIO_Pin_getConsumer, ZEND_ACC_PUBLIC)
	ZEND_ME(GPIO_Pin, getDrive, arginfo_class_GPIO_Pin_getDrive, ZEND_ACC_PUBLIC)
	ZEND_ME(GPIO_Pin, getChip, arginfo_class_GPIO_Pin_getChip, ZEND_ACC_PUBLIC)
	ZEND_ME(GPIO_Pin, isActiveLow, arginfo_class_GPIO_Pin_isActiveLow, ZEND_ACC_PUBLIC)
	ZEND_ME(GPIO_Pin, isUsed, arginfo_class_GPIO_Pin_isUsed, ZEND_ACC_PUBLIC)
	ZEND_ME(GPIO_Pin, getName, arginfo_class_GPIO_Pin_getName, ZEND_ACC_PUBLIC)
	ZEND_ME(GPIO_Pin, getOffset, arginfo_class_GPIO_Pin_getOffset, ZEND_ACC_PUBLIC)
	ZEND_ME(GPIO_Pin, asInput, arginfo_class_GPIO_Pin_asInput, ZEND_ACC_PUBLIC)
	ZEND_ME(GPIO_Pin, asOutput, arginfo_class_GPIO_Pin_asOutput, ZEND_ACC_PUBLIC)
	ZEND_ME(GPIO_Pin, isInput, arginfo_class_GPIO_Pin_isInput, ZEND_ACC_PUBLIC)
	ZEND_ME(GPIO_Pin, isOutput, arginfo_class_GPIO_Pin_isOutput, ZEND_ACC_PUBLIC)
	ZEND_ME(GPIO_Pin, isHigh, arginfo_class_GPIO_Pin_isHigh, ZEND_ACC_PUBLIC)
	ZEND_ME(GPIO_Pin, isLow, arginfo_class_GPIO_Pin_isLow, ZEND_ACC_PUBLIC)
	ZEND_ME(GPIO_Pin, setHigh, arginfo_class_GPIO_Pin_setHigh, ZEND_ACC_PUBLIC)
	ZEND_ME(GPIO_Pin, setLow, arginfo_class_GPIO_Pin_setLow, ZEND_ACC_PUBLIC)
	ZEND_ME(GPIO_Pin, waitForEdge, arginfo_class_GPIO_Pin_waitForEdge, ZEND_ACC_PUBLIC)
	ZEND_FE_END
};


static const zend_function_entry class_GPIO_Pins_methods[] = {
	ZEND_ME(GPIO_Pins, getChip, arginfo_class_GPIO_Pins_getChip, ZEND_ACC_PUBLIC)
	ZEND_ME(GPIO_Pins, count, arginfo_class_GPIO_Pins_count, ZEND_ACC_PUBLIC)
	ZEND_ME(GPIO_Pins, offsetExists, arginfo_class_GPIO_Pins_offsetExists, ZEND_ACC_PUBLIC)
	ZEND_ME(GPIO_Pins, offsetGet, arginfo_class_GPIO_Pins_offsetGet, ZEND_ACC_PUBLIC)
	ZEND_ME(GPIO_Pins, offsetSet, arginfo_class_GPIO_Pins_offsetSet, ZEND_ACC_PUBLIC)
	ZEND_ME(GPIO_Pins, offsetUnset, arginfo_class_GPIO_Pins_offsetUnset, ZEND_ACC_PUBLIC)
	ZEND_ME(GPIO_Pins, current, arginfo_class_GPIO_Pins_current, ZEND_ACC_PUBLIC)
	ZEND_ME(GPIO_Pins, key, arginfo_class_GPIO_Pins_key, ZEND_ACC_PUBLIC)
	ZEND_ME(GPIO_Pins, next, arginfo_class_GPIO_Pins_next, ZEND_ACC_PUBLIC)
	ZEND_ME(GPIO_Pins, rewind, arginfo_class_GPIO_Pins_rewind, ZEND_ACC_PUBLIC)
	ZEND_ME(GPIO_Pins, valid, arginfo_class_GPIO_Pins_valid, ZEND_ACC_PUBLIC)
	ZEND_FE_END
};
