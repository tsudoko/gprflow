#!/bin/awk -f

# we're too lazy to parse c properly, use this style:
#
#	enum fieldname /* enumstr: StructName structfield */ {
# 		SomeValue = 9,
# 		OtherValue,
# 	};

BEGIN {
	print "#include <stdio.h>"
	print "#define _POSIX_SOURCE"
	print "#include <iconv.h>"
	print "#include \"reader.h\""
	print "#include \"route.h\""
}

lastf != FILENAME {
	lastf = FILENAME
	f = FILENAME
	sub("^src/", "", f)
	print "#include \""f"\""
	init = 1
}

$1 == "enum" && $3 == "/*" && $4 == "enumstr:" && $7 == "*/" {
	inside = 1
	print "const char *\n" $2 "str("$5" *v)\n{"
	print "	switch(v->"$6") {"
	next
}

/}/ && inside {
	inside = 0;
	print "	default: return \"???\";"
	print "	}\n}"
}

inside && NF {
	sub(/=.*/, "", $1)
	sub(/,$/, "", $1)
	print "	case "$1": return \""$1"\"; break;"
}
