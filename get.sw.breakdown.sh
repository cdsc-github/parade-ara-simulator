################################################################################################################
#Common configurations in the script
PREFIX=$1
VERSION=$2
BASE_DIR=./parade-${PREFIX}-${VERSION}/

function print_stats() {
    if grep "$1" ${_FILENAME} > tmp
    then
        grep "$1" ${_FILENAME} | awk '{printf "\t%s", $2}{if($2=="")printf "\terror"}'
    else
        echo -n -e "\terror"
    fi
}

BENCHS=(
###Medical Imaging(MI) benchmarks##############
    'Deblur_Modified'
    'Denoise'
    'Registration_Modified'
    'Segmentation'
###CoMmercial(CM) benchmarks###################
    'BlackScholes'
    'StreamCluster'
    'Swaptions'
###computer VISion(VIS) benchmarks#############
    'LPCIP_Desc'
    'Texture_Synthesis'
###NAVigation(NAV) benchmarks##################
    'Robot_Localization'
    'Disparity_Map'
    'EKF_SLAM'
)

for bench in ${BENCHS[@]}; do
    BENCH_DIR=${BASE_DIR}/${PREFIX}_${bench}/
    _FILENAME=${BENCH_DIR}/stats.txt
    printf "${bench}:"

    # get number of fetch cycles: total cycles, icacheStallCycles
#    printf "\nFETCH_CYCLES"
#    FETCH_CYCLES="system.switch_cpus_1.fetch.Cycles"
#    print_stats ${FETCH_CYCLES}
#    FETCH_STALL_CYCLES="system.switch_cpus_1.fetch.icacheStallCycles"
#    print_stats ${FETCH_STALL_CYCLES}

    # get number of decode cycles: total cycles, icacheStallCycles
#    printf "\nFETCH_CYCLES"
#    FETCH_CYCLES="system.switch_cpus_1.fetch.Cycles"
#    print_stats ${FETCH_CYCLES}
#    FETCH_STALL_CYCLES="system.switch_cpus_1.fetch.icacheStallCycles"
#    print_stats ${FETCH_STALL_CYCLES}

    # get number of L1D hits
    printf "\nL1D_HITS"
    for(( i=0; i < 1; i++)); do
	L1D_HITS="system.ruby.l1_cntrl"${i}".L1Dcache.demand_hits"
	print_stats ${L1D_HITS}
    done

    # get number of L1D misses
    printf "\nL1D_MISSES"
    for(( i=0; i < 1; i++)); do
	L1D_MISSES="system.ruby.l1_cntrl"${i}".L1Dcache.demand_misses"
	print_stats ${L1D_MISSES}
    done

    # get number of L1I hits
    printf "\nL1I_HITS"
    for(( i=0; i < 1; i++)); do
	L1I_HITS="system.ruby.l1_cntrl"${i}".L1Icache.demand_hits"
	print_stats ${L1I_HITS}
    done

    # get number of L1I misses
    printf "\nL1I_MISSES"
    for(( i=0; i < 1; i++)); do
	L1I_MISSES="system.ruby.l1_cntrl"${i}".L1Icache.demand_misses"
	print_stats ${L1I_MISSES}
    done

    # get number of L2 hits
    printf "\nL2_HITS"
    for(( i=0; i < 32; i++)); do
	L2_HITS="system.ruby.l2_cntrl"${i}".L2cache.demand_hits"
	print_stats ${L2_HITS}
    done

    # get number of L2 misses
    printf "\nL2_MISSES"
    for(( i=0; i < 32; i++)); do
	L2_MISSES="system.ruby.l2_cntrl"${i}".L2cache.demand_misses"
	print_stats ${L2_MISSES}
    done

    # get number of reads for all memory controllers
    printf "\nMEM_READS"
    for(( i=0; i < 4; i++)); do
	MEM_READS="system.mem_ctrls"${i}".num_reads::total"
	print_stats ${MEM_READS}
    done

    # get number of writes for all memory controllers
    printf "\nMEM_WRITES"
    for(( i=0; i < 4; i++)); do
	MEM_WRITES="system.mem_ctrls"${i}".num_writes::total"
	print_stats ${MEM_WRITES}
    done

    printf "\n"
done

