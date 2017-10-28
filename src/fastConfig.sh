echo starting quick config.
echo Target directory is ASST2

./configure

cd kern/conf

./config ASST2

cd ../compile/ASST2

make depend
make
make install

echo Build is complete
