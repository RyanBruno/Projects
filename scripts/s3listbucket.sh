
echo "<ListBucketResult>"

NEWLINE="\n"
TAB="\t"

if [ -e FALSE ]
then
    NEWLINE=
    TAB=
fi

find . -type f -printf "%u %U %s %p\n" | #sed -e 's/^\.\///' - |
    while IFS=' ' read -r user uid size name
do
    printf "$TAB<Contents>$NEWLINE"
    printf "$TAB$TAB<Key>%s</Key>$NEWLINE" "$name"
    printf "$TAB$TAB<Owner>$NEWLINE"
    printf "$TAB$TAB$TAB<DisplayName>%s</DisplayName>$NEWLINE" "$user"
    printf "$TAB$TAB$TAB<ID>%s</ID>$NEWLINE" "$uid"
    printf "$TAB$TAB</Owner>$NEWLINE"
    printf "$TAB$TAB<Size>%s</Size>$NEWLINE" "$size"
    printf "$TAB</Contents>$NEWLINE"
done

echo "</ListBucketResult>"
