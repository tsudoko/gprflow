#!/bin/awk -f

# we're too lazy to parse c properly, use this style:
#
#	enum fieldname /* enumstr: StructName structfield */ {
# 		SomeValue = 9,
# 		OtherValue,
# 	};

$1 == "enum" && $3 == "/*" && $4 == "enumstr:" && $8 == "*/" {
	inside = 1
	prefix = $7
	print "const char *\n" $2 "str("$5" *v)\n{"
	print "	switch(v->"$6") {"
	next
}

/}/ && inside {
	inside = 0
	print "	default: return \"???\";"
	print "	}\n}\n"
}

inside && NF {
	sub(/=.*/, "", $1)
	sub(/,$/, "", $1)
	printf "	case "$1": "
	sub("^"prefix, "", $1)
	print "return \""$1"\"; break;"
}
