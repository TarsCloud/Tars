# Language: bash
for((i=1; i<=$1; i++)); do {
        ./TarsStressClient     $2   1500000  Test.StressServer.StressObj  $3
} & done
wait
