options gen2
module tomlc17 shared public

require tomlc17_core public

// -----------------------------------------------------------------------
// Timestamp struct — structured view of DATE / TIME / DATETIME datums
// -----------------------------------------------------------------------

struct TomlTimestamp {
    year    : int16
    month   : int16
    day     : int16
    hour    : int16
    minute  : int16
    second  : int16
    usec    : int
    tz      : int16
}

// -----------------------------------------------------------------------
// TomlValue variant
//
// _array / _table / date-time cases carry the raw TomlDatum for lazy
// traversal.  Use toml_datum_arr_* / toml_datum_tab_* / datum_timestamp()
// to unpack further.
// -----------------------------------------------------------------------

variant TomlValue {
    _unknown    : void?
    _string     : string
    _int        : int64
    _float      : double
    _bool       : bool
    _date       : TomlDatum
    _time       : TomlDatum
    _datetime   : TomlDatum
    _datetimetz : TomlDatum
    _array      : TomlDatum
    _table      : TomlDatum
}

// -----------------------------------------------------------------------
// datum_value — convert a raw TomlDatum into a typed TomlValue
// -----------------------------------------------------------------------

def datum_value(d : TomlDatum) : TomlValue {
    let t : TomlType = d.typev
    if (t == TomlType.TOML_STRING) {
        return TomlValue(_string = string(toml_datum_str(d)))
    } elif (t == TomlType.TOML_INT64) {
        return TomlValue(_int = toml_datum_int(d))
    } elif (t == TomlType.TOML_FP64) {
        return TomlValue(_float = toml_datum_fp(d))
    } elif (t == TomlType.TOML_BOOLEAN) {
        return TomlValue(_bool = toml_datum_bool(d))
    } elif (t == TomlType.TOML_DATE) {
        return TomlValue(_date = d)
    } elif (t == TomlType.TOML_TIME) {
        return TomlValue(_time = d)
    } elif (t == TomlType.TOML_DATETIME) {
        return TomlValue(_datetime = d)
    } elif (t == TomlType.TOML_DATETIMETZ) {
        return TomlValue(_datetimetz = d)
    } elif (t == TomlType.TOML_ARRAY) {
        return TomlValue(_array = d)
    } elif (t == TomlType.TOML_TABLE) {
        return TomlValue(_table = d)
    }
    return TomlValue(_unknown = null)
}

// -----------------------------------------------------------------------
// datum_timestamp — extract structured time fields from a date/time datum
// -----------------------------------------------------------------------

def datum_timestamp(d : TomlDatum) : TomlTimestamp {
    return TomlTimestamp(
        year   = toml_datum_ts_year(d),
        month  = toml_datum_ts_month(d),
        day    = toml_datum_ts_day(d),
        hour   = toml_datum_ts_hour(d),
        minute = toml_datum_ts_minute(d),
        second = toml_datum_ts_second(d),
        usec   = toml_datum_ts_usec(d),
        tz     = toml_datum_ts_tz(d)
    )
}

// -----------------------------------------------------------------------
// Convenience — typed value directly from a table datum
// -----------------------------------------------------------------------

def toml_get_value(tablev : TomlDatum, key : string) : TomlValue {
    return datum_value(toml_get(tablev, key))
}

def toml_seek_value(tablev : TomlDatum; key : string) : TomlValue {
    return datum_value(toml_seek(tablev, key))
}

// -----------------------------------------------------------------------
// Utility functions
// -----------------------------------------------------------------------

def toml_append_paths(a, b : string) : string {
    return string(a + "." + b)
}
