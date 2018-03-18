#!/bin/awk -f

# we're too lazy to parse c properly, use this style:
#
#	enum fieldname /* enumstr: StructName structfield */ {
# 		SomeValue = 9,
# 		OtherValue,
# 	};

lastf != FILENAME {
	lastf = FILENAME
	f = FILENAME
	sub("^src/", "", f)
	print "#include \""f"\"\n"
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
	print "	}\n}\n"
}

inside && NF {
	sub(/=.*/, "", $1)
	sub(/,$/, "", $1)
	print "	case "$1": return \""$1"\"; break;"
}
