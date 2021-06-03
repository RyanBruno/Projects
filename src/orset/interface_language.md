
key => uint64\_t
value => string

key add(value)
key remove(key)

Bits
[  8  ]|[         n        ]
OpCode | Operation Dependant

Operations:

add
[   8   ]|[  n  ]
00000001 | string

remove
[   8   ]|[ 64 ]
00000010 | key

string
[ 16  ]|[ length ]
Length | Charaters

key (big-endian)
[      8       ]|[8]|[8]|[8]|[8]|[      8       ]
Most significant|...|...|...|...|Least significant

response
[64]
key

