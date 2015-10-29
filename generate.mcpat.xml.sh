################################################################################################################
#Command to parse the stats and config to mcpat.xml
#Common configurations in the script
PREFIX=$1
VERSION=$2
BASE_DIR=./parade-${PREFIX}-${VERSION}/
MCPAT_TEMPLATE=mcpat-parade.xml

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
    echo "perl mcpat-script/m5-mcpat.pl ${BENCH_DIR}/stats.txt ${BENCH_DIR}/config.ini mcpat-script/${MCPAT_TEMPLATE} > ${BENCH_DIR}/mcpat.xml"
    perl mcpat-script/m5-mcpat.pl ${BENCH_DIR}/stats.txt ${BENCH_DIR}/config.ini mcpat-script/${MCPAT_TEMPLATE} > ${BENCH_DIR}/mcpat.xml
done
