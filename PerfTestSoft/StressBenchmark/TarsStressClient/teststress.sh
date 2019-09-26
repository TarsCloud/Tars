#!/bin/bash

function get_time()
{
    start=$1
    end=$2

    start_s=`echo $start | cut -d '.' -f 1`
    start_ns=`echo $start | cut -d '.' -f 2`
    end_s=`echo $end | cut -d '.' -f 1`
    end_ns=`echo $end | cut -d '.' -f 2`

    time_ms=$(( (10#$end_s-10#$start_s)*1000 + (10#$end_ns/1000000 - 10#$start_ns/1000000) ))
    #time_ms=`expr $time_micro/1000  | bc `

    echo "total time $time_ms ms"
}

if [[ $# -lt 3 ]];then
  echo "Usage: $0 [ProcessNum] [ThreadNum] [size]"
  exit 1
fi

begin_time=$(date +%s.%N)
#ct=1500000
ct=1500000

for((i=1; i<=$1; i++)); do {
   ./TarsStressClient $2 $ct $3
} & done
wait

end_time=$(date +%s.%N)
get_time $begin_time $end_time

