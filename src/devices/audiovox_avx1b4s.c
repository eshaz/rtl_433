/** @file
    Audiovox AVX1BS4 - Car Remote.

    Copyright (C) 2023 Ethan Halsall

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.
*/
/** @fn int audiovox_avx1b4s_decode(r_device *decoder, bitbuffer_t *bitbuffer)
Audiovox AVX1BS4 - Car Remote

Manufacturer:
- Audiovox

Supported Models:
- ATCD-1
- AVX1BS4, AVX-1BS4 (FCC ID ELVATCC)
- A1BTX (FCC ID ELVATFE)

Data structure:

Audiovox Type 4 and Code Alarm Type 7 Transmitters

Transmitter uses a rolling code that changes between each button press.
The same code is continuously repeated while button is held down.

|length-|description----|example--
|{20}   |Transmitter ID |0x3c93f
|{28}   |Rolling Code   |0x0933227

*/

#include "decoder.h"

static int audiovox_avx1b4s_decode(r_device *decoder, bitbuffer_t *bitbuffer)
{
    int id   = 0;
    int code = 0;

    if (bitbuffer->bits_per_row[0] > 48 || bitbuffer->bits_per_row[0] < 44) {
        return DECODE_ABORT_LENGTH;
    }

    uint8_t *bytes = bitbuffer->bb[0];

    id = (bytes[0] << 20) |
         (bytes[1] << 12) |
         (bytes[2] >> 4);

    code = ((bytes[2] & 0x0f) << 20) |
           (bytes[3] << 16) |
           (bytes[4] << 8) |
           bytes[5];

    if (id == 0 || code == 0) {
        return DECODE_ABORT_EARLY;
    }

    /* clang-format off */
    data_t *data = data_make(
            "model",    "model",        DATA_STRING, "AVX1B4S-CarRemote",
            "id",       "device-id",    DATA_INT,    id,
            "code",     "code",         DATA_INT,    code,
            NULL);
    /* clang-format on */

    decoder_output_data(decoder, data);
    return 1;
}

static char const *const output_fields[] = {
        "model",
        "id",
        "code",
        NULL,
};

r_device const audiovox_avx1b4s = {
        .name        = "Audiovox AVX1B4S car key",
        .modulation  = OOK_PULSE_MANCHESTER_ZEROBIT,
        .short_width = 550,
        .long_width  = 550,
        .reset_limit = 1290,
        .decode_fn   = &audiovox_avx1b4s_decode,
        .fields      = output_fields,
};