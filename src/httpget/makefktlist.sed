s/\([a-z0-9_.]*\) \([a-z_]*\) \([a-z0-9_]*\)\([(].*[)]\)/\/\* \3, \1 \*\/ \2 \3\4;/
s/\([a-z0-9_.]*\) \([a-z_]*\) \([a-z]*\) \([a-z0-9_]*\)\([(].*[)]\)/\/\* \4, \1 \*\/ \2 \3 \4\5;/
s/\([a-z0-9_.]*\) \([a-z_]*\) [*]\([a-z0-9_]*\)\([(].*[)]\)/\/\* \3, \1 \*\/ \2 *\3\4;/
s/\([a-z0-9_.]*\) \([a-z_]*\) \([a-z]*\) [*]\([a-z0-9_]*\)\([(].*[)]\)/\/\* \4, \1 \*\/ \2 \3 *\4\5;/
