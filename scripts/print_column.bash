od -An -t u8 -v $1 | awk '{for(i=1; i<=NF; i++) print $i}'
