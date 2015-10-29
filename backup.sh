################################################################################################################
#Common configurations in the script
PREFIX=$1
VERSION=$2
FROM_DIR=./parade-test/
TO_DIR=./parade-${PREFIX}-${VERSION}/

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

echo "rm -rf ${TO_DIR}"
rm -rf ${TO_DIR}
echo "mkdir ${TO_DIR}"
mkdir ${TO_DIR}

for bench in ${BENCHS[@]}; do
    FROM_BENCH_DIR=${FROM_DIR}/${PREFIX}_${bench}/
    TO_BENCH_DIR=${TO_DIR}/${PREFIX}_${bench}/
    echo "rm -rf ${TO_BENCH_DIR}"
    rm -rf ${TO_BENCH_DIR}
    echo "mkdir ${TO_BENCH_DIR}"
    mkdir ${TO_BENCH_DIR}
    echo "cp ${FROM_BENCH_DIR}/config.ini ${FROM_BENCH_DIR}/config.json ${FROM_BENCH_DIR}/result.txt ${FROM_BENCH_DIR}/stats.txt ${FROM_BENCH_DIR}/system.pc.com_1.terminal ${TO_BENCH_DIR}"
    cp ${FROM_BENCH_DIR}/config.ini ${FROM_BENCH_DIR}/config.json ${FROM_BENCH_DIR}/result.txt ${FROM_BENCH_DIR}/stats.txt ${FROM_BENCH_DIR}/system.pc.com_1.terminal ${TO_BENCH_DIR}
done
