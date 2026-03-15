# Arduino library for AS7421

This is an Arduino library for the [AS7421](https://ams.com/en/as7421) 64-channel NIR sensor.

The library is based on the datasheet and some reverse engineering to determine the correct register settings and
interpretation of the data. Considering both production and support has been dropped I felt it is useful to have a library
to interface with the sensor

## Limitations

As this is a reverse engineered library, there are some limitations:

### SMUX configuration

The smux configuration is only partially known. Each of the RAM ragisters for a particular channel is assigned to two pixels and a nibble is used
to select which ADC is connected to the pixel. There are 4 ADCs that can be assigned for each integration channels ``A``, ``B``, ``C`` and ``D``.
For example, setting RAM location 0 as 0x21 will map two photodiodes to ADC 2 and ADC 1. At least thats how it looks. However, the actual
mapping of photodiodes to each of the RAM locations is not known. 

### Wavelength assignment

Wavelength assignment was determined using a monochromator. Accuracy is not guaranteed.

### Calibration

The sensor does require some form of calibration to maximise data and unfortunately this procedure is not known.

### License

This library is licensed under the MIT license. See [LICENSE.md](LICENSE.md) for more details.
