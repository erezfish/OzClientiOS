################
##  Linux 64  ##
################
# Build on a 64bit linux machine

#WARNNIG: libOsnClient doesn't exist in SVN!!!
#You have to put LibOsnClient.so in ../../lib/Linux

export BUILD_DIR=$PWD

\rm -Rf linux64 ; mkdir linux64 ; cd linux64  ; cmake -DTARGET=linux64 ../../ ; make VERBOSE=1 

make
make VERBOSE=1


echo
echo =====================
echo Linux output files:
echo =====================
export OUT_LIB=${BUILD_DIR}/linux64/OzCloud
printf "Library: "
ls ${OUT_LIB}/lib* -l --time-style=long-iso | awk '{print $5, $6, $7, $8, $9}'
echo

cp $OUT_LIB/libOzClient.so ../../run
cp $OUT_LIB/libOzClient.so ../../../lib/Linux

echo ==================================================================



