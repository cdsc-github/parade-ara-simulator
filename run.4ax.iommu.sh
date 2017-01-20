################################################################################################################
#Command to restore from the checkpoint (after OS boot), and then read new script,
#run it using atomic cpu until the end of initilization, then take checkpoint and exit

# Common configurations in the script
GEM5=gem5.opt
TARGET_DIR=./result/
BOOT_DIR=./configs/boot
START_CKPT_DIR=ckpt-1core
PREFIX=TDLCA
SUFFIX=td
DEBUG_FLAG="--debug-flags=PageTableWalker"
COMMON_CONFIG="../configs/example/fs_tlb.py --checkpoint-dir=${START_CKPT_DIR} \
    --restore-with-cpu=detailed -r 1 -n 1 --l2_size=64kB --num-l2caches=32 \
    --mem-size=2GB --num-dirs=4 --ruby --lcacc --garnet=fixed --topology=Mesh \
    --mesh-rows=4 --num_accinstances=4 --td_tlb_size=1 --lcacc_tlb_size=1 \
    --lcacc_tlb_mshr=0 --iommu"

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

for((i=0; i < ${#BENCHS[@]}; i++)) do
    bench=${BENCHS[$i]}
    SCRIPT_FILE=${BOOT_DIR}/${bench}.${SUFFIX}.rcS
    OUT_FILE=${TARGET_DIR}/${PREFIX}_${bench}/result.txt
    echo "(time -p ${GEM5} --outdir=${TARGET_DIR}/${PREFIX}_${bench} \
        ${COMMON_CONFIG} --acc_type=${BENCHS[$i]} --work-end-exit-count=1 \
        --script=${SCRIPT_FILE}) >& ${OUT_FILE} &"
    (time -p ${GEM5} --outdir=${TARGET_DIR}/${PREFIX}_${bench} \
        ${COMMON_CONFIG} --acc_types=${bench} --work-end-exit-count=1 \
        --script=${SCRIPT_FILE}) >& ${OUT_FILE} &
done

