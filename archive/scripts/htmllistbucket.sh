
TABLE_DATA=$(find $1 -type f -not -name ".*" -printf "%u %U %s %p\n" |
    sed -e "s/ \.\// /" - |
    grep -v "index.html" |
    while IFS=' ' read -r user uid size name
do
    printf "<tr> \n\
    <td><a href=\"%s\">%s</a></td>\n\
    <td>%s</td>\n\
    <td>%s</td>\n\
</tr>\n" "$name" "$name" "$user" "$size"
done); export TABLE_DATA

cat .template.html | envsubst
