// extend: keep old
// truncate: do not distinguish signed and unsigned
void extend_signed_to_signed()
{
    char small;
    int big;

    big = (int) small;
}

void extend_signed_to_unsigned()
{
    char small;
    unsigned int big;

    big = (unsigned int) small;
}

void extend_unsigned_to_signed()
{
    unsigned char small;
    int big;

    big = (int) small;
}

void extend_unsigned_to_unsigned()
{
    unsigned char small;
    unsigned int big;

    big = (unsigned int) small;
}

void truncate_signed_to_signed()
{
    char small;
    int big;

    small = (char) big;
}

void truncate_signed_to_unsigned()
{
    unsigned char small;
    int big;

    small = (unsigned char) big;
}

void truncate_unsigned_to_signed()
{
    char small;
    unsigned int big;

    small = (char) big;
}

void truncate_unsigned_to_unsigned()
{
    unsigned char small;
    unsigned int big;

    small = (unsigned char) big;
}

