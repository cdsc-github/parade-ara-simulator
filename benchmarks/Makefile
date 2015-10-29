OPT_MODE := -O3

BUILD_DIR := $(shell pwd)

#SW version#############################################
#NODES += SW_VectorAddSample
###Medical Imaging(MI) benchmarks##############
NODES += SW_Deblur
NODES += SW_Deblur_Pthread
NODES += SW_Denoise
NODES += SW_Denoise_Pthread
NODES += SW_Registration
NODES += SW_Registration_Pthread
NODES += SW_Segmentation
NODES += SW_Segmentation_Pthread
###Medical Imaging(MI) benchmarks##############
###CoMmercial(CM) benchmarks###################
NODES += SW_BlackScholes
NODES += SW_BlackScholes_Pthread
NODES += SW_StreamCluster
NODES += SW_StreamCluster_Pthread
NODES += SW_Swaptions
NODES += SW_Swaptions_Pthread
###CoMmercial(CM) benchmarks###################
###computer VISion(VIS) benchmarks#############
NODES += SW_LPCIP_Desc
NODES += SW_LPCIP_Desc_Pthread
NODES += SW_Texture_Synthesis
NODES += SW_Texture_Synthesis_Pthread
###computer VISion(VIS) benchmarks#############
###NAVigation(NAV) benchmarks##################
NODES += SW_Robot_Localization
NODES += SW_Robot_Localization_Pthread
NODES += SW_Disparity_Map
NODES += SW_Disparity_Map_Pthread
NODES += SW_EKF_SLAM
NODES += SW_EKF_SLAM_Pthread
###NAVigation(NAV) benchmarks##################
#SW version#############################################

#TDLCA(DEDICATED_ARA) version###############################
NODES += TDLCA_VectorAddSample
###Medical Imaging(MI) benchmarks##############
NODES += TDLCA_Deblur_Modified
NODES += TDLCA_Denoise
NODES += TDLCA_Registration_Modified
NODES += TDLCA_Segmentation
###Medical Imaging(MI) benchmarks##############
###CoMmercial(CM) benchmarks###################
NODES += TDLCA_BlackScholes
NODES += TDLCA_StreamCluster
NODES += TDLCA_Swaptions
###CoMmercial(CM) benchmarks###################
###computer VISion(VIS) benchmarks#############
NODES += TDLCA_LPCIP_Desc
NODES += TDLCA_Texture_Synthesis
###computer VISion(VIS) benchmarks#############
###NAVigation(NAV) benchmarks##################
NODES += TDLCA_Robot_Localization
NODES += TDLCA_Disparity_Map
NODES += TDLCA_EKF_SLAM
###NAVigation(NAV) benchmarks##################
#TDLCA(DEDICATED_ARA) version###############################

.PHONY : clean

all : bench.out $(addprefix Nodes/,$(NODES))

$(addprefix Nodes/,$(NODES)) : Benchmarks/$(basename $(notdir $@))/$(notdir $(@:=/*))  BenchmarkNode.h ObjectFiles/SimicsHeader.o ObjectFiles/m5op.o
	g++ $(OPT_MODE) -lm -lpthread -lrt -I./ ObjectFiles/SimicsHeader.o ObjectFiles/m5op.o Benchmarks/$(basename $(notdir $@))/$(notdir $(@:=/*.cpp)) -o $@
	@mkdir -p WorkingDir/$(basename $(notdir $@))
	@echo $(notdir $@) complete

bench.out : main.cpp
	g++ -O3 main.cpp -o bench.out
	@echo Benchmark core complete

ObjectFiles/SimicsHeader.o : SimicsHeader.cpp
	g++ -O0 -c SimicsHeader.cpp -o ObjectFiles/SimicsHeader.o
	@echo Simics Header complete

ObjectFiles/m5op.o : m5op_x86.S
	g++ -O0 -c m5op_x86.S -o ObjectFiles/m5op.o

clean:
	-rm bench.out
	-rm ObjectFiles/*
	-rm Nodes/*
