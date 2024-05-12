#!/usr/bin/env fish

echo > TODO

set files kernel/src klibc/src kernel/include klibc/include
set afiles

for f in $files
  set afiles $afiles $(find $f -type f)
end

echo $afiles

for af in $afiles
  set cnt $(cat $af | grep TODO) $(cat $af | grep FIXME)
  if count $cnt > /dev/null
    echo $af ":" $cnt >> TODO
  end
end
