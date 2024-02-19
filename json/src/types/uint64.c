#include "json/types/uint64.h"
#include "util/maths.h"        // for flo_power
#include "util/text/char.h"    // for flo_isNumerical
#include "json/types/common.h" // for flo_json_isEndOfJsonValue
#include <stddef.h>            // for ptrdiff_t

#define BASE 10

flo_json_uint64ParseResult flo_json_parseUint64(flo_parseStatus *ps) {
    flo_json_uint64ParseResult result;

    uint64_t integral = 0;
    uint64_t exponent = 0;

    ptrdiff_t integralCharCount = 0;
    bool foundExponent = false;
    FLO_PARSE_NEXT_CHAR_UNTIL(*ps, (flo_json_isEndOfJsonValue(ch)), {
        if (!flo_isNumerical(ch)) {
            switch (ch) {
            case 'e':
            case 'E': {
                if (foundExponent || integralCharCount == 0) {
                    result.result = FLO_DESERIALIZATION_BAD_FORMAT;
                    return result;
                }
                foundExponent = true;
                break;
            }
            default: {
                result.result = FLO_DESERIALIZATION_BAD_FORMAT;
                return result;
            }
            }
        } else {
            uint64_t *currentPart = foundExponent ? &exponent : &integral;

            uint8_t digit = ch - '0';
            if (*currentPart > (UINT64_MAX - digit) / BASE) {
                result.result = FLO_DESERIALIZATION_OUT_OF_RANGE;
                return result;
            }

            *currentPart = *currentPart * BASE + digit;

            integralCharCount += !(foundExponent);
        }
    })

    if (foundExponent && exponent > 0) {
        // Short circuit => 10^x > UINT64MAX = x > log_10(UINT64MAX) =
        // x > 19.2...
        // We also count integral char count in the short circuit:
        // x * 10^y > UINT64MAX means we can count how many base 10s there are
        // in x and add them to the exponentiation e.g. 1000 * 10^17 =
        // 1 * 10^20, so we short circuit
        if (exponent + (integralCharCount - 1) > 19) {
            result.result = FLO_DESERIALIZATION_OUT_OF_RANGE;
            return result;
        }
        uint64_t exponentiationResult = flo_power(BASE, exponent);
        if (exponentiationResult == 0) {
            result.result = FLO_DESERIALIZATION_OUT_OF_RANGE;
            return result;
        }

        if (integral > UINT64_MAX / exponentiationResult) {
            result.result = FLO_DESERIALIZATION_OUT_OF_RANGE;
            return result;
        }

        integral *= exponentiationResult;
    }

    result.result = FLO_DESERIALIZATION_SUCCESS;
    result.parsed = integral;

    return result;
}
