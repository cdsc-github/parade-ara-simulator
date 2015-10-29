################################################################################################################
#Command used to do checkpoint, we have to use the hack_back_ckpt.rcS to do checkpoint so that after restoring from checkpoint, it can read our new script
./parade-test/gem5.opt --outdir=x86-out configs/example/fs.py --cpu-type=atomic -n 1 --mem-size=2GB --script=configs/boot/hack_back_ckpt.rcS
################################################################################################################
