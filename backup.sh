################################################################################################################
#Common configurations in the script
NAME=$1
VERSION=$2
FROM_DIR=./parade-test/
TO_DIR=./${NAME}-${VERSION}/

BENCHS=(
#### Medical Imaging #####
    'Deblur_Modified'
    'Denoise'
    'Registration_Modified'
    'Segmentation'
#### Commercial ##########
    'BlackScholes'
    'StreamCluster'
    'Swaptions'
#### Computer vision #####
    'Disparity_Map'
    'LPCIP_Desc'
#### Navigation ##########
    'Robot_Localization'
    'EKF_SLAM'
)

echo "rm -rf ${TO_DIR}"
rm -rf ${TO_DIR}
echo "mkdir ${TO_DIR}"
mkdir ${TO_DIR}

PREFIX=TDLCA

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
