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

    # get number of spm reads
    printf "\nSPM_READS"
    SPM_READS="system.ruby.num_spm_reads"
    print_stats ${SPM_READS}

    # get number of spm writes
    printf "\nSPM_WRITES"
    SPM_WRITES="system.ruby.num_spm_writes"
    print_stats ${SPM_WRITES}

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

