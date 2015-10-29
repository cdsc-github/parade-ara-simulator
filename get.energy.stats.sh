################################################################################################################
#Common configurations in the script
PREFIX=$1
VERSION=$2
BASE_DIR=./parade-${PREFIX}-${VERSION}/

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
    _MCPAT_FILE=${BENCH_DIR}/mcpat.energy
    _DSENT_FILE=${BENCH_DIR}/dsent.txt
    _STATS_FILE=${BENCH_DIR}/stats.txt
    printf "${bench}:"

    # get Subthreshold Leakage for Core and L2
    Subthreshold_Leakage="Subthreshold Leakage"
    printf "\nSubthreshold_Leakage"
    if grep "${Subthreshold_Leakage}" ${_MCPAT_FILE} > tmp1
    then
	sed -n '2,3p' tmp1 > tmp2
        grep "${Subthreshold_Leakage}" tmp2 | awk '{printf "\t%s", $4}{if($4=="")printf "\t\terror"}'
    else
        echo -n -e "\terror"
    fi

    # get Gate Leakage for Core and L2
    Gate_Leakage="Gate Leakage"
    printf "\nGate_Leakage"
    if grep "${Gate_Leakage}" ${_MCPAT_FILE} > tmp1
    then
	sed -n '2,3p' tmp1 > tmp2
        grep "${Gate_Leakage}" tmp2 | awk '{printf "\t%s", $4}{if($4=="")printf "\t\terror"}'
    else
        echo -n -e "\terror"
    fi

    # get Runtime Dynamic for Core and L2
    Runtime_Dynamic="Runtime Dynamic"
    printf "\nRuntime_Dynamic"
    if grep "${Runtime_Dynamic}" ${_MCPAT_FILE} > tmp1
    then
	sed -n '2,3p' tmp1 > tmp2
        grep "${Runtime_Dynamic}" tmp2 | awk '{printf "\t%s", $4}{if($4=="")printf "\t\terror"}'
    else
        echo -n -e "\terror"
    fi

    # get Router power
    Router_total_power="Router_total_power"
    printf "\nRouter_total_power"
    if grep "${Router_total_power}" ${_DSENT_FILE} > tmp
    then
        grep "${Router_total_power}" ${_DSENT_FILE} | awk '{printf "\t%s", $2}{if($2=="")printf "\t\terror"}'
    else
        echo -n -e "\terror"
    fi

    # get Link power
    Link_total_power="Link_total_power"
    printf "\nLink_total_power"
    if grep "${Link_total_power}" ${_DSENT_FILE} > tmp
    then
        grep "${Link_total_power}" ${_DSENT_FILE} | awk '{printf "\t%s", $2}{if($2=="")printf "\t\terror"}'
    else
        echo -n -e "\terror"
    fi

    # get Dram power
    Dram_total_power="averagePower"
    printf "\nDram_total_power"
    if grep "${Dram_total_power}" ${_STATS_FILE} > tmp
    then
        grep "${Dram_total_power}" ${_STATS_FILE} | awk '{printf "\t%s", $2}{if($2=="")printf "\t\terror"}'
    else
        echo -n -e "\terror"
    fi

    printf "\n"
done

