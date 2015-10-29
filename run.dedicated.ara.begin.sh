################################################################################################################
#Command to restore from the checkpoint (after OS boot), and then read new script, 
#run it using atomic cpu until the end of initilization, then take checkpoint and exit

#Common configurations in the script
GEM5=./parade-test/gem5.opt
OUT_DIR=./parade-test/
BOOT_DIR=./configs/boot/
START_CKPT_DIR=./parade-test/ckpt-1core/
PREFIX=TDLCA
SUFFIX=td
COMMON_CONFIG="configs/example/fs.py --checkpoint-dir=${START_CKPT_DIR} --restore-with-cpu=timing -r 1 -n 1 --l2_size=64kB --num-l2caches=32 --mem-size=2GB --num-dirs=4 --ruby --lcacc --garnet=fixed --topology=Mesh --mesh-rows=4 --work-begin-exit-count=1"

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

for((i=0; i < ${#BENCHS[@]}; i++)) do
    bench=${BENCHS[$i]}
    BENCH_DIR=${OUT_DIR}/${PREFIX}_${bench}/
    SCRIPT_FILE=${BOOT_DIR}/${bench}.${SUFFIX}.rcS
    BENCH_CKPT_DIR=${BENCH_DIR}/ckpt/
    OUT_FILE=${BENCH_DIR}/result.txt
    echo "(time -p ${GEM5} --outdir=${BENCH_DIR} ${COMMON_CONFIG} --script=${SCRIPT_FILE}) >& ${OUT_FILE} &" 
    (time -p ${GEM5} --outdir=${BENCH_DIR} ${COMMON_CONFIG} --script=${SCRIPT_FILE}) >& ${OUT_FILE} & 
done

################################################################################################################
#Command used to do checkpoint, we have to use the hack_back_ckpt.rcS to do checkpoint so that after restoring from checkpoint, it can read our new script
#./parade-test/gem5.opt --outdir=x86-out configs/example/fs.py --cpu-type=atomic -n 1 --mem-size=2GB --script=configs/boot/hack_back_ckpt.rcS
################################################################################################################
