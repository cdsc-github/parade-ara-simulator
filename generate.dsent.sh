################################################################################################################
#export variables for CMake
export CC=gcc
export CXX=g++
#Command to parse the stats and config to mcpat.xml
#Common configurations in the script
PREFIX=$1
VERSION=$2
BASE_DIR=./parade-${PREFIX}-${VERSION}/
MCPAT_TEMPLATE=mcpat-parade.xml
PYTHON=/curr/zhenman/simulation/bin/python-2.7.8/bin/python
SCRIPT=util/on-chip-network-power-area.py
ROUTER_CFG=ext/dsent/configs/router.cfg
LINK_CFG=ext/dsent/configs/electrical-link.cfg
RESULT_FILE=dsent.txt

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
    echo "$PYTHON $SCRIPT $M5_PATH ${BENCH_DIR} $ROUTER_CFG $LINK_CFG >& ${BENCH_DIR}"/"$RESULT_FILE"
    $PYTHON $SCRIPT $M5_PATH ${BENCH_DIR} $ROUTER_CFG $LINK_CFG >& ${BENCH_DIR}"/"$RESULT_FILE
done
