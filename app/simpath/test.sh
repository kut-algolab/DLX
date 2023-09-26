#!/bin/bash

make simpath

bench_dir=~/Git/kut-algolab/ICGCA23/icgca-benchmarks/one_pair
sample_case=$bench_dir/$(printf "%03d".col "$1")

tmp_case=tmp.col
sum=0
i=$(grep l $sample_case | sed "s/l //")
printf "%03d".col\\n "$1"
for len in `seq  1 $i`
do
    sed "s/l $i/l $len/" $sample_case > $tmp_case
    ## python3.9 mk_dlx_instance.py < $tmp_case > sample.xcc
    echo "len $len"
    
    ## ans=$(./simpath < sample.xcc)
    ## ./simpath < sample.xcc > foo

    python3.9 mk_dlx_instance.py < $tmp_case | ./simpath > foo
    
    cat foo
    ans=$(cat foo | grep sols | sed "s/# of sols = //")
    sum=$(($sum + $ans))
done

echo $sum

