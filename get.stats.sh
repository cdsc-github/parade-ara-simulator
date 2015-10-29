################################################################################################################
#Common configurations in the script
PREFIX=$1
VERSION=$2
BASE_DIR=./parade-${PREFIX}-${VERSION}/
if [[ ${VERSION} == begin || ${VERSION} == comp-begin ]]; then
    Cycle="system.cpu.numCycles"
else
    Cycle="system.switch_cpus_1.numCycles"
fi
Instrs="sim_insts"
Ops="sim_ops"
SimTime="sim_seconds"
HostTime="real"

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
    printf "${bench}\t"
    BENCH_DIR=${BASE_DIR}/${PREFIX}_${bench}/
    _FILENAME=${BENCH_DIR}/stats.txt
    if grep "${Cycle}" ${_FILENAME} > tmp
    then
        grep "${Cycle}" ${_FILENAME} | awk '{printf "\t%s", $2}{if($2=="")printf "\t\terror"}'
    else
        echo -n -e "\terror"
    fi
    if grep "${Instrs}" ${_FILENAME} > tmp
    then
        grep "${Instrs}" ${_FILENAME} | awk '{printf "\t%s", $2}{if($2=="")printf "\t\terror"}'
    else
        echo -n -e "\terror"
    fi
    if grep "${Ops}" ${_FILENAME} > tmp
    then
        grep "${Ops}" ${_FILENAME} | awk '{printf "\t%s", $2}{if($2=="")printf "\t\terror"}'
    else
        echo -n -e "\terror"
    fi
    if grep "${SimTime}" ${_FILENAME} > tmp
    then
        grep "${SimTime}" ${_FILENAME} | awk '{printf "\t%s", $2}{if($2=="")printf "\t\terror"}'
    else
        echo -n -e "\terror"
    fi
    _FILENAME=${BENCH_DIR}/result.txt
    if grep "${HostTime}" ${_FILENAME} > tmp
    then
        grep "${HostTime}" ${_FILENAME} | awk '{printf "\t%s", $2}{if($2=="")printf "\t\terror"}'
    else
        echo -n -e "\terror"
    fi
    echo ""
done
